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
#else
  setenv("DATA", data.c_str(), true);
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
    boost::filesystem::remove_all(temp_base_);
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

}  // namespace shapeworks
