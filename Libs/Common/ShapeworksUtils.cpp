#include "ShapeworksUtils.h"

#include <Logging.h>
#include <sys/stat.h>
#include <tbb/global_control.h>
#include <tbb/info.h>

namespace shapeworks {

// Windows doesn't have S_ISDIR and S_ISREG macros
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

unsigned ShapeWorksUtils::rng_seed_ = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 ShapeWorksUtils::mt_;

//-----------------------------------------------------------------------------
/// looks at the pathname to see if it's a file or a directory or neither
static bool stat_path(const std::string& pathname, bool isdir = false) {
  struct stat info;
  if (stat(pathname.c_str(), &info) != 0) {
    return false;
  } else {
    return isdir ? S_ISDIR(info.st_mode) : S_ISREG(info.st_mode);
  }
}

//-----------------------------------------------------------------------------
void ShapeWorksUtils::set_rng_seed(const unsigned seed) {
  rng_seed_ = seed;
  mt_.seed(rng_seed_);
}

//-----------------------------------------------------------------------------
bool ShapeWorksUtils::is_directory(const std::string& pathname) { return stat_path(pathname, true); }

//-----------------------------------------------------------------------------
bool ShapeWorksUtils::file_exists(const std::string& filename) { return stat_path(filename, false); }

//-----------------------------------------------------------------------------
void ShapeWorksUtils::setup_console_logging(bool show_progress, bool xml_status) {
  if (show_progress) {
    auto progress_callback = [](double progress, std::string message) {
      // show status message and percentage complete
      std::cout << fmt::format("{} ({:.1f}%)        \r", message, progress);
      std::cout.flush();
    };
    Logging::Instance().set_progress_callback(progress_callback);
  }

  if (xml_status) {
    auto progress_callback = [](double progress, std::string message) {
      // print status message and percentage complete
      std::cout << fmt::format("<xml><status>{}</status><progress>{:.1f}</progress></xml>\n", message, progress);
      std::cout.flush();
    };
    Logging::Instance().set_progress_callback(progress_callback);

    auto error_callback = [](std::string message) {
      std::cout << fmt::format("<xml><error>{}</error></xml>\n", message);
      std::cout.flush();
    };
    Logging::Instance().set_error_callback(error_callback);
  }
}

//-----------------------------------------------------------------------------
void ShapeWorksUtils::set_progress_callback(void* ptr) {
  std::function<void(double, std::string)>& callback =
      *reinterpret_cast<std::function<void(double, std::string)>*>(ptr);
  Logging::Instance().set_progress_callback(callback);
}

//-----------------------------------------------------------------------------
void ShapeWorksUtils::setup_threads() {
  // control number of threads
  int num_threads = tbb::info::default_concurrency();
  const char* num_threads_env = getenv("TBB_NUM_THREADS");
  if (num_threads_env) {
    num_threads = std::max(1, atoi(num_threads_env));
  }
  SW_DEBUG("TBB using {} threads", num_threads);
  Eigen::setNbThreads(num_threads);
  tbb::global_control c(tbb::global_control::max_allowed_parallelism, num_threads);
}

//-----------------------------------------------------------------------------
Matrix33 ShapeWorksUtils::convert_matrix(const vtkSmartPointer<vtkMatrix4x4>& mat) {
  Matrix33 m;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      m[i][j] = mat->GetElement(i, j);
    }
  }

  return m;
}

//-----------------------------------------------------------------------------
Vector3 ShapeWorksUtils::get_offset(const vtkSmartPointer<vtkMatrix4x4>& mat) {
  return makeVector({mat->GetElement(0, 3), mat->GetElement(1, 3), mat->GetElement(2, 3)});
}

//-----------------------------------------------------------------------------
double ShapeWorksUtils::elapsed(ShapeWorksUtils::time_point start, ShapeWorksUtils::time_point end,
                                bool print_elapsed) {
  // Calculating total time taken by the program.
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  time_taken *= 1e-9;

  if (print_elapsed) cout << "Elapsed: " << std::fixed << time_taken << std::setprecision(9) << " sec" << endl;

  return time_taken;
}

}  // namespace shapeworks
