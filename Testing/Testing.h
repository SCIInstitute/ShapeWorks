#include <gtest/gtest.h>

#include "TestConfiguration.h"
#include "boost/filesystem/path.hpp"

void setupenv(const std::string& testDirectory);

namespace shapeworks {
class TestUtils {
 public:
  //! Return the singleton instance
  static TestUtils& Instance();

  //! Constructor
  TestUtils();

  //! Destructor, cleans up test directory if desired
  ~TestUtils();

  //! Return output directory
  std::string get_output_dir(std::string test_name);

  //! Create a copy of a test data folder in a temp folder and chdir there
  void prep_temp(std::string test_folder, std::string test_name);

 private:
  bool should_keep_dir();

  void recursive_copy(const boost::filesystem::path &src, const boost::filesystem::path &dst);

  boost::filesystem::path temp_base_;
};
}  // namespace shapeworks
