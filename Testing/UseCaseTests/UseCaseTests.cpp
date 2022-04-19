#include <boost/filesystem/operations.hpp>
#include <fstream>

#include "Testing.h"

//---------------------------------------------------------------------------
void run_use_case(const std::string& name) {
  // change to the python examples directory
  boost::filesystem::current_path(PYTHON_EXAMPLES_DIR);

  // delete the out file to make sure it's remade
  const std::string outputname = name + "_output.txt";
  std::remove(outputname.c_str());

  // run python
  std::string command = "python RunUseCase.py " + name + " --tiny_test 1>" + outputname + " 2>&1";
  // use the below instead of there is some problem in getting the output
  // std::string command = "python RunUseCase.py " + name + " --tiny_test";
  std::cerr << "Running command: " << command << "\n";
  bool result = system(command.c_str());
  if (result) {
    std::cerr << "Error running command, output: " << command << "\n";
    std::ifstream file(outputname.c_str());
    if (file.good()) {
      std::string line;
      while (std::getline(file, line)) {
        std::cerr << line << "\n";
      }
    }
  }
  ASSERT_FALSE(result);
}

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid) { run_use_case("ellipsoid"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_mesh) { run_use_case("ellipsoid_mesh"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_fd) { run_use_case("ellipsoid_fd"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_cut) { run_use_case("ellipsoid_cut"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_evaluate) { run_use_case("ellipsoid_evaluate"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, lumps) { run_use_case("lumps"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, left_atrium) { run_use_case("left_atrium"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_cut) { run_use_case("femur_cut"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, deep_ssm) { run_use_case("deep_ssm"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, supershapes_1mode_contour) { run_use_case("supershapes_1mode_contour"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, thin_cavity_bean) { run_use_case("thin_cavity_bean"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_multiple_domain) { run_use_case("ellipsoid_multiple_domain"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_multiple_domain_mesh) { run_use_case("ellipsoid_multiple_domain_mesh"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, ellipsoid_pca) { run_use_case("ellipsoid_pca"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, femur_pvalues) { run_use_case("femur_pvalues"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, peanut_shared_boundary) { run_use_case("peanut_shared_boundary"); }

//---------------------------------------------------------------------------
TEST(UseCaseTests, incremental_supershapes) { run_use_case("incremental_supershapes"); }
