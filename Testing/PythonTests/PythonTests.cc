#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <cstdio>

#include "TestConfiguration.h"

#ifdef _WIN32
   #include <io.h>
   #define access    _access_s
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
  std::cerr << "Location: " << python_examples_location << "\n";

  // check that one of the resulting files exists
  std::string check_file = python_examples_location +
                           "/TestEllipsoids/PointFiles/32/seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_local.particles";

  // change to the python examples directory
  chdir(python_examples_location.c_str());

  // delete the file to make sure it's remade
  std::remove(check_file.c_str());

  // run python
  std::string command = "python ellipsoidMain.py --tiny_test " + bin_dir;
  system(command.c_str());

  ASSERT_TRUE(file_exists(check_file));
}
