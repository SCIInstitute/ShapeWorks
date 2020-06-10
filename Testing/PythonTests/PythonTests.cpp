#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <cstdio>

#include "TestConfiguration.h"

#ifdef _WIN32
   #include <io.h>
   #define access _access_s
#else
   #include <unistd.h>
#endif

//using namespace shapeworks;

//---------------------------------------------------------------------------
static bool file_exists(const std::string &filename)
{
  return access(filename.c_str(), 0) == 0;
}

//---------------------------------------------------------------------------
TEST(PythonTests, tiny_test) {

  // locations
  std::string python_examples_location = std::string(TEST_DATA_DIR) + std::string(
    "/../../Examples/Python");
  std::string bin_dir = std::string(BUILD_DIR) + "/bin";
#ifdef _WIN32
  bin_dir = bin_dir + "/Release";
  std::replace( bin_dir.begin(), bin_dir.end(), '/', '\\'); 
  std::string path = getenv("PATH");
  path = path + ";" + std::string(INSTALL_DIR) + "\\bin";
  std::cerr << "Setting PATH to " << path << "\n";
  _putenv_s("PATH", path.c_str());
#endif

  std::cerr << "Python examples Location: " << python_examples_location << "\n";
  std::cerr << "Binary Directory: " << bin_dir << "\n";

  // check that one of the resulting files exists
  std::string check_file = python_examples_location +
                           "/TestEllipsoids/PointFiles/32/seg.ellipsoid_00.isores.center.pad.com.aligned.cropped.tpSmoothDT_local.particles";

  // change to the python examples directory
  chdir(python_examples_location.c_str());

  // delete the file to make sure it's remade
  std::remove(check_file.c_str());

  // run python
  std::string command = "python RunUseCase.py --use_case ellipsoid --tiny_test --shapeworks_path " + bin_dir;
  std::cerr << "Running command: " << command << "\n";
  system(command.c_str());

  ASSERT_TRUE(file_exists(check_file));
}
