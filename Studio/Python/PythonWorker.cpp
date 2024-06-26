// pybind
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Logging.h>
#include <Python/PythonWorker.h>
#include <Shape.h>

#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <iostream>
#include <sstream>

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

 private:
  std::function<void(std::string)> callback_;
  std::function<void(double, std::string)> progress_callback_;

  std::atomic<bool> aborted_{false};
};

//---------------------------------------------------------------------------
PYBIND11_EMBEDDED_MODULE(logger, m) {
  py::class_<PythonLogger, std::shared_ptr<PythonLogger>>(m, "PythonLogger")
      .def(py::init<>())
      .def("log", &PythonLogger::cpp_log)
      .def("check_abort", &PythonLogger::check_abort)
      .def("progress", &PythonLogger::cpp_progress);
};

//---------------------------------------------------------------------------
PythonWorker::PythonWorker() {
  python_logger_ = QSharedPointer<PythonLogger>::create();

  // create singular Python thread and move this object to the new thread
  thread_ = new QThread(this);
  moveToThread(thread_);
  thread_->start();
}

//---------------------------------------------------------------------------
PythonWorker::~PythonWorker() {
  end_python();
  thread_->wait();
  delete thread_;
}

//---------------------------------------------------------------------------
void PythonWorker::set_vtk_output_window(vtkSmartPointer<StudioVtkOutputWindow> output_window) {
  studio_vtk_output_window_ = output_window;
}

//---------------------------------------------------------------------------
void PythonWorker::start_job(QSharedPointer<Job> job) {
  if (init()) {
    try {
      job->start_timer();
      if (!job->get_quiet_mode()) {
        SW_LOG("Running Task: " + job->name().toStdString());
      }
      Q_EMIT job->progress(0);
      current_job_ = job;
      current_job_->run();
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

  QString home = qgetenv("HOME");
#ifdef _WIN32
  home = qgetenv("USERPROFILE");
#endif

  // read pythonhome generated by:
  // python -c "import sys; print(sys.prefix)" > $HOME/.shapeworks/python_home_<version>.txt
  QString python_home_file = home + "/.shapeworks/python_home_" + PythonWorker::python_api_version + ".txt";
  std::cerr << "Reading python home from " << python_home_file.toStdString() << "\n";

  QFile qfile(python_home_file);
  QString python_home;
  if (qfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    python_home = QString(qfile.readAll()).toUtf8().trimmed();
  } else {
    SW_ERROR("Unable to initialize Python.  Could not find python_home file. Please run " + script);
    initialized_success_ = false;
    return false;
  }

  // read list generated by something like this:
  // python -c "import sys; print('\n'.join(sys.path))" > $HOME/.shapeworks/python_path_<version>.txt
  std::vector<std::string> python_path;
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

  std::cerr << "PYTHONHOME = " << python_home.toStdString() << "\n";
  qputenv("PYTHONHOME", python_home.toUtf8());

#ifdef _WIN32
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
    }

    qputenv("PATH", path.toUtf8());
    SW_LOG("Setting PATH for Python to: " + path.toStdString());
  }
#endif  // ifdef _WIN32

  try {
    py::initialize_interpreter();

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
    QString path_executable = python_home + "/python.exe";
#else
    QString path_executable = python_home + "/bin/python";
#endif
    SW_LOG("checking {}", path_executable);

    // check that it exists and is a file and is executable
    if (QFile::exists(path_executable) && QFileInfo(path_executable).isFile() &&
        QFile::permissions(path_executable) & QFile::ExeUser) {
      python_executable = path_executable;
    } else {
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
  if (initialized_success_) {
    py::finalize_interpreter();
  }
  thread_->exit();
}

//---------------------------------------------------------------------------
void PythonWorker::incoming_python_progress(double value, std::string message) {
  Q_EMIT current_job_->progress(value, QString::fromStdString(message));
}

//---------------------------------------------------------------------------
void PythonWorker::abort_job() { python_logger_->set_abort(); }
}  // namespace shapeworks
