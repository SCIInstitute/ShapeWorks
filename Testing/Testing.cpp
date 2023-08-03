#include "Testing.h"

#include <boost/filesystem.hpp>
#include <cstdlib>

void setupenv(const std::string& testDirectory) {
  // change to the shapeworksTests directory
  auto shapeworksTestsDir(testDirectory);
  chdir(shapeworksTestsDir.c_str());

  // set location of shapeworks DATA used by shell scripts
  std::string data(TEST_DATA_DIR);
#ifdef _WIN32
  _putenv_s("DATA", data.c_str());
  _putenv_s("VTK_SMP_BACKEND_IN_USE", "Sequential");
#else
  setenv("DATA", data.c_str(), true);
  setenv("VTK_SMP_BACKEND_IN_USE", "Sequential", true);
#endif
}

namespace shapeworks {

//-----------------------------------------------------------------------------
TestUtils& TestUtils::Instance() {
  static TestUtils instance;
  return instance;
}

//-----------------------------------------------------------------------------
TestUtils::TestUtils() {}

//-----------------------------------------------------------------------------
TestUtils::~TestUtils() {
  if (!temp_base_.empty() && !should_keep_dir()) {
    try {
      boost::filesystem::remove_all(temp_base_);
    } catch (std::exception& e) {
      std::cerr << "Error removing test output directory: " << temp_base_ << "\n";
      std::cerr << "Error: " << e.what();
    }
  }
  if (should_keep_dir()) {
    std::cerr << "Test output left in: " << temp_base_ << "\n";
  }
}

//-----------------------------------------------------------------------------
std::string TestUtils::get_output_dir(std::string test_name) {
  if (temp_base_.empty()) {
    temp_base_ = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(temp_base_);
    if (should_keep_dir()) {
      std::cerr << "Test output will be kept in: " << temp_base_ << "\n";
    }
  }

  boost::filesystem::path test_temp = temp_base_ / test_name;
  boost::filesystem::create_directories(test_temp);

  return test_temp.generic_string();
}

//-----------------------------------------------------------------------------
void TestUtils::prep_temp(std::string test_folder, std::string test_name) {
  auto temp_dir = get_output_dir(test_name);
  recursive_copy(test_folder, temp_dir);
  boost::filesystem::current_path(temp_dir);
}

//-----------------------------------------------------------------------------
bool TestUtils::should_keep_dir() {
  bool keep = false;
  const char* keep_env = getenv("SW_KEEP_TEST_OUTPUT");
  if (keep_env) {
    if (std::string(keep_env) == "1") {
      keep = true;
    }
  }

  return keep;
}

//-----------------------------------------------------------------------------
void TestUtils::recursive_copy(const boost::filesystem::path& src, const boost::filesystem::path& dst) {
  // adapted from https://gist.github.com/ssteffl/30055ac128bb92801568899ebad73365

  if (boost::filesystem::is_directory(src)) {
    boost::filesystem::create_directories(dst);
    for (boost::filesystem::directory_entry& item : boost::filesystem::directory_iterator(src)) {
      recursive_copy(item.path(), dst / item.path().filename());
    }
  } else if (boost::filesystem::is_regular_file(src)) {
    // if it already exists, remove it
    if (boost::filesystem::exists(dst)) {
      boost::filesystem::remove(dst);
    }
    boost::filesystem::copy(src, dst);
  } else {
    throw std::runtime_error(dst.generic_string() + " not dir or file");
  }
}

//-----------------------------------------------------------------------------

}  // namespace shapeworks
