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
  Vector3 distance(femur.hausdorffDistance(pelvis));
  Vector3 ground_truth(makeVector({32.2215,32.2215,32.2215}));

  std::cout << distance;

  if (distance[0] == 32.2215)
    std::cout << "correct";
  else
    std::cout << "incorrect";  

  ASSERT_TRUE(distance == ground_truth);
}

TEST(MeshTests, relativeDistanceABTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  Vector relativeDistanceAB = femur.relativeDistanceAtoB(pelvis);

  ASSERT_TRUE(relativeDistanceAB == makeVector({32.2215, 32.2215, 32.2215}));
}

TEST(MeshTests, relativeDistanceBATest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  Vector relativeDistanceBA = femur.relativeDistanceBtoA(pelvis);

  ASSERT_TRUE(relativeDistanceBA == makeVector({16.1937, 16.1937, 16.1937}));
}

TEST(MeshTests, rasterizationOriginTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh::Region region = femur.boundingBox();
  Point3 origin({-113, -21, 1209});

  ASSERT_TRUE(femur.rasterizationOrigin(region) == origin);
}

TEST(MeshTests, rasterizationSizeTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh::Region region = femur.boundingBox();
  Dims size({44, 43, 40});

  ASSERT_TRUE(femur.rasterizationSize(region) == size);
}