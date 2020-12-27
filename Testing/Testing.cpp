#include <cstdlib>

#include "Testing.h"

#ifdef _WIN32
static std::string find_in_path(std::string file) {
  std::stringstream path(getenv("PATH"));
  while (! path.eof()) {
    std::string test;
    struct stat info;
    getline(path, test, ';');
    std::string base = test;
    test.append("/");
    test.append(file);
    if (stat(test.c_str(), &info) == 0) {
      return base;
    }
  }
  return "";
}
#endif

/// set necessary env vars for running shapeworks executable and using python api
// TODO: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
void shapeworksEnvSetup()
{
  const char* curr_path(std::getenv("PATH"));
  const char* curr_pythonpath(std::getenv("PYTHONPATH"));
  const char* curr_ld_library_path(std::getenv("LD_LIBRARY_PATH"));

#ifdef _WIN32
  auto path(std::string(BUILD_DIR) + "\\bin\\Release" + ";"
            + std::string(BUILD_DIR) + "\\bin\\RelWithDebInfo" + ";"
            + std::string(BUILD_DIR) + "\\bin\\Debug" + ";"
            + std::string(INSTALL_DIR) + "\\bin\\Release" + ";"
            + std::string(INSTALL_DIR) + "\\bin\\RelWithDebInfo" + ";"
            + std::string(INSTALL_DIR) + "\\bin\\Debug" + ";");
  //path += std::string(DEPS_DIR) + "\\bin" + ";";
  path += (curr_path ? curr_path : ""); // fixme: could be /bin/Debug if we ever figure out how to build Windows Debug
  std::cout << "path: " << path << std::endl;
  _putenv_s("PATH", path.c_str());

  auto pythonpath(std::string(BUILD_DIR) + "\\bin\\Release" + ";"
                  + std::string(BUILD_DIR) + "\\bin\\RelWithDebInfo" + ";"
                  + std::string(BUILD_DIR) + "\\bin\\Debug" + ";"
                  + std::string(INSTALL_DIR) + "\\bin" + ";"
                  + std::string(INSTALL_DIR) + "\\lib" + ";");
  //pythonpath += std::string(DEPS_DIR) + "\\bin\\lib\\site-packages" + ";";
  pythonpath += (curr_pythonpath ? curr_pythonpath : "");
  std::cout << "pythonpath: " << pythonpath << std::endl;
  _putenv_s("PYTHONPATH", pythonpath.c_str());

  std::string found_path = find_in_path("python.exe");
  if (found_path != "") {
     std::cerr << "python.exe found in: " << found_path << "\n";
     _putenv_s("PYTHONHOME", found_path.c_str());
  }

#elif __linux__
  auto path(std::string(BUILD_DIR) + "/bin" + ":"
            + std::string(BUILD_DIR) + "/lib" + ":"
            + std::string(INSTALL_DIR) + "/bin" + ":"
            + std::string(INSTALL_DIR) + "/lib" + ":");
  path += (curr_path ? curr_path : "");
  std::cout << "path: " << path << std::endl;
  setenv("PATH", path.c_str(), true);

  auto pythonpath(std::string(BUILD_DIR) + "/bin" + ":"
                  + std::string(BUILD_DIR) + "/lib" + ":"
                  + std::string(INSTALL_DIR) + "/bin" + ":"
                  + std::string(INSTALL_DIR) + "/lib" + ":");
  //pythonpath += std::string(DEPS_DIR) + "/lib/python3.7/site-packages" + ":";
  pythonpath += (curr_pythonpath ? curr_pythonpath : "");
  std::cout << "pythonpath: " << pythonpath << std::endl;
  setenv("PYTHONPATH", pythonpath.c_str(), true);

  auto ld_library_path(std::string(BUILD_DIR) + "/lib" + ":"
                       + std::string(BUILD_DIR) + "/bin" + ":"
                       + std::string(INSTALL_DIR) + "/lib" + ":"
                       + std::string(INSTALL_DIR) + "/bin" + ":");
  //ld_library_path += std::string(DEPS_DIR) + "/lib" + ":";
  //ld_library_path += std::string(QT_LIBDIR) + ":";
  ld_library_path += (curr_ld_library_path ? curr_ld_library_path : "");
  std::cout << "ld_library_path: " << ld_library_path << std::endl;
  setenv("LD_LIBRARY_PATH", ld_library_path.c_str(), true);
#else
  auto path(std::string(BUILD_DIR) + "/bin" + ":"
            + std::string(BUILD_DIR) + "/lib" + ":"
            + std::string(BUILD_DIR) + "/bin/Debug" + ":"
            + std::string(BUILD_DIR) + "/lib/Debug" + ":"
            + std::string(BUILD_DIR) + "/bin/Release" + ":"
            + std::string(BUILD_DIR) + "/lib/Release" + ":"
            + std::string(INSTALL_DIR) + "/bin" + ":"
            + std::string(INSTALL_DIR) + "/lib" + ":"
            + std::string(INSTALL_DIR) + "/bin/Debug" + ":"
            + std::string(INSTALL_DIR) + "/lib/Debug" + ":"
            + std::string(INSTALL_DIR) + "/bin/Release" + ":"
            + std::string(INSTALL_DIR) + "/lib/Release" + ":");
  path += (curr_path ? curr_path : "");
  std::cout << "path: " << path << std::endl;
  setenv("PATH", path.c_str(), true);

  auto pythonpath(std::string(BUILD_DIR) + "/bin" + ":"
                  + std::string(BUILD_DIR) + "/lib" + ":"
                  + std::string(BUILD_DIR) + "/bin/Debug" + ":"
                  + std::string(BUILD_DIR) + "/lib/Debug" + ":"
                  + std::string(BUILD_DIR) + "/bin/Release" + ":"
                  + std::string(BUILD_DIR) + "/lib/Release" + ":"
                  + std::string(INSTALL_DIR) + "/bin" + ":"
                  + std::string(INSTALL_DIR) + "/lib" + ":"
                  + std::string(INSTALL_DIR) + "/bin/Debug" + ":"
                  + std::string(INSTALL_DIR) + "/lib/Debug" + ":"
                  + std::string(INSTALL_DIR) + "/bin/Release" + ":"
                  + std::string(INSTALL_DIR) + "/lib/Release" + ":");
  //pythonpath += std::string(DEPS_DIR) + "/lib/python3.7/site-packages" + ":";
  pythonpath += (curr_pythonpath ? curr_pythonpath : "");
  std::cout << "pythonpath: " << pythonpath << std::endl;
  setenv("PYTHONPATH", pythonpath.c_str(), true);
#endif

  // set location of shapeworks DATA used by shell scripts
  std::string data(TEST_DATA_DIR);
#ifdef _WIN32
  _putenv_s("DATA", data.c_str());
#else
  setenv("DATA", data.c_str(), true);
#endif

  // change to the shapeworksTests directory
  auto shapeworksTestsDir(std::string(TEST_DATA_DIR) + "/../shapeworksTests");
  chdir(shapeworksTestsDir.c_str());

  std::cout << "test environment setup complete!" << std::endl;
}

//---------------------------------------------------------------------------
void pythonEnvSetup()
{
  // set paths to find shapeworks python module
  shapeworksEnvSetup();

  // change to the PythonTests directory
  auto pythonTestsDir(std::string(TEST_DATA_DIR) + "/../PythonTests");
  chdir(pythonTestsDir.c_str());
}
