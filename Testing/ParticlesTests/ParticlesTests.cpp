#include <vector>
#include <string>

#include "Testing.h"

#include "ParticleSystem.h"
#include "ShapeEvaluation.h"
#include "ParticleShapeStatistics.h"
#include "ReconstructSurface.h"

using namespace shapeworks;

const std::string test_dir = std::string(TEST_DATA_DIR) + "/ellipsoid_particles/";
const std::vector<std::string> filenames = {
  test_dir + "seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_1.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_10.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_11.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_12.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_13.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_14.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_15.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_16.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_17.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_18.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_19.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_2.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_20.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_21.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles"
};

const std::vector<std::string> subFilenames = {
  test_dir + "seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_1.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_10.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles"
};

std::vector<std::string> distanceTransformsFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.DT.nrrd",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.DT.nrrd",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.DT.nrrd"
};

std::vector<std::string> localParticlesFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.local.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.local.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.local.particles"
};

std::vector<std::string> worldParticlesFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.world.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.world.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.world.particles"
};

std::string denseFile = std::string(TEST_DATA_DIR) + "/_dense.vtk";
std::string sparseFile = std::string(TEST_DATA_DIR) + "/_sparse.particles";
std::string goodPointsFile = std::string(TEST_DATA_DIR) + "/_goodPoints.txt";

TEST(ParticlesTests, pca)
{
  ParticleSystem particleSystem(subFilenames);
  ParticleShapeStatistics stats;
  stats.DoPCA(particleSystem);
  stats.PrincipalComponentProjections();
  auto pcaVec = stats.PCALoadings();

  Eigen::Matrix<double, 3, 3, Eigen::RowMajor> ground_truth;
  ground_truth << -9.47447, 1.92655, -0.698966,
                  -9.94971, -1.89538, -3.96331,
                  19.4242, -0.0311699, 4.66228;

  ASSERT_TRUE(((pcaVec - ground_truth).norm() < 1E-4));
}

TEST(ParticlesTests, compactness)
{
  ParticleSystem particleSystem(filenames);
  const double compactness = ShapeEvaluation::ComputeCompactness(particleSystem, 1);
  ASSERT_DOUBLE_EQ(compactness, 0.99178682878009183);
}

TEST(ParticlesTests, generalization)
{
  ParticleSystem particleSystem(filenames);
  const double generalization = ShapeEvaluation::ComputeGeneralization(particleSystem, 1);
  ASSERT_DOUBLE_EQ(generalization, 0.19815116412998687);
}

TEST(ParticlesTests, specificity)
{
  ParticleSystem particleSystem(filenames);
  const double specificity = ShapeEvaluation::ComputeSpecificity(particleSystem, 1);
  ASSERT_NEAR(specificity, 0.262809, 1e-1f);
}

TEST(ParticlesTests, reconstructsurfaceTestRBFS)
{
  ReconstructSurface<RBFSSparseTransform> reconstructor(denseFile, sparseFile, goodPointsFile);
  reconstructor.setOutPrefix(std::string(TEST_DATA_DIR));
  reconstructor.surface(localParticlesFiles);

  Mesh baselineDenseMesh1(std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_00.dense.vtk");
  Mesh baselineDenseMesh2(std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_01.dense.vtk");
  Mesh baselineDenseMesh3(std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_02.dense.vtk");

  Mesh denseMesh1(std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_dense.vtk");
  Mesh denseMesh2(std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_dense.vtk");
  Mesh denseMesh3(std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_dense.vtk");

  std::vector<std::string> baselineSparseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_00.sparse.particles",
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_01.sparse.particles",
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_02.sparse.particles"
  };

  std::vector<std::string> sparseParticlesFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_sparse.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_sparse.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_sparse.particles"
  };

  std::vector<std::string> baselineDenseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_00.dense.particles",
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_01.dense.particles",
  std::string(TEST_DATA_DIR) + "/RBFSSparseTransform/ellipsoid_02.dense.particles"
  };

  std::vector<std::string> denseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_dense.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_dense.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_dense.particles"
  };

  ParticleSystem baselineSparseParticles(baselineSparseParticleFiles);
  ParticleSystem sparseParticles(sparseParticlesFiles);

  ParticleSystem baselineDenseParticles(baselineDenseParticleFiles);
  ParticleSystem denseParticles(denseParticleFiles);

  ASSERT_TRUE(baselineSparseParticles.EvaluationCompare(sparseParticles) && baselineDenseParticles.EvaluationCompare(denseParticles) &&
              baselineDenseMesh1 == denseMesh1 && baselineDenseMesh2 == denseMesh2 && baselineDenseMesh3 == denseMesh3);
}

TEST(ParticlesTests, reconstructsurfaceTestThinPlateSpline)
{
  ReconstructSurface<ThinPlateSplineTransform> reconstructor(denseFile, sparseFile, goodPointsFile);
  reconstructor.setOutPrefix(std::string(TEST_DATA_DIR));
  reconstructor.surface(localParticlesFiles);

  Mesh baselineDenseMesh1(std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_00.dense.vtk");
  Mesh baselineDenseMesh2(std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_01.dense.vtk");
  Mesh baselineDenseMesh3(std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_02.dense.vtk");

  Mesh denseMesh1(std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_dense.vtk");
  Mesh denseMesh2(std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_dense.vtk");
  Mesh denseMesh3(std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_dense.vtk");

  std::vector<std::string> baselineSparseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_00.sparse.particles",
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_01.sparse.particles",
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_02.sparse.particles"
  };

  std::vector<std::string> sparseParticlesFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_sparse.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_sparse.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_sparse.particles"
  };

  std::vector<std::string> baselineDenseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_00.dense.particles",
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_01.dense.particles",
  std::string(TEST_DATA_DIR) + "/ThinPlateSplineTransform/ellipsoid_02.dense.particles"
  };

  std::vector<std::string> denseParticleFiles = {
  std::string(TEST_DATA_DIR) + "/ellipsoid_00.local_dense.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_01.local_dense.particles",
  std::string(TEST_DATA_DIR) + "/ellipsoid_02.local_dense.particles"
  };

  ParticleSystem baselineSparseParticles(baselineSparseParticleFiles);
  ParticleSystem sparseParticles(sparseParticlesFiles);

  ParticleSystem baselineDenseParticles(baselineDenseParticleFiles);
  ParticleSystem denseParticles(denseParticleFiles);

  ASSERT_TRUE(baselineSparseParticles.EvaluationCompare(sparseParticles) && baselineDenseParticles.EvaluationCompare(denseParticles) &&
              baselineDenseMesh1 == denseMesh1 && baselineDenseMesh2 == denseMesh2 && baselineDenseMesh3 == denseMesh3);
}

TEST(ParticlesTests, reconstructPCATest1)
{
  ReconstructSurface<RBFSSparseTransform> reconstructor(denseFile, sparseFile, goodPointsFile);
  auto temp_dir = TestUtils::Instance().get_output_dir("reconstruct_pca_test1");
  reconstructor.setOutPrefix(temp_dir);
  reconstructor.setOutPath(temp_dir);
  reconstructor.setNumOfParticles(128);
  reconstructor.setNumOfModes(1);
  reconstructor.setNumOfSamplesPerMode(3);
  reconstructor.samplesAlongPCAModes(worldParticlesFiles);

  Mesh baselineDenseMesh1(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test1/reconstruct_pca_test1_mode-00_sample-000_dense.vtk");
  Mesh baselineDenseMesh2(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test1/reconstruct_pca_test1_mode-00_sample-001_dense.vtk");
  Mesh baselineDenseMesh3(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test1/reconstruct_pca_test1_mode-00_sample-002_dense.vtk");

  Mesh denseMesh1(std::string(temp_dir) + "/mode-00/reconstruct_pca_test1_mode-00_sample-000_dense.vtk");
  Mesh denseMesh2(std::string(temp_dir) + "/mode-00/reconstruct_pca_test1_mode-00_sample-001_dense.vtk");
  Mesh denseMesh3(std::string(temp_dir) + "/mode-00/reconstruct_pca_test1_mode-00_sample-002_dense.vtk");

  ASSERT_TRUE(baselineDenseMesh1 == denseMesh1);
  ASSERT_TRUE(baselineDenseMesh2 == denseMesh2);
  ASSERT_TRUE(baselineDenseMesh3 == denseMesh3);
}

TEST(ParticlesTests, reconstructPCATest2)
{
  ReconstructSurface<ThinPlateSplineTransform> reconstructor(denseFile, sparseFile, goodPointsFile);
  auto temp_dir = TestUtils::Instance().get_output_dir("reconstruct_pca_test2");
  reconstructor.setOutPrefix(temp_dir);
  reconstructor.setOutPath(temp_dir);
  reconstructor.setNumOfParticles(128);
  reconstructor.setNumOfModes(1);
  reconstructor.setNumOfSamplesPerMode(3);
  reconstructor.samplesAlongPCAModes(worldParticlesFiles);

  Mesh baselineDenseMesh1(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test2/reconstruct_pca_test2_mode-00_sample-000_dense.vtk");
  Mesh baselineDenseMesh2(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test2/reconstruct_pca_test2_mode-00_sample-001_dense.vtk");
  Mesh baselineDenseMesh3(std::string(TEST_DATA_DIR) + "/reconstruct_pca_test2/reconstruct_pca_test2_mode-00_sample-002_dense.vtk");

  Mesh denseMesh1(std::string(temp_dir) + "/mode-00/reconstruct_pca_test2_mode-00_sample-000_dense.vtk");
  Mesh denseMesh2(std::string(temp_dir) + "/mode-00/reconstruct_pca_test2_mode-00_sample-001_dense.vtk");
  Mesh denseMesh3(std::string(temp_dir) + "/mode-00/reconstruct_pca_test2_mode-00_sample-002_dense.vtk");

  ASSERT_TRUE(baselineDenseMesh1 == denseMesh1);
  ASSERT_TRUE(baselineDenseMesh2 == denseMesh2);
  ASSERT_TRUE(baselineDenseMesh3 == denseMesh3);
}

TEST(ParticlesTests, reconstructMeanSurfaceTest)
{
  ReconstructSurface<RBFSSparseTransform> reconstructor;
  auto temp_dir = TestUtils::Instance().get_output_dir("reconstruct_mean_surface");
  reconstructor.setOutPrefix(temp_dir);
  reconstructor.setOutPath(temp_dir);
  reconstructor.setNumOfParticles(128);
  reconstructor.setNumOfClusters(3);
  reconstructor.meanSurface(distanceTransformsFiles, localParticlesFiles, worldParticlesFiles);

  auto baseline_mesh = Mesh(std::string(TEST_DATA_DIR) + "/reconstruct_mean_surface.vtk");
  auto compare_mesh = Mesh(temp_dir + "/_dense_rcout.vtk");
  ASSERT_TRUE(baseline_mesh == compare_mesh);

  auto baseline_dt = Image(std::string(TEST_DATA_DIR) + "/reconstruct_mean_surface.nrrd");
  auto compare_dt = Image(temp_dir + "/_meanDT.nrrd");
  ASSERT_TRUE(baseline_dt == compare_dt);
}
