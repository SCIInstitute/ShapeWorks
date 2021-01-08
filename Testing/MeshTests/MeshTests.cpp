#include "Testing.h"

#include "Mesh.h"
#include "MeshUtils.h"

using namespace shapeworks;

TEST(MeshTests, coverageTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  pelvis.coverage(femur);

  Mesh baseline(std::string(TEST_DATA_DIR) + "/coverage.vtk");
  ASSERT_TRUE(pelvis.compare_points_equal(baseline));
  ASSERT_TRUE(pelvis.compare_scalars_equal(baseline));
  ASSERT_TRUE(true);
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

TEST(MeshTests, clipTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(MeshUtils::createPlane(makeVector({0.0,0.0,0.0}), Point3({0.0,0.0,0.0})));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, scaleTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.scale(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/scale1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, distanceTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double distance = femur.hausdorffDistance(pelvis);

  ASSERT_TRUE(compare_double(distance, 32.2215));
}

TEST(MeshTests, relativeDistanceABTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double relativeDistanceAB = femur.relativeDistanceAtoB(pelvis);

  ASSERT_TRUE(compare_double(relativeDistanceAB, 32.2215));
}

TEST(MeshTests, relativeDistanceBATest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  double relativeDistanceBA = femur.relativeDistanceBtoA(pelvis);

  ASSERT_TRUE(compare_double(relativeDistanceBA, 16.1937));
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