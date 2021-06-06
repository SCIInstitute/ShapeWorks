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

//---------------------------------------------------------------------------
void run_use_case(const std::string& name, const std::string& check_file)
{
  // check that one of the resulting files exists
  std::string file = python_examples_location + "/" + check_file;

  // change to the python examples directory
  chdir(python_examples_location.c_str());

  // delete the file to make sure it's remade
  std::remove(file.c_str());

  // run python
  std::string command = "python RunUseCase.py --use_case " + name + " --tiny_test";
  std::cerr << "Running command: " << command << "\n";
  ASSERT_FALSE(system(command.c_str()));

  ASSERT_TRUE(file_exists(file));
}


//---------------------------------------------------------------------------
// only need to run pythonEnvSetup once or it continuously appends to paths
// FIXME: This is a problem if you only need to execute a single test (same problem in ShapeworksTests)
TEST(UseCaseTests, setup)
{
  pythonEnvSetup();
  ASSERT_TRUE(true);
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid)
{
  run_use_case("ellipsoid",
               "Output/ellipsoid/shape_models/32/ellipsoid_00.isores.center.com.aligned.cropped.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_mesh)
{
  run_use_case("ellipsoid_mesh",
               "Output/ellipsoid_mesh/shape_models/32/ellipsoid_00_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_fd)
{
  run_use_case("ellipsoid_fd",
               "Output/ellipsoid_fd/shape_models/128/ellipsoid_19.isores.center.com.aligned.cropped.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_cut)
{
  run_use_case("ellipsoid_cut",
               "Output/ellipsoid_cut/shape_models/16/ellipsoid_00.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_evaluate)
{
  run_use_case("ellipsoid_evaluate",
               "Output/ellipsoid/evaluation/compactness/scree.txt");
}


//---------------------------------------------------------------------------
TEST(UseCaseTests, lumps)
{
  run_use_case("lumps",
               "Output/lumps/shape_models/32/lump0_0_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, left_atrium)
{
  run_use_case("left_atrium",
               "Output/left_atrium/shape_models/32/CARMA0046.laendo_no_veins.isores.center.pad.com.aligned.cropped.tpSmoothDT_local.particles");
}


//---------------------------------------------------------------------------
TEST(UseCaseTests, femur)
{
  run_use_case("femur",
               "Output/femur/shape_models/32/m03_L_femur.isores.pad.com.center.aligned.clipped.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_mesh)
{
  run_use_case("femur_mesh",
               "Output/femur_mesh/shape_models/32/m03_L_femur_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_cut)
{
  run_use_case("femur_cut",
               "Output/femur_cut/shape_models/32/m03_L_femur.isores.pad.com.center.aligned.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, deep_ssm)
{
  /// TODO: Disabled, doesn't run on windows GH
  return;
  run_use_case("deep_ssm",
               "Output/deep_ssm/Augmentation/violin.png");
}
//---------------------------------------------------------------------------
TEST(UseCaseTests, supershapes_1mode_contour)
{
  run_use_case("supershapes_1mode_contour",
               "Output/supershapes_1mode_contour/shape_models/64/00_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, thin_cavity_bean)
{
  run_use_case("thin_cavity_bean",
               "Output/thin_cavity_bean/shape_models/32/thin_cavity_bean_00_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_pca)
{
  run_use_case("ellipsoid_pca",
               "Output/ellipsoid/pca/pca_eigen_vectors.txt");
}