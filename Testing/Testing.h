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

 private:
  bool should_keep_dir();

  boost::filesystem::path temp_base_;
};
}  // namespace shapeworks
