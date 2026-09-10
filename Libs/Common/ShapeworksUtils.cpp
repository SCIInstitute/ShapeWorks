#include "ShapeworksUtils.h"

#include <Logging.h>
#include <sys/stat.h>
#include <tbb/global_control.h>
#include <tbb/info.h>
#include <vtksys/SystemTools.hxx>

#include <boost/filesystem.hpp>
#include <boost/nowide/utf8_codecvt.hpp>
#include <locale>

namespace shapeworks {

unsigned ShapeWorksUtils::rng_seed_ = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 ShapeWorksUtils::mt_;
std::unique_ptr<tbb::global_control> ShapeWorksUtils::tbb_global_control_;

//-----------------------------------------------------------------------------
void ShapeWorksUtils::set_rng_seed(const unsigned seed) {
  rng_seed_ = seed;
  mt_.seed(rng_seed_);
}

//-----------------------------------------------------------------------------
void ShapeWorksUtils::initialize_path_handling() {
  // boost::filesystem stores a path in the character type the OS API uses: char on POSIX, but
  // wchar_t on Windows.  There every narrow string is converted using the codecvt facet imbued
  // into boost::filesystem::path, which by default comes from the global locale rather than being
  // UTF-8.  The rest of ShapeWorks (Qt, VTK, ITK) speaks UTF-8, so without this a path holding
  // non-ASCII characters is mangled on the way in and no longer resolves.  On POSIX no conversion
  // takes place and this only affects the wide-string accessors.
  boost::filesystem::path::imbue(std::locale(std::locale(), new boost::nowide::utf8_codecvt<wchar_t>));
}

//-----------------------------------------------------------------------------
bool ShapeWorksUtils::is_directory(const std::string& pathname) {
  return vtksys::SystemTools::FileIsDirectory(pathname);
}

//-----------------------------------------------------------------------------
bool ShapeWorksUtils::file_exists(const std::string& filename) {
  // vtksys rather than boost::filesystem: on Windows it decodes UTF-8 and applies the \\?\ prefix,
  // so it agrees with the VTK and ITK readers about which paths exist.  boost::filesystem does
  // neither, and rejects long or non-ASCII paths that those readers open without complaint.
  return vtksys::SystemTools::FileExists(filename, true);
}

//-----------------------------------------------------------------------------
bool ShapeWorksUtils::path_exists(const std::string& pathname) {
  return vtksys::SystemTools::PathExists(pathname);
}

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

  tbb_global_control_ =
      std::make_unique<tbb::global_control>(tbb::global_control::max_allowed_parallelism, num_threads);
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
