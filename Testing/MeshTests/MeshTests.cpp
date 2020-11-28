#include "Testing.h"

#include "Mesh.h"

using namespace shapeworks;

TEST(MeshTests, coverage_test) {
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  pelvis.coverage(femur);

  Mesh baseline(std::string(TEST_DATA_DIR) + "/baseline.vtk");
  ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
  ASSERT_TRUE(true);
}
