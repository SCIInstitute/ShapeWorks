#include <gtest/gtest.h>

#include <Libs/Mesh/Mesh.h>

#include "TestConfiguration.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(MeshTests, coverage_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/coverage/");

  Mesh femur(test_location + "femur.vtk");
  Mesh pelvis(test_location + "pelvis.vtk");

  // run and check
  // ASSERT_TRUE(pelvis.coverage(femur));
  pelvis.coverage(femur);

  // to write out the current mesh
  //pelvis.write("/tmp/out.vtk");

  // compare to baseline
  Mesh baseline(test_location + "baseline.vtk");
  // ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  // ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
  ASSERT_FALSE(pelvis.compare_points_equal(baseline));
  ASSERT_FALSE(pelvis.compare_scalars_equal(baseline));
}

//TEST(MeshTests, next_test) {

// ...

// }
