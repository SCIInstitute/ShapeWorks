#include "Testing.h"
#include "MeshUtils.h"
#include "Mesh.h"
#include <igl/point_mesh_squared_distance.h>
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
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/mesh/ellipsoid_0.ply");
  Mesh::MeshType inMesh = ellipsoid.get_poly_data();

  // read points and load Eigen stuff
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(inMesh);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(inMesh);
  std::string particlePath = std::string(TEST_DATA_DIR) + "/mesh/ellipsoid_0.particles";
  Eigen::MatrixXd staticControl = MeshUtils::pointReadFormat(particlePath, 128);

  // obtain the warp matrix
  Eigen::MatrixXd W1 = MeshUtils::generateWarpMatrix(Vref, Fref, staticControl);
  // warp the image 
  Mesh output = MeshUtils::warpMesh(particlePath, W1, Fref, 128);
  Mesh::MeshType inMesh2 = output.get_poly_data();

  // read points and load Eigen stuff
  Eigen::MatrixXd Vref2 = MeshUtils::distilVertexInfo(inMesh2);
  Eigen::MatrixXi Fref2 = MeshUtils::distilFaceInfo(inMesh2);

  Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(Vref2.rows(),0,Vref2.rows()-1);
	Eigen::VectorXd sqrD;
	Eigen::MatrixXd _2;
  Eigen::VectorXi b;
	igl::point_mesh_squared_distance(staticControl,Vref2,J,sqrD,b,_2);
  
  bool chkdst = (!(sqrD.sum())) && (!(sqrD.maxCoeff()));
  ASSERT_TRUE(chkdst);
}
