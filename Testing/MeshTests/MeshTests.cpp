#include "Testing.h"
#include "Eigen/Core"
#include "Eigen/Dense"
#include "Mesh.h"
#include "MeshUtils.h"

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

TEST(MeshTests, warpTest1)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/Mesh/ellipsoid_0.ply");
  Mesh::MeshType inMesh = ellipsoid.get_poly_data();
  // read points and load Eigen stuff
  Eigen::MatrixXd* Vref;
  Eigen::MatrixXi* Fref;
  Eigen::MatrixXd staticControl = MeshUtils::pointReadFormat(std::string(TEST_DATA_DIR) + "/ellipsoid_particles/seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles", 128)
  MeshUtils::distilToEigen(inMesh, &Vref, &Fref);
  // obtain the warp matrix
  Eigen::MatrixXd W = MeshUtils::generateWarpMatrix(&Vref, &Fref, staticControl);
  // warp the image 
  Mesh output = MeshUtils::warpMesh(std::string(TEST_DATA_DIR) + "/ellipsoid_particles/seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles", W, &Fref, 128)
  
  ASSERT_TRUE(output == ellipsoid);
}
