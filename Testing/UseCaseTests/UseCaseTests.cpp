#include "Testing.h"

#include <fstream>

#ifdef _WIN32
  #include <io.h>
  #define access _access_s
#else

  #include <unistd.h>
#endif

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
  std::string file = std::string(PYTHON_EXAMPLES_DIR) + "/" + check_file;

  // change to the python examples directory
  chdir(std::string(PYTHON_EXAMPLES_DIR).c_str());

  // delete the file to make sure it's remade
  std::remove(file.c_str());
  std::remove("output.txt");

  // run python
  std::string command = "python RunUseCase.py " + name + " --tiny_test 1> output.txt 2>&1";
  std::cerr << "Running command: " << command << "\n";
  bool result = system(command.c_str());
  if (!result) {
    std::cerr << "Error running command, output: " << command << "\n";
    std::ifstream file("output.txt");
    if (file.good()) {
      std::string line;
      while (std::getline(file, line)) {
        std::cerr << line << "\n";
      }
    }
  }
  ASSERT_FALSE(result);

  ASSERT_TRUE(file_exists(file));
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid)
{
  run_use_case("ellipsoid",
               "Output/ellipsoid/shape_models/32/ellipsoid_00_local.particles");
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
               "Output/ellipsoid_fd/shape_models/128/ellipsoid_00.isores.center.com.aligned.cropped.tpSmoothDT_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_cut)
{
  run_use_case("ellipsoid_cut",
               "Output/ellipsoid_cut/shape_models/16/ellipsoid_00_local.particles");
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
               "Output/left_atrium/shape_models/32/CARMA0046.laendo_no_veins_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur)
{
  run_use_case("femur",
               "Output/femur/shape_models/32/m03_L_femur_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_groom_images)
{
  run_use_case("femur --groom_images",
               "Output/femur/shape_models/32/m03_L_femur_local.particles");
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
               "Output/femur_cut/shape_models/32/m03_L_femur_local.particles");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, deep_ssm)
{
  run_use_case("deep_ssm",
               "Output/deep_ssm/femur_deepssm/predictions/PCA_Predictions/predicted_pca_m07_L.particles");
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
TEST(UseCaseTests, ellipsoid_multiple_domain)
{
  run_use_case("ellipsoid_multiple_domain",
               "Output/ellipsoid_multiple_domain/shape_models/32_32/ellipsoid_joint_00_d1_local.particles");
}
//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_multiple_domain_mesh)
{
  run_use_case("ellipsoid_multiple_domain_mesh",
               "Output/ellipsoid_multiple_domain_mesh/shape_models/32_32/ellipsoid_joint_00_d1_local.particles");
}
//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_pca)
{
  run_use_case("ellipsoid_pca",
               "Output/ellipsoid/pca/pca_eigen_vectors.txt");
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_pvalues)
{
  run_use_case("femur_pvalues",
               "Output/femur_pvalues/femur_pvalues.txt");
}
