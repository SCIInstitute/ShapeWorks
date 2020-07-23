#include <cstdlib>

#include "Testing.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
  // set PATH to shapeworks executable called by shell scripts
#ifdef _WIN32
  auto path(std::string(BUILD_DIR) + "\\bin\\Release" + ";" + std::getenv("PATH"));
  _putenv_s("PATH", path.c_str());
#else
  auto path(std::string(BUILD_DIR) + "/bin" + ":" + std::getenv("PATH")); // fixme: could be /bin/Debug or /bin/Release for systems such as Xcode
  setenv("PATH", path.c_str(), true);
#endif

  // set location of shapeworks DATA used by shell scripts
  std::string data(TEST_DATA_DIR);
  data += "/shapeworks";
#ifdef _WIN32
  _putenv_s("DATA", data.c_str());
#else
  setenv("DATA", data.c_str(), true);
#endif

  // change to the shapeworksTest directory
  auto shapeworksTestsDir(std::string(TEST_DATA_DIR) + "/../shapeworksTests");
  chdir(shapeworksTestsDir.c_str());
}

