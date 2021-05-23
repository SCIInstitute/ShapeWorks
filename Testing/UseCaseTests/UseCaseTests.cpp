#include "Testing.h"

#ifdef _WIN32
  #include <io.h>
  #define access _access_s
#else

  #include <unistd.h>
#endif

// locations
std::string python_examples_location = std::string(TEST_DATA_DIR) + std::string(
  "/../../Examples/Python");

//---------------------------------------------------------------------------
static bool file_exists(const std::string& filename)
{
  std::cerr << "Checking for " << filename << "\n";
  return access(filename.c_str(), 0) == 0;
}

// only need to run pythonEnvSetup once or it continuously appends to paths
// FIXME: This is a problem if you only need to execute a single test (same problem in ShapeworksTests)
TEST(UseCaseTests, setup)
{
  pythonEnvSetup();
  ASSERT_TRUE(true);
}

TEST(UseCaseTests, ellipsoid)
{
  // check that one of the resulting files exists
  std::string check_file = python_examples_location +
                           "/Output/ellipsoid/shape_models/32/ellipsoid_00.isores.center.com.aligned.cropped.tpSmoothDT_local.particles";

  // change to the python examples directory
  chdir(python_examples_location.c_str());

  // delete the file to make sure it's remade
  std::remove(check_file.c_str());

  // run python
  std::string command = "python RunUseCase.py --use_case ellipsoid --tiny_test";
  std::cerr << "Running command: " << command << "\n";
  system(command.c_str());

  ASSERT_TRUE(file_exists(check_file));
}

TEST(UseCaseTests, ellipsoid_mesh)
{
  // check that one of the resulting files exists
  std::string check_file = python_examples_location +
                           "/Output/ellipsoid_mesh/shape_models/32/ellipsoid_00_local.particles";

  // change to the python examples directory
  chdir(python_examples_location.c_str());

  // delete the file to make sure it's remade
  std::remove(check_file.c_str());

  // run python
  std::string command = "python RunUseCase.py --use_case ellipsoid_mesh --tiny_test";
  std::cerr << "Running command: " << command << "\n";
  system(command.c_str());

  ASSERT_TRUE(file_exists(check_file));
}
