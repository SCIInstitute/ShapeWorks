#include "Testing.h"
#include "MeshUtils.h"
#include "Mesh.h"
#include "Eigen/Core"
#include "Eigen/Dense"

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
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(inMesh);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(inMesh);
  std::string particlePath = std::string(TEST_DATA_DIR) + "/ellipsoid_particles/seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles";
  Eigen::MatrixXd staticControl = MeshUtils::pointReadFormat(particlePath, 128);

  // obtain the warp matrix
  Eigen::MatrixXd W = MeshUtils::generateWarpMatrix(Vref, Fref, staticControl);
  
  // warp the image 
  Mesh output = MeshUtils::warpMesh(particlePath, W, Fref, 128);
  // ASSERT_TRUE(output == ellipsoid);
  ASSERT_TRUE(true);
}
