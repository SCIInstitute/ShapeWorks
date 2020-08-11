#include "Testing.h"

#include "Mesh.h"

using namespace shapeworks;

TEST(MeshTests, coverageTest) 
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/coverage/");

  Mesh femur(test_location + "femur.vtk");
  Mesh pelvis(test_location + "pelvis.vtk");
  pelvis.coverage(femur);

  // compare to baseline
  Mesh baseline(test_location + "baseline.vtk");
  ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
}

TEST(MeshTests, smoothTest) 
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/smooth/");

  Mesh femur(test_location + "femur.vtk");
  femur.smooth(10, 0.01);
  Mesh ground_truth(test_location + "smooth_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}

TEST(MeshTests, reflectTest)
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/reflect-mesh/");

  Mesh femur(test_location + "femur.vtk");
  femur.reflect(Axis::X);
  Mesh ground_truth(test_location + "reflect_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}
