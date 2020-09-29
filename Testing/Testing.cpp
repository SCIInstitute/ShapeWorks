#include <cstdlib>

#include "Testing.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
  // set PATH to shapeworks executable called by shell scripts
#ifdef _WIN32
  auto path(std::string(BUILD_DIR) + "\\bin\\Release" + ";" + std::string(DEPS_DIR) + "\\bin" + ";" + std::getenv("PATH"));
  std::cout << "path: " << path << std::endl;
  _putenv_s("PATH", path.c_str());
  auto pythonpath(std::string(DEPS_DIR) + "\\bin\\lib\\site-packages" + ";" + std::getenv("PYTHONPATH"));
  std::cout << "pythonpath: " << pythonpath << std::endl;
  _putenv_s("PYTHONPATH", pythonpath.c_str());
#elif __linux__
  auto path(std::string(BUILD_DIR) + "/bin" + ":" + std::getenv("PATH")); // fixme: could be /bin/Debug or /bin/Release for systems such as Xcode
  std::cout << "path: " << path << std::endl;
  setenv("PATH", path.c_str(), true);
  auto pythonpath(std::string(DEPS_DIR) + "/lib/python3.7/site-packages" + ":" + std::getenv("PYTHONPATH"));
  std::cout << "pythonpath: " << pythonpath << std::endl;
  setenv("PYTHONPATH", pythonpath.c_str(), true);
  auto ld_library_path(std::string(DEPS_DIR) + "/lib" + ":" + std::string("QT_LIBDIR") + ":" + std::getenv("LD_LIBRARY_PATH"));
  std::cout << "ld_library_path: " << ld_library_path << std::endl;
  setenv("LD_LIBRARY_PATH", ld_library_path.c_str(), true);
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
  std::cout << "changing directory to " << shapeworksTestsDir << std::endl;
  chdir(shapeworksTestsDir.c_str());

  std::cout << "test environment setup complete!" << std::endl;
}

