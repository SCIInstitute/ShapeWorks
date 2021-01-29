#include "Testing.h"
#include "MeshUtils.h"
#include "Mesh.h"
#include <igl/point_mesh_squared_distance.h>
#include "Eigen/Core"
#include "Eigen/Dense"
#include "ParticleSystem.h"
#include "MeshUtils.h"
#include "Image.h"

using namespace shapeworks;

TEST(MeshTests, readFailTest)
{
  try {
    Mesh mesh(std::string(TEST_DATA_DIR) + "/foo.vtk");
  } catch(...) {
    ASSERT_TRUE(true);
    return;
  }

  ASSERT_TRUE(false);
}

TEST(MeshTests, smoothTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.smooth(10, 0.01);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/smooth1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, smoothTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.smooth();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/smooth2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate(0.0, 0.0, true);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

// TODO: Karthik will add MeshTest for inverNormal

TEST(MeshTests, reflectTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.reflect(Axis::X);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/reflect1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, reflectTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.reflect(Axis::Y);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/reflect2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, fillHolesTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.fillHoles();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fillholes.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, probeTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.probeVolume(std::string(TEST_DATA_DIR) + "/femurVtkDT.nrrd");
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/probe.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  // clip upper half of mesh from center
  femur.clip(makePlane(makeVector({0.0,0.0,1.0}), Point3({-91.0, 0.0, 1230.0})));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  // clip lower half of mesh from center
  femur.clip(makePlane(makeVector({0.0,0.0,-1.0}), Point3({-91.0, 0.0, 1230.0})));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  // clip arbitrary mesh from an edge
  femur.clip(makePlane(makeVector({-5.0,3.14159,1.0}), Point3({-60.0, 10.0, 1235.0})));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip3.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({-10.0,-10.0,-10.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({0,0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate3.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, scaleTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.scale(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/scale1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, scaleTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.scale(makeVector({-1.0, 1.5, 1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/scale2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, fixTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
  femur.fix();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fix1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

// TODO: fix is broken (lol)
// TEST(MeshTests, fixTest2)
// {
//   Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
//   femur.fix(true, true, true, 1.0, 10, true, 1.0);
//   Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fix2.vtk");

//   ASSERT_TRUE(femur == ground_truth);
// }

TEST(MeshTests, distanceTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double distance = femur.hausdorffDistance(pelvis);

  ASSERT_TRUE(std::abs(distance - 32.2215) < 1e-4);
}

TEST(MeshTests, relativeDistanceABTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double relativeDistanceAB = femur.relativeDistanceAtoB(pelvis);

  ASSERT_TRUE(std::abs(relativeDistanceAB - 32.2215) < 1e-4);
}

TEST(MeshTests, relativeDistanceBATest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double relativeDistanceBA = femur.relativeDistanceBtoA(pelvis);

  ASSERT_TRUE(std::abs(relativeDistanceBA - 16.1937) < 1e-4);
}

TEST(MeshTests, rasterizationOriginTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Region region = femur.boundingBox();
  Point3 origin({-114, -22, 1209});

  ASSERT_TRUE(femur.rasterizationOrigin(region) == origin);
}

TEST(MeshTests, rasterizationOriginTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  std::vector <Mesh> meshes;
  meshes.push_back(femur);
  meshes.push_back(pelvis);
  Region region = MeshUtils::boundingBox(meshes);
  Point3 origin({-114, -22, 1202});

  ASSERT_TRUE(femur.rasterizationOrigin(region) == origin);
}

TEST(MeshTests, rasterizationSizeTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Region region = femur.boundingBox();
  Dims size({45, 45, 41});

  ASSERT_TRUE(femur.rasterizationSize(region) == size);
}

TEST(MeshTests, rasterizationSizeTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  std::vector <Mesh> meshes;
  meshes.push_back(femur);
  meshes.push_back(pelvis);
  Region region = MeshUtils::boundingBox(meshes);
  Dims size({48, 51, 54});

  ASSERT_TRUE(femur.rasterizationSize(region) == size);
}

TEST(MeshTests, centerTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Point3 center(femur.center());

  ASSERT_TRUE(epsEqual(center, Point3({90.7541, -160.557, -673.572}), 1e-3));
}

TEST(MeshTests, centerofmassTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Point3 com(femur.centerOfMass());

  ASSERT_TRUE(epsEqual(com, Point3({92.6201, -157.662, -666.611}), 1e-3));
}

TEST(MeshTests, toImageTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Region region = femur.boundingBox();
  Image image = femur.toImage(makeVector({1.0,1.0,1.0}), femur.rasterizationSize(region), femur.rasterizationOrigin(region));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, antialiasTest3)
{
  Image aa1(Mesh(std::string(TEST_DATA_DIR) + "/femur.ply").toImage());
  aa1.antialias(50, 0.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/antialias3.nrrd");

  ASSERT_TRUE(aa1 == ground_truth);
}

TEST(MeshTests, toDistanceTransformTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Image image = femur.toDistanceTransform();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurDT.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// <ctc> add toImage and toDT tests that specify params
// also, think of a way to specify padding automatically computing origin and size

TEST(MeshTests, coverageTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  pelvis.coverage(femur);

  Mesh baseline(std::string(TEST_DATA_DIR) + "/fm_coverage.vtk");
  ASSERT_TRUE(pelvis.comparePointsEqual(baseline));
  ASSERT_TRUE(pelvis.compareScalarsEqual(baseline));
}

TEST(MeshTests, icpTest)
{
  Mesh source(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh target(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  shapeworks::vtkTransform transform = source.createTransform(target);
  source.applyTransform(transform);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/icp.ply");

  ASSERT_TRUE(source == ground_truth);
}


TEST(MeshTests, warpTest1)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(ellipsoid);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(ellipsoid);
  std::string particlePath = std::string(TEST_DATA_DIR) + "/ellipsoid_0.particles";
  std::vector<std::string> paths;
  paths.push_back(particlePath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd staticPts = particlesystem.Particles();
  staticPts.resize(3, 128);
  Eigen::MatrixXd W1 = MeshUtils::generateWarpMatrix(Vref, Fref, staticPts.transpose());
  Mesh output = MeshUtils::warpMesh(staticPts.transpose(), W1, Fref);
  Eigen::MatrixXd Vref2 = MeshUtils::distilVertexInfo(output);
  Eigen::MatrixXi Fref2 = MeshUtils::distilFaceInfo(output);
  Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(Vref2.rows(),0,Vref2.rows()-1);
	Eigen::VectorXd sqrD;
	Eigen::MatrixXd _2;
  Eigen::VectorXi b;
	igl::point_mesh_squared_distance(staticPts.transpose(),Vref2,J,sqrD,b,_2);
  bool chkdst = (!(sqrD.sum() > 1.0e-10)) && (!(sqrD.maxCoeff() > 1.0e-10));
  ASSERT_TRUE(chkdst);
}

TEST(MeshTests, warpTest2)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Mesh ellipsoid_warped( std::string(TEST_DATA_DIR) + "/ellipsoid_warped.ply");
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(ellipsoid);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(ellipsoid);
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
  Eigen::MatrixXd W = MeshUtils::generateWarpMatrix(Vref, Fref, staticPoints.transpose());
  Mesh output = MeshUtils::warpMesh(movingPoints.transpose(), W, Fref);
  ASSERT_TRUE(output == ellipsoid_warped);
}
