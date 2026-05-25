// pybind
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Libs/Application/Job/PythonWorker.h>
#include <Logging.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace shapeworks {

//---------------------------------------------------------------------------
class PythonLogger {
 public:
  void set_callback(const std::function<void(std::string)>& callback) { callback_ = callback; }

  void set_progress_callback(const std::function<void(double, std::string)>& callback) {
    progress_callback_ = callback;
  }

  void cpp_log(std::string msg) { callback_(msg); }

  void cpp_progress(double progress, std::string message) { progress_callback_(progress * 100, message); }

  void clear_abort() { aborted_ = false; }

  void set_abort() { aborted_ = true; }

  bool check_abort() { return aborted_; }

  bool is_cli_mode() { return is_cli_mode_; }

  void set_cli_mode(bool cli) { is_cli_mode_ = cli; }

 private:
  std::function<void(std::string)> callback_;
  std::function<void(double, std::string)> progress_callback_;

  std::atomic<bool> aborted_{false};

  //! Command line interface mode
  std::atomic<bool> is_cli_mode_{false};
};

//---------------------------------------------------------------------------
PYBIND11_EMBEDDED_MODULE(logger, m) {
  py::class_<PythonLogger, std::shared_ptr<PythonLogger>>(m, "PythonLogger")
      .def(py::init<>())
      .def("log", &PythonLogger::cpp_log)
      .def("check_abort", &PythonLogger::check_abort)
      .def("progress", &PythonLogger::cpp_progress)
      .def("is_cli_mode", &PythonLogger::is_cli_mode);
};

//---------------------------------------------------------------------------
PythonWorker::PythonWorker() {
  python_logger_ = QSharedPointer<PythonLogger>::create();

  qRegisterMetaType<QSharedPointer<Job>>("QSharedPointer<Job>");

  // create singular Python thread and move this object to the new thread
  thread_ = new QThread(this);
  moveToThread(thread_);
  thread_->start();
}

//---------------------------------------------------------------------------
PythonWorker::~PythonWorker() {
  end_python();
  if (thread_) {
    thread_->wait();
    delete thread_;
  }
}

//---------------------------------------------------------------------------
void PythonWorker::set_vtk_output_window(vtkSmartPointer<ShapeWorksVtkOutputWindow> output_window) {
  studio_vtk_output_window_ = output_window;
}

//---------------------------------------------------------------------------
void PythonWorker::set_cli_mode(bool cli_mode) { python_logger_->set_cli_mode(cli_mode); }

//---------------------------------------------------------------------------
void PythonWorker::start_job(QSharedPointer<Job> job) {
  if (init()) {
    try {
      // Invalidate import caches in case packages were installed since init()
      // (e.g. PyTorch on-demand install)
      py::module::import("importlib").attr("invalidate_caches")();

      job->start_timer();
      if (!job->get_quiet_mode()) {
        SW_LOG("Running Task: " + job->name().toStdString());
      }
      Q_EMIT job->progress(0);
      current_job_ = job;
      current_job_->execute();
      current_job_->set_complete(true);
      if (!job->get_quiet_mode()) {
        SW_LOG(current_job_->get_completion_message().toStdString());
      }
    } catch (py::error_already_set& e) {
      SW_ERROR("{}", e.what());
    }
  }

  python_logger_->clear_abort();
  if (current_job_) {
    Q_EMIT current_job_->finished();
  }
}

//---------------------------------------------------------------------------
void PythonWorker::run_job(QSharedPointer<Job> job) {
  job->moveToThread(thread_);
  // run on python thread
  QMetaObject::invokeMethod(this, "start_job", Qt::QueuedConnection, Q_ARG(QSharedPointer<Job>, job));
}

//---------------------------------------------------------------------------
void PythonWorker::set_current_job(QSharedPointer<Job> job) { current_job_ = job; }

//---------------------------------------------------------------------------
bool PythonWorker::init() {
  std::string script = "install_shapeworks.sh";
#ifdef _WIN32
  script = "install_shapeworks.bat";
#endif

  if (initialized_) {
    if (!initialized_success_) {
      SW_ERROR("Unable to initialize Python.  Please run " + script);
    }
    return initialized_success_;
  }
  initialized_ = true;

  SW_LOG("Initializing Python!");

  QString python_home;
  std::vector<std::string> python_path;
  bool using_bundled = false;

#ifdef SHAPEWORKS_BUNDLED_PYTHON
  {
    std::string bundled_home = find_bundled_python_home();
    if (!bundled_home.empty()) {
      SW_LOG("Using bundled Python from: " + bundled_home);
      python_path = compute_bundled_python_path(bundled_home);
      using_bundled = true;
      // Prevent user site-packages from interfering
      qputenv("PYTHONNOUSERSITE", "1");

      // In the build tree we use conda's libpython, so PYTHONHOME must
      // point to conda's prefix; in the installed app it points to the
      // bundled tree (where libpython and stdlib are self-consistent).
#ifdef _WIN32
      std::string site_sw = bundled_home + "/Lib/site-packages/shapeworks/__init__.py";
#else
      std::string site_sw = bundled_home + "/lib/python3.12/site-packages/shapeworks/__init__.py";
#endif
      if (!QFile::exists(QString::fromStdString(site_sw))) {
        python_home = QString::fromStdString(std::string(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX));
        SW_LOG("Build tree detected, PYTHONHOME set to conda: " + python_home.toStdString());
      } else {
        python_home = QString::fromStdString(bundled_home);
      }
    } else {
      SW_ERROR("Bundled Python not found. The installation may be corrupted.");
      initialized_success_ = false;
      return false;
    }
  }
#else
  {
    QString home = qgetenv("HOME");
#ifdef _WIN32
    home = qgetenv("USERPROFILE");
#endif

    // read pythonhome generated by:
    // python -c "import sys; print(sys.prefix)" > $HOME/.shapeworks/python_home_<version>.txt
    QString python_home_file = home + "/.shapeworks/python_home_" + PythonWorker::python_api_version + ".txt";
    std::cerr << "Reading python home from " << python_home_file.toStdString() << "\n";

    QFile qfile(python_home_file);
    if (qfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      python_home = QString(qfile.readAll()).toUtf8().trimmed();
    } else {
      SW_ERROR("Unable to initialize Python.  Could not find python_home file. Please run " + script);
      initialized_success_ = false;
      return false;
    }

    // read list generated by something like this:
    // python -c "import sys; print('\n'.join(sys.path))" > $HOME/.shapeworks/python_path_<version>.txt
    std::fstream file;

    std::string python_path_file =
        home.toStdString() + "/.shapeworks/python_path_" + PythonWorker::python_api_version + ".txt";
    file.open(python_path_file, std::ios::in);
    std::cerr << "Reading python path from " << python_path_file << "\n";
    if (file.is_open()) {
      std::string line;
      while (getline(file, line)) {
        std::cerr << "Adding to python path: " << line << "\n";
        python_path.push_back(line);
      }
      file.close();
    } else {
      SW_ERROR("Unable to initialize Python.  Could not find python_path file.  Please run " + script);

      initialized_success_ = false;
      return false;
    }
  }
#endif

  std::cerr << "PYTHONHOME = " << python_home.toStdString() << "\n";
  qputenv("PYTHONHOME", python_home.toUtf8());

#ifdef _WIN32
  if (using_bundled) {
    // Prepend Python DLL directories to PATH. Conda's .pyd extensions need
    // DLLs from Library/bin/ (e.g. libexpat.dll) in the build tree, and the
    // installed app has critical DLLs in bin/ from InstallBundledPython.
    QString dlls_dir = python_home + "/DLLs";
    QString app_dir = QCoreApplication::applicationDirPath();
    QString current_path = qgetenv("PATH");
    qputenv("PATH", (dlls_dir + ";" + python_home + ";" + app_dir + ";" + current_path).toUtf8());

    SW_LOG("Registered bundled Python DLL directories");
  } else {
    // Legacy conda-based Python path
    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);

    QString home = qgetenv("USERPROFILE");
    if (python_home.isEmpty()) {
      SW_ERROR("Unable to initialize Python\nPlease run install_shapeworks.bat");
      return false;
    } else {
      std::fstream file;
      QString path;
      file.open(home.toStdString() + "/.shapeworks/path_" + PythonWorker::python_api_version + ".txt", std::ios::in);
      if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
          path = QString::fromStdString(line);
        }
        file.close();
        qputenv("PATH", path.toUtf8());
        SW_LOG("Setting PATH for Python to: " + path.toStdString());
      }

      // Add the Library\bin directory where conda keeps DLLs like libexpat.dll
      QString library_bin = python_home + "/Library/bin";
      AddDllDirectory(library_bin.toStdWString().c_str());

      // Also add the DLLs directory
      QString dlls_dir = python_home + "/DLLs";
      AddDllDirectory(dlls_dir.toStdWString().c_str());

      // And the base python home
      AddDllDirectory(python_home.toStdWString().c_str());
    }
  }
#endif  // ifdef _WIN32

  try {
    py::initialize_interpreter();
    interpreter_started_ = true;

#ifdef _WIN32
    // Register DLL directories AFTER Python init. Python 3.8+ calls
    // SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) during
    // Py_Initialize(), which enables LOAD_LIBRARY_SEARCH_USER_DIRS.
    // AddDllDirectory calls made AFTER this point are effective for all
    // subsequent DLL loads (e.g. _ctypes.pyd needing ffi-8.dll,
    // vtkmodules .pyd files needing vtk*.dll).
    if (using_bundled) {
      QString dlls_dir = python_home + "/DLLs";
      AddDllDirectory(dlls_dir.toStdWString().c_str());
      AddDllDirectory(python_home.toStdWString().c_str());
      QString library_bin = python_home + "/Library/bin";
      if (QDir(library_bin).exists()) {
        AddDllDirectory(library_bin.toStdWString().c_str());
      }
      QString app_dir = QCoreApplication::applicationDirPath();
      AddDllDirectory(app_dir.toStdWString().c_str());
    }
#endif

    py::module sys = py::module::import("sys");

#ifdef __APPLE__
    setenv("OMP_NUM_THREADS", "1", 1);
#endif

    sys.attr("path") = python_path;

    // this is necessary or the plots will crash the process
    py::module py_matplot_lib = py::module::import("matplotlib");
    py_matplot_lib.attr("use")("agg");

    // search directories in python_path vector for the python executable
    QString python_executable;

#ifdef _WIN32
    QStringList exe_candidates = {
        python_home + "/python.exe",  // bundled or conda root
    };
#else
    QStringList exe_candidates = {
        python_home + "/bin/python",
        python_home + "/bin/python3",
    };
#endif
    for (const auto& path_executable : exe_candidates) {
      SW_LOG("checking {}", path_executable.toStdString());
      if (QFile::exists(path_executable) && QFileInfo(path_executable).isFile() &&
          QFile::permissions(path_executable) & QFile::ExeUser) {
        python_executable = path_executable;
        break;
      }
    }
    if (python_executable.isEmpty()) {
      SW_LOG("Unable to locate python executable");
    }

    // set up for multprocessing
    py::module multiprocessing = py::module::import("multiprocessing");
    // set the executable
    multiprocessing.attr("set_executable")(python_executable.toStdString());
    SW_DEBUG("Python executable: {}", python_executable.toStdString());

    python_logger_->set_callback(std::bind(&PythonWorker::incoming_python_message, this, std::placeholders::_1));
    python_logger_->set_progress_callback(
        std::bind(&PythonWorker::incoming_python_progress, this, std::placeholders::_1, std::placeholders::_2));
    py::module logger = py::module::import("logger");

    SW_LOG("Initializing ShapeWorks Python Module");

    py::module sw_utils = py::module::import("shapeworks.utils");

    py::object get_version = sw_utils.attr("get_api_version");
    std::string version = get_version().cast<std::string>();
    if (version != PythonWorker::python_api_version) {
      SW_ERROR("Unable to initialize Python. Expected API version " + std::string(PythonWorker::python_api_version) +
               " but found API version " + version + ". Please run " + script);
      initialized_success_ = false;
      return false;
    }

    py::object set_sw_logger = sw_utils.attr("set_sw_logger");
    set_sw_logger(python_logger_.data());

    py::module sw = py::module::import("shapeworks");
    py::object set_progress_callback = sw.attr("set_progress_callback");
    std::function<void(double, std::string)> progress_callback = [](double progress, std::string message) {
      SW_PROGRESS(progress, message);
      // std::cerr << "callback: " << progress << " " << message << "\n";
    };

    void* ptr = &progress_callback;
    set_progress_callback(ptr);

    // must reset the output window so that vtkPython's from conda's python doesn't take over
    vtkOutputWindow::SetInstance(studio_vtk_output_window_);

    SW_LOG("Embedded Python Interpreter Initialized");
  } catch (py::error_already_set& e) {
    SW_ERROR("Unable to initialize Python:\n" + std::string(e.what()));
    SW_ERROR("Unable to initialize Python.  Please run " + script);
    initialized_success_ = false;
    return false;
  } catch (const std::exception& e) {
    SW_ERROR("Unable to initialize Python:\n" + std::string(e.what()));
    SW_ERROR("Unable to initialize Python.  Please run " + script);
    initialized_success_ = false;
    return false;
  }

  initialized_success_ = true;
  return true;
}

//---------------------------------------------------------------------------
void PythonWorker::incoming_python_message(std::string message_string) { SW_LOG(message_string); }

//---------------------------------------------------------------------------
void PythonWorker::end_python() {
  // send to python thread
  QMetaObject::invokeMethod(this, "finalize_python");
}

//---------------------------------------------------------------------------
void PythonWorker::finalize_python() {
  if (interpreter_started_) {
    py::finalize_interpreter();
    interpreter_started_ = false;
  }
  thread_->exit();
}

//---------------------------------------------------------------------------
void PythonWorker::incoming_python_progress(double value, std::string message) {
  Q_EMIT current_job_->progress(value, QString::fromStdString(message));
}

//---------------------------------------------------------------------------
void PythonWorker::abort_job() { python_logger_->set_abort(); }

//---------------------------------------------------------------------------
std::string PythonWorker::get_user_site_packages() {
#ifdef __APPLE__
  QString base = QDir::homePath() + "/Library/Application Support/ShapeWorks";
#elif defined(_WIN32)
  QString base = qgetenv("LOCALAPPDATA") + "/ShapeWorks";
#else
  QString base = QDir::homePath() + "/.local/share/ShapeWorks";
#endif
  QString path = base + "/" + PythonWorker::python_api_version + "/site-packages";
  return path.toStdString();
}

//---------------------------------------------------------------------------
bool PythonWorker::is_torch_available() {
#ifdef SHAPEWORKS_BUNDLED_PYTHON
  std::string python_home = find_bundled_python_home();
  if (python_home.empty()) {
    return true;  // not bundled, assume conda has torch
  }

#ifdef _WIN32
  QString python_exe = QString::fromStdString(python_home) + "/python.exe";
#else
  QString python_exe = QString::fromStdString(python_home) + "/bin/python3";
#endif
  if (!QFile::exists(python_exe)) {
    return true;  // can't check, assume available
  }

  QProcess proc;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  std::string user_sp = get_user_site_packages();
  QStringList extra_paths = {QString::fromStdString(user_sp)};
#if defined(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX)
  // Only add conda site-packages in the build tree (where shapeworks isn't in bundled site-packages)
#ifdef _WIN32
  std::string site_sw = python_home + "/Lib/site-packages/shapeworks/__init__.py";
#else
  std::string site_sw = python_home + "/lib/python3.12/site-packages/shapeworks/__init__.py";
#endif
  if (!QFile::exists(QString::fromStdString(site_sw))) {
#ifdef _WIN32
    extra_paths.append(QString(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX) + "/Lib/site-packages");
#else
    extra_paths.append(QString(SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX) + "/lib/python3.12/site-packages");
#endif
  }
#endif
#ifdef _WIN32
  env.insert("PYTHONPATH", extra_paths.join(";"));
#else
  env.insert("PYTHONPATH", extra_paths.join(":"));
#endif
  proc.setProcessEnvironment(env);
  proc.start(python_exe, {"-c", "import torch"});
  proc.waitForFinished(10000);
  return proc.exitCode() == 0;
#else
  return true;  // conda workflow — assume torch is available
#endif
}

//---------------------------------------------------------------------------
bool PythonWorker::install_torch(std::function<void(std::string)> output_callback) {
#ifdef SHAPEWORKS_BUNDLED_PYTHON
  std::string python_home = find_bundled_python_home();
  if (python_home.empty()) {
    return false;
  }

#ifdef _WIN32
  QString python_exe = QString::fromStdString(python_home) + "/python.exe";
#else
  QString python_exe = QString::fromStdString(python_home) + "/bin/python3";
#endif
  if (!QFile::exists(python_exe)) {
    if (output_callback) output_callback("Error: bundled Python executable not found");
    return false;
  }

  // Install into versioned user site-packages so the .app doesn't need to be writable
  std::string user_sp = get_user_site_packages();
  QDir().mkpath(QString::fromStdString(user_sp));

  if (output_callback) {
    output_callback("Installing PyTorch to " + user_sp);
    output_callback("This may take several minutes...");
  }

  // Use light-the-torch to install the correct PyTorch variant
  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);

  // Connect output to callback
  if (output_callback) {
    QObject::connect(&proc, &QProcess::readyReadStandardOutput, [&]() {
      QString output = proc.readAllStandardOutput();
      for (const auto& line : output.split('\n', Qt::SkipEmptyParts)) {
        output_callback(line.toStdString());
      }
    });
  }

  proc.start(python_exe, {"-m", "light_the_torch", "install", "--target", QString::fromStdString(user_sp), "torch"});

  // Wait up to 10 minutes for install to complete
  if (!proc.waitForFinished(600000)) {
    if (output_callback) output_callback("Error: PyTorch installation timed out");
    return false;
  }

  if (proc.exitCode() != 0) {
    if (output_callback) {
      output_callback("Error: PyTorch installation failed (exit code " + std::to_string(proc.exitCode()) + ")");
      QString stderr_output = proc.readAllStandardError();
      if (!stderr_output.isEmpty()) {
        output_callback(stderr_output.toStdString());
      }
    }
    return false;
  }

  if (output_callback) output_callback("PyTorch installed successfully.");
  return true;
#else
  if (output_callback) output_callback("Not using bundled Python — install torch via conda/pip manually.");
  return false;
#endif
}

//---------------------------------------------------------------------------
std::string PythonWorker::find_bundled_python_home() {
  // Application directory varies by platform and context:
  // - macOS .app: .../Contents/MacOS
  // - Linux/Windows installed: .../bin
  // - Windows build tree: .../bin/Release
  // - Build tree (macOS .app): .../bin/ShapeWorksStudio.app/Contents/MacOS
  // - Build tree (CLI/Linux): .../bin
  QString app_dir = QCoreApplication::applicationDirPath();
  QDir dir(app_dir);

  // The stdlib marker file differs on Windows (Lib/os.py) vs Unix (lib/python3.12/os.py)
#ifdef _WIN32
  QString stdlib_marker = "/Lib/os.py";
#else
  QString stdlib_marker = "/lib/python3.12/os.py";
#endif

  // Candidates for installed bundles (in priority order)
  QStringList installed_candidates = {
#ifdef __APPLE__
      // macOS .app bundle: ../Resources/Python relative to Contents/MacOS
      dir.absoluteFilePath("../Resources/Python"),
#else
      // Linux/Windows installed: ../lib/python relative to bin/
      dir.absoluteFilePath("../lib/python"),
#endif
  };

  for (const auto& c : installed_candidates) {
    QDir candidate_dir(c);
    QString canonical = candidate_dir.canonicalPath();
    if (!canonical.isEmpty() && QFile::exists(canonical + stdlib_marker)) {
      return canonical.toStdString();
    }
  }

  // Try build-tree layouts.
  // Studio .app: executable is in bin/ShapeWorksStudio.app/Contents/MacOS/
  // CLI/Linux:   executable is in bin/
  // Windows:     executable is in bin/Release/
  // Bundled Python is in _bundled_python/python/ (sibling to bin/)
  QStringList build_tree_candidates = {
      dir.absoluteFilePath("../../../../_bundled_python/python"),  // macOS .app bundle
      dir.absoluteFilePath("../../_bundled_python/python"),        // Windows bin/Release/
      dir.absoluteFilePath("../_bundled_python/python"),           // CLI executable or Linux
  };

  for (const auto& c : build_tree_candidates) {
    QDir candidate_dir(c);
    QString canonical = candidate_dir.canonicalPath();
    if (!canonical.isEmpty() && QFile::exists(canonical + stdlib_marker)) {
      return canonical.toStdString();
    }
  }

  return {};
}

//---------------------------------------------------------------------------
std::vector<std::string> PythonWorker::compute_bundled_python_path(const std::string& python_home) {
  std::vector<std::string> paths;

#ifdef _WIN32
  // Windows layout: Lib/ (stdlib), DLLs/ (C extensions), Lib/site-packages/
  paths.push_back(python_home + "/Lib");
  paths.push_back(python_home + "/DLLs");
  paths.push_back(python_home + "/Lib/site-packages");
#else
  paths.push_back(python_home + "/lib/python3.12");
  paths.push_back(python_home + "/lib/python3.12/lib-dynload");
  paths.push_back(python_home + "/lib/python3.12/site-packages");
#endif

  // Add versioned user site-packages for on-demand installs (e.g. PyTorch)
  std::string user_sp = get_user_site_packages();
  if (!user_sp.empty()) {
    paths.push_back(user_sp);
  }

#if defined(SHAPEWORKS_BUNDLED_PYTHON_BUILD_BIN) && defined(SHAPEWORKS_BUNDLED_PYTHON_SOURCE_DIR)
  // In the build tree, shapeworks Python packages live in the source tree,
  // shapeworks_py.so/.pyd lives in the build bin/ directory, and third-party
  // packages (VTK, ITK, etc.) are in conda's site-packages.
  // Check if we're in the build tree by looking for shapeworks in site-packages.
#ifdef _WIN32
  std::string site_sw = python_home + "/Lib/site-packages/shapeworks/__init__.py";
#else
  std::string site_sw = python_home + "/lib/python3.12/site-packages/shapeworks/__init__.py";
#endif
  if (!QFile::exists(QString::fromStdString(site_sw))) {
    // Build tree: the embedded interpreter uses conda's libpython, so we
    // need conda's stdlib (platform.py, etc.) to match. Replace the bundled
    // stdlib paths with conda's, keeping bundled site-packages for pip packages.
    std::string conda_prefix = SHAPEWORKS_BUNDLED_PYTHON_CONDA_PREFIX;
    std::string source_dir = SHAPEWORKS_BUNDLED_PYTHON_SOURCE_DIR;
    std::string build_bin = SHAPEWORKS_BUNDLED_PYTHON_BUILD_BIN;

    // Rebuild paths: conda stdlib, then build/source dirs (so the dev's
    // shapeworks package wins over any stale conda-installed one), then
    // conda site-packages for third-party packages (VTK, ITK, etc.).
    paths.clear();
#ifdef _WIN32
    paths.push_back(conda_prefix + "/Lib");
    paths.push_back(conda_prefix + "/DLLs");
#else
    paths.push_back(conda_prefix + "/lib/python3.12");
    paths.push_back(conda_prefix + "/lib/python3.12/lib-dynload");
#endif
    paths.push_back(build_bin);

    QDir python_dir(QString::fromStdString(source_dir));
    for (const auto& entry : python_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      paths.push_back(source_dir + "/" + entry.toStdString());
    }

    // Conda site-packages first (has torch, numpy, etc. that match conda's libpython),
    // then bundled site-packages as fallback for packages only in the bundled tree.
#ifdef _WIN32
    paths.push_back(conda_prefix + "/Lib/site-packages");
    paths.push_back(python_home + "/Lib/site-packages");
#else
    paths.push_back(conda_prefix + "/lib/python3.12/site-packages");
    paths.push_back(python_home + "/lib/python3.12/site-packages");
#endif
  }
#endif

  return paths;
}

}  // namespace shapeworks
