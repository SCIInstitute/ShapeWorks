#include <iostream>
#include <fstream>
#include <cstdio>

#include "Testing.h"

//---------------------------------------------------------------------------
static bool file_exists(const std::string &filename)
{
  return access(filename.c_str(), 0) == 0;
}

//---------------------------------------------------------------------------
TEST(PythonTests, ellipsoidTinyTest) {
  shapeworksEnvSetup();

  std::string test_location(std::string(TEST_DATA_DIR) + "/../../Examples/Python");

  // decide success by checking that one of the files created by the use case exists (todo: better measure of success)
  std::string check_file = test_location +
    "/TestEllipsoids/shape_models/32/seg.ellipsoid_00.isores.center.pad.com.aligned.cropped.tpSmoothDT_local.particles";

  // delete the file to make sure it's remade
  chdir(test_location.c_str());
  std::remove(check_file.c_str());

  // run the use case
  std::string command = "python RunUseCase.py --use_case ellipsoid --tiny_test";
  std::cerr << "Running command: " << command << "\n";
  system(command.c_str());

  ASSERT_TRUE(file_exists(check_file));
}
