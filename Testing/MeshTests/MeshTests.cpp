#include "Testing.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "Image.h"
#include "ParticleSystem.h"

#include <Libs/Mesh/MeshWarper.h>

#include <igl/point_mesh_squared_distance.h>

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
  femur.decimate(0.0, 0.0, false);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate(0.0, 0.0);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate3.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest4)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate(0.9, 25.5, true);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate4.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, decimateTest5)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.decimate(0.9, 25.5, false);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/decimate5.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, invertNormalsTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.invertNormals();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/invertnormals.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

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

TEST(MeshTests, probeVolumeTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.probeVolume(std::string(TEST_DATA_DIR) + "/femurVtkDT.nrrd");
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/probe.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(makeVector({0.0, 0.0, 1.0}),
                       Point3({-91.0, 0.0, 1230.0}))); // clip upper half of mesh from center
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(makeVector({0.0, 0.0, -1.0}),
                       Point3({-91.0, 0.0, 1230.0}))); // clip lower half of mesh from center
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(makeVector({-5.0, 3.14159, 1.0}),
                       Point3({-60.0, 10.0, 1235.0}))); // clip arbitrary mesh from an edge
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

// https://github.com/SCIInstitute/ShapeWorks/issues/938
// TEST(MeshTests, fixTest1)
// {
//   Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
//   femur.fix();
//   Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fix1.vtk");
//   ASSERT_TRUE(femur == ground_truth);
// }

// TEST(MeshTests, fixTest2)
// {
//   Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
//   // femur.fix(true, true, 0.5, 1, true, 0.5);
//   femur.fix();
//   Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fix1.vtk");
//   ASSERT_TRUE(femur == ground_truth);
// }

TEST(MeshTests, clipClosedSurfaceTest1)
{
  Vector v(makeVector({0, 850, 0}));
  Point o({10.0, 0.0, 10.0});

  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clipClosedSurface(makePlane(v, o));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clipClosed1.vtk");

  ASSERT_TRUE(femur == ground_truth);
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
  Image image = femur.toImage();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, toImageTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Image image = femur.toImage(makeVector({2.0, 2.0, 1.0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, toImageTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Image image = femur.toImage(makeVector({1.0, 1.0, 1.0}), {40, 145, 131});
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage3.nrrd");

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

TEST(MeshTests, coverageTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  pelvis.coverage(femur);

  Mesh baseline(std::string(TEST_DATA_DIR) + "/fm_coverage.vtk");
  ASSERT_TRUE(pelvis == baseline);
}

TEST(MeshTests, distanceTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  femur.distance(pelvis);
  pelvis.distance(femur);

  Mesh f2p(std::string(TEST_DATA_DIR) + "/meshdistance2.vtk");
  Mesh p2f(std::string(TEST_DATA_DIR) + "/meshdistance2rev.vtk");
  ASSERT_TRUE(femur == f2p);
  ASSERT_TRUE(pelvis == p2f);
}

TEST(MeshTests, distanceTest2)
{
  Mesh femur1(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh femur2(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  femur1.distance(femur2, Mesh::DistanceMethod::POINT_TO_CELL);
  femur2.distance(femur1);

  Mesh fwd(std::string(TEST_DATA_DIR) + "/meshdistance1p2c.vtk");
  Mesh rev(std::string(TEST_DATA_DIR) + "/meshdistance1rev.vtk");
  ASSERT_TRUE(femur1 == fwd);
  ASSERT_TRUE(femur2 == rev);
}

TEST(MeshTests, fieldTest1)
{
  Mesh dist(std::string(TEST_DATA_DIR) + "/meshdistance2.vtk");
  double a = dist.getFieldValue("distance", 0);
  double b = dist.getFieldValue("distance", 1000);
  double c = dist.getFieldValue("distance", dist.numPoints()-1);

  ASSERT_TRUE(std::abs(a - 0.375761) < 1e-4);
  ASSERT_TRUE(std::abs(b - 2.18114) < 1e-4);
  ASSERT_TRUE(std::abs(c - 6.915) < 1e-4);
}

TEST(MeshTests, fieldTest2)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + "/mesh1.vtk");
  double a = mesh.getFieldValue("scalars", 0);
  double b = mesh.getFieldValue("scalars", 1000);
  double c = mesh.getFieldValue("Normals", 4231);
  double d = mesh.getFieldValue("Normals", 5634);

  ASSERT_TRUE(a==1);
  ASSERT_TRUE(b==1);
  ASSERT_TRUE(std::abs(c - 0.57735) < 1e-4);
  ASSERT_TRUE(d==0);
}

//TODO: add tests for independent fields and fields on cells once #935 complete

TEST(MeshTests, icpTest)
{
  Mesh source(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh target(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  shapeworks::MeshTransform transform = source.createTransform(target);
  source.applyTransform(transform);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/icp.ply");

  ASSERT_TRUE(source == ground_truth);
}

TEST(MeshTests, generateNormalsTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.generateNormals();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/normals.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, warpTest1)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Mesh ellipsoid_warped( std::string(TEST_DATA_DIR) + "/ellipsoid_warped.ply");

  std::string staticPath = std::string(TEST_DATA_DIR) + "/ellipsoid_0.particles";
  std::string movingPath = std::string(TEST_DATA_DIR) + "/ellipsoid_1.particles";
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);

  int numParticles = staticPoints.rows() / 3;
  staticPoints.resize(3, numParticles);
  staticPoints.transposeInPlace();
  movingPoints.resize(3, numParticles);
  movingPoints.transposeInPlace();

  MeshWarper warper;
  warper.set_reference_mesh(ellipsoid.getVTKMesh(), staticPoints);
  ASSERT_TRUE(warper.get_warp_available());

  Mesh output = warper.build_mesh(movingPoints);

  ASSERT_TRUE(output == ellipsoid_warped);
}
