#include <cstdlib>

#include "Testing.h"

void setupenv(const std::string& testDirectory)
{
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
