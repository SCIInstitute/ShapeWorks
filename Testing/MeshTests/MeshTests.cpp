#include "Testing.h"

#include "Mesh.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(MeshTests, coverage_test) {

  std::string test_location(std::string(TEST_DATA_DIR) + "/coverage/");

  Mesh femur(test_location + "femur.vtk");
  Mesh pelvis(test_location + "pelvis.vtk");
  pelvis.coverage(femur);

  // compare to baseline
  Mesh baseline(test_location + "baseline.vtk");
  ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
}
