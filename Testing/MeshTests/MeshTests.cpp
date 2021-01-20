#include "Testing.h"
#include "MeshUtils.h"
#include "Mesh.h"
#include <igl/point_mesh_squared_distance.h>
#include "Eigen/Core"
#include "Eigen/Dense"
#include "ParticleSystem.h"

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
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Mesh::MeshType inMesh = ellipsoid.get_poly_data();

  // read points and load Eigen stuff
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(inMesh);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(inMesh);
  std::string particlePath = std::string(TEST_DATA_DIR) + "/ellipsoid_0.particles";
  std::vector<std::string> paths;
  paths.push_back(particlePath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd staticPts = particlesystem.Particles();
  staticPts.resize(3, 128);
  std::cout << staticPts.col(10) <<"cece" <<std::endl;
  // obtain the warp matrix
  Eigen::MatrixXd W1 = MeshUtils::generateWarpMatrix(Vref, Fref, staticPts.transpose());
  // warp the image 
  Mesh output = MeshUtils::warpMesh(staticPts.transpose(), W1, Fref);
  std::cout << staticPts.col(10) <<"cece" <<std::endl;

  Mesh::MeshType inMesh2 = output.get_poly_data();

  // read points and load Eigen stuff
  Eigen::MatrixXd Vref2 = MeshUtils::distilVertexInfo(inMesh2);
  Eigen::MatrixXi Fref2 = MeshUtils::distilFaceInfo(inMesh2);

  Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(Vref2.rows(),0,Vref2.rows()-1);
	Eigen::VectorXd sqrD;
	Eigen::MatrixXd _2;
  Eigen::VectorXi b;
	igl::point_mesh_squared_distance(staticPts.transpose(),Vref2,J,sqrD,b,_2);
  std::cout << sqrD.sum()<<", " << sqrD.maxCoeff() <<std::endl;
  bool chkdst = (!(sqrD.sum() > 1.0e-10)) && (!(sqrD.maxCoeff() > 1.0e-10));
  ASSERT_TRUE(chkdst);
}

TEST(MeshTests, warpTest2)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Mesh ellipsoid_warped( std::string(TEST_DATA_DIR) + "/ellipsoid_warped.ply");
  Mesh::MeshType inMesh = ellipsoid.get_poly_data();

  // read points and load Eigen stuff
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(inMesh);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(inMesh);
  std::string staticPath = std::string(TEST_DATA_DIR) + "/ellipsoid_0.particles";
  std::string movingPath = std::string(TEST_DATA_DIR) + "/ellipsoid_1.particles";
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);
  staticPoints.resize(3, 128);
  movingPoints.resize(3, 128);

  // obtain the warp matrix
  Eigen::MatrixXd W = MeshUtils::generateWarpMatrix(Vref, Fref, staticPoints.transpose());
  // warp the image 
  Mesh output = MeshUtils::warpMesh(movingPoints.transpose(), W, Fref);
  
  ASSERT_TRUE(output == ellipsoid_warped);
}
