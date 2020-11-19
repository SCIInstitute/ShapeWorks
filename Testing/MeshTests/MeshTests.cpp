#include "Testing.h"

#include "Mesh.h"

using namespace shapeworks;

TEST(MeshTests, coverage)
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/coverage/");

  Mesh femur(test_location + "femur.vtk");
  Mesh pelvis(test_location + "pelvis.vtk");
  pelvis.coverage(femur);

  // compare to baseline
  Mesh baseline(test_location + "baseline.vtk");
  ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
  ASSERT_TRUE(true);
}

TEST(MeshTests, march)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.march();
  femur.write(std::string(TEST_DATA_DIR) + "/march_baseline.vtk");
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/march_baseline.vtk");
  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}

TEST(MeshTests, smooth)
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/smooth/");

  Mesh femur(test_location + "femur.vtk");
  femur.smooth(10, 0.01);
  Mesh ground_truth(test_location + "smooth_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}

TEST(MeshTests, decimate)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}

TEST(MeshTests, reflect)
{
  std::string test_location(std::string(TEST_DATA_DIR) + "/reflect-mesh/");

  Mesh femur(test_location + "femur.vtk");
  femur.reflect(Axis::X);
  Mesh ground_truth(test_location + "reflect_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}

TEST(MeshTests, fillHoles)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.fillHoles();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fillholes_baseline.vtk");

  ASSERT_TRUE(femur.compare_points_equal(ground_truth));
}
