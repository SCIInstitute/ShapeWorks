#include <vtkDataArray.h>

#include <string>
#include <vector>

#include "CorrespondenceEvaluation.h"
#include "Libs/Optimize/Domain/Surface.h"
#include "Mesh/Mesh.h"
#include "ParticleNormalEvaluation.h"
#include "ParticleShapeStatistics.h"
#include "ParticleSystemEvaluation.h"
#include "ReconstructSurface.h"
#include "ShapeEvaluation.h"
#include "Testing.h"

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
  ParticleSystemEvaluation ParticleSystemEvaluation(subFilenames);
  ParticleShapeStatistics stats;
  stats.do_pca(ParticleSystemEvaluation);
  stats.principal_component_projections();
  auto pcaVec = stats.get_pca_loadings();

  Eigen::Matrix<double, 3, 2, Eigen::RowMajor> ground_truth;
  ground_truth << -9.47447, 1.92655,
      -9.94971, -1.89538,
      19.4242, -0.0311699;

  ASSERT_LE((pcaVec.block<3,2>(0,0) - ground_truth).norm(), 1E-4);
}

TEST(ParticlesTests, compactness)
{
  ParticleSystemEvaluation ParticleSystemEvaluation(filenames);
  const double compactness = ShapeEvaluation::compute_compactness(ParticleSystemEvaluation, 1);
  ASSERT_DOUBLE_EQ(compactness, 0.99178682878009183);
}

TEST(ParticlesTests, generalization)
{
  ParticleSystemEvaluation ParticleSystemEvaluation(filenames);
  const double generalization = ShapeEvaluation::compute_generalization(ParticleSystemEvaluation, 1);
  ASSERT_DOUBLE_EQ(generalization, 0.19815116412998687);
}

TEST(ParticlesTests, specificity)
{
  ParticleSystemEvaluation ParticleSystemEvaluation(filenames);
  const double specificity = ShapeEvaluation::compute_specificity(ParticleSystemEvaluation, 1);
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

  ParticleSystemEvaluation baselineSparseParticles(baselineSparseParticleFiles);
  ParticleSystemEvaluation sparseParticles(sparseParticlesFiles);

  ParticleSystemEvaluation baselineDenseParticles(baselineDenseParticleFiles);
  ParticleSystemEvaluation denseParticles(denseParticleFiles);

  ASSERT_TRUE(baselineSparseParticles.evaluation_compare(sparseParticles) && baselineDenseParticles.evaluation_compare(denseParticles) &&
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

  ParticleSystemEvaluation baselineSparseParticles(baselineSparseParticleFiles);
  ParticleSystemEvaluation sparseParticles(sparseParticlesFiles);

  ParticleSystemEvaluation baselineDenseParticles(baselineDenseParticleFiles);
  ParticleSystemEvaluation denseParticles(denseParticleFiles);

  ASSERT_TRUE(baselineSparseParticles.evaluation_compare(sparseParticles) && baselineDenseParticles.evaluation_compare(denseParticles) &&
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

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
TEST(ParticlesTests, particle_normal_evaluation_test)
{
  Mesh mesh1(std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals1_groomed.vtk");
  Mesh mesh2(std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals2_groomed.vtk");
  Mesh mesh3(std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals3_groomed.vtk");

  std::vector<std::shared_ptr<Surface>> meshes;
  meshes.push_back(std::make_shared<Surface>(mesh1.getVTKMesh()));
  meshes.push_back(std::make_shared<Surface>(mesh2.getVTKMesh()));
  meshes.push_back(std::make_shared<Surface>(mesh3.getVTKMesh()));

  std::vector<std::string> particle_files = {
    std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals_particles/particle_normals1_groomed_groomed_local.particles",
    std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals_particles/particle_normals2_groomed_groomed_local.particles",
    std::string(TEST_DATA_DIR) + "/particle_normals/particle_normals_particles/particle_normals3_groomed_groomed_local.particles"
  };

  ParticleSystemEvaluation system(particle_files);
  auto particles = system.get_matrix();

  auto eval = [&](double angle, int expected_good_count) {
    auto normals = ParticleNormalEvaluation::compute_particle_normals(particles, meshes);
    auto angles = ParticleNormalEvaluation::evaluate_particle_normals(particles, normals);
    auto good_bad = ParticleNormalEvaluation::threshold_particle_normals(angles, angle);
    int good_count = std::count(good_bad.begin(), good_bad.end(), true);
    ASSERT_EQ(good_count, expected_good_count);
  };

  eval(80, 128); // at 80 degrees, all good
  eval(45, 123); // at 45 degrees a few bad
  eval(5, 9); // at 5 degrees, most are bad
  eval(1, 0); // at 1 degree, all bad
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace {

//! the first `columns` columns of the NxN grid below, so the spacing is the same whatever the width
Mesh make_grid_strip_mesh(int n, int columns) {
  Eigen::MatrixXd points(n * columns, 3);
  for (int y = 0; y < n; y++) {
    for (int x = 0; x < columns; x++) {
      points.row(y * columns + x) << static_cast<double>(x) / (n - 1), static_cast<double>(y) / (n - 1), 0.0;
    }
  }

  Eigen::MatrixXi faces(2 * (n - 1) * (columns - 1), 3);
  int f = 0;
  for (int y = 0; y < n - 1; y++) {
    for (int x = 0; x < columns - 1; x++) {
      const int i = y * columns + x;
      faces.row(f++) << i, i + 1, i + columns;
      faces.row(f++) << i + 1, i + columns + 1, i + columns;
    }
  }
  return Mesh(points, faces);
}

//! flat NxN grid of triangles in the z=0 plane, spanning [0,1] in x and y
Mesh make_grid_mesh(int n) { return make_grid_strip_mesh(n, n); }

}  // namespace

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, identicalMeshesHaveNoDistance) {
  Mesh mesh = make_grid_mesh(10);

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(mesh.getVTKMesh(), mesh,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  ASSERT_NEAR(row.mean_dist, 0.0, 1e-9);
  ASSERT_NEAR(row.median_dist, 0.0, 1e-9);
  ASSERT_NEAR(row.p99_dist, 0.0, 1e-9);
  ASSERT_NEAR(row.max_dist, 0.0, 1e-9);
  ASSERT_GT(row.bbox_diag, 0.0);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, uniformOffsetMeasuresThatOffset) {
  const double offset = 0.25;
  Mesh groomed = make_grid_mesh(10);

  Mesh shifted = make_grid_mesh(10);
  shifted.translate(makeVector({0, 0, offset}));

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(shifted.getVTKMesh(), groomed,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  // every vertex is the same distance from the target plane, so all the statistics agree
  ASSERT_NEAR(row.mean_dist, offset, 1e-6);
  ASSERT_NEAR(row.median_dist, offset, 1e-6);
  ASSERT_NEAR(row.max_dist, offset, 1e-6);

  // the target is the flat grid, so its bounding box diagonal is that of the unit square
  ASSERT_NEAR(row.bbox_diag, std::sqrt(2.0), 1e-6);
  ASSERT_NEAR(row.norm_mean, offset / std::sqrt(2.0), 1e-6);
}

//---------------------------------------------------------------------------
// The failure this metric exists to catch: a few swapped correspondence points leave most of the
// surface intact, so the mean barely moves while the tail spikes.
TEST(CorrespondenceEvaluationTests, localizedDefectSpikesTheTailNotTheMean) {
  const int n = 20;  // 400 vertices, so p99 and max land on different ones
  Mesh groomed = make_grid_mesh(n);

  Mesh damaged = make_grid_mesh(n);
  auto poly_data = damaged.getVTKMesh();
  double point[3];
  poly_data->GetPoint(0, point);
  point[2] += 1.0;  // drag a single vertex well off the surface
  poly_data->GetPoints()->SetPoint(0, point);
  poly_data->Modified();

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(poly_data, groomed,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  ASSERT_NEAR(row.median_dist, 0.0, 1e-9);      // the surface is otherwise untouched
  ASSERT_NEAR(row.max_dist, 1.0, 1e-6);         // the moved vertex
  ASSERT_LT(row.mean_dist, 0.01);               // one vertex in 400 barely moves the mean
  ASSERT_GT(row.max_dist / row.mean_dist, 50);  // which is exactly why ranking on the mean hides it

  // p99 ignores the single outlier, so it is a steadier basis for the localization ratio than max
  ASSERT_LT(row.p99_dist, row.max_dist);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, summarizeReportsOrderStatistics) {
  std::vector<double> values{5.0, 1.0, 4.0, 2.0, 3.0};
  auto stats = CorrespondenceEvaluation::summarize(values);

  ASSERT_NEAR(stats.mean, 3.0, 1e-9);
  ASSERT_NEAR(stats.median, 3.0, 1e-9);
  ASSERT_NEAR(stats.max, 5.0, 1e-9);

  ASSERT_NEAR(CorrespondenceEvaluation::summarize({}).mean, 0.0, 1e-9);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, aggregatesExcludeTheTemplate) {
  CorrespondenceQualityReport report;
  report.rows.push_back({"a", 0, 1.0, 1.0, 1.0, 1.0, 10.0, 0.1, 0.1, 0.1, 0.1, false});
  report.rows.push_back({"b", 0, 3.0, 3.0, 3.0, 3.0, 10.0, 0.3, 0.3, 0.3, 0.3, false});
  report.rows.push_back({"t", 0, 99.0, 99.0, 99.0, 99.0, 10.0, 9.9, 9.9, 9.9, 9.9, true});

  CorrespondenceEvaluation::compute_aggregates(report);

  ASSERT_EQ(report.num_evaluated, 2);
  ASSERT_EQ(report.num_template_rows, 1);
  ASSERT_NEAR(report.agg_raw.mean, 2.0, 1e-9);  // the template row would have dominated this
  ASSERT_NEAR(report.agg_raw.max, 3.0, 1e-9);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, identicalMeshesHaveNoDisagreement) {
  Mesh mesh = make_grid_mesh(10);

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(mesh.getVTKMesh(), mesh,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  ASSERT_NEAR(row.push.max, 0.0, 1e-9);
  ASSERT_NEAR(row.disagreement.max, 0.0, 1e-9);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, uniformOffsetIsTheSameFromEitherSide) {
  const double offset = 0.25;
  Mesh groomed = make_grid_mesh(10);
  Mesh shifted = make_grid_mesh(10);
  shifted.translate(makeVector({0, 0, offset}));

  for (auto method : {CorrespondenceEvaluation::DistanceMethod::PointToCell,
                      CorrespondenceEvaluation::DistanceMethod::PointToPoint}) {
    auto row = CorrespondenceEvaluation::evaluate_reconstruction(shifted.getVTKMesh(), groomed, method);

    ASSERT_NEAR(row.push.mean, offset, 1e-6);
    ASSERT_NEAR(row.push.max, offset, 1e-6);
    ASSERT_NEAR(row.disagreement.mean, offset, 1e-6);
    ASSERT_NEAR(row.disagreement.norm_mean, offset / std::sqrt(2.0), 1e-6);
  }
}

//---------------------------------------------------------------------------
// What pull alone misses: a reconstruction that covers only part of the groomed surface lies exactly
// on it, so every reconstructed vertex is at distance zero however much surface is missing.
TEST(CorrespondenceEvaluationTests, missingRegionIsCaughtByPushNotPull) {
  const int n = 21;  // puts a column of vertices at x = 0.5
  Mesh groomed = make_grid_mesh(n);
  Mesh half = make_grid_strip_mesh(n, 11);  // x <= 0.5 only

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(half.getVTKMesh(), groomed,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  ASSERT_NEAR(row.max_dist, 0.0, 1e-9);  // pull sees nothing wrong

  // each groomed vertex beyond x = 0.5 is (x - 0.5) from the edge of the reconstruction
  ASSERT_NEAR(row.push.max, 0.5, 1e-6);
  ASSERT_NEAR(row.disagreement.max, 0.5, 1e-6);

  // weighted by area over the unit square, the mean of max(0, x - 0.5) is 1/8; unweighted, the
  // grid's half-area boundary vertices would pull it away from that
  ASSERT_NEAR(row.push.mean, 0.125, 1e-6);
  ASSERT_NEAR(row.disagreement.mean, 0.125, 1e-6);
}

//---------------------------------------------------------------------------
// And what push alone misses: one reconstructed vertex dragged off the surface leaves the groomed
// surface covered, so only its pull distance, carried onto the groomed vertices beneath it, sees it.
TEST(CorrespondenceEvaluationTests, offSurfaceSpikeIsCaughtByPullNotPush) {
  const int n = 20;
  Mesh groomed = make_grid_mesh(n);

  Mesh damaged = make_grid_mesh(n);
  auto poly_data = damaged.getVTKMesh();
  double point[3];
  poly_data->GetPoint(0, point);
  point[2] += 1.0;
  poly_data->GetPoints()->SetPoint(0, point);
  poly_data->Modified();

  auto row = CorrespondenceEvaluation::evaluate_reconstruction(poly_data, groomed,
                                                               CorrespondenceEvaluation::DistanceMethod::PointToCell);

  ASSERT_NEAR(row.max_dist, 1.0, 1e-6);  // pull sees the spike
  ASSERT_LT(row.push.max, 0.1);          // the groomed surface is still covered
  ASSERT_NEAR(row.disagreement.max, 1.0, 1e-6);
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, disagreementFieldLivesOnTheGroomedMesh) {
  const int n = 21;
  Mesh groomed = make_grid_mesh(n);
  Mesh half = make_grid_strip_mesh(n, 11);

  vtkSmartPointer<vtkDataArray> pull;
  vtkSmartPointer<vtkDataArray> disagreement;
  vtkSmartPointer<vtkDataArray> push;
  CorrespondenceEvaluation::evaluate_reconstruction(
      half.getVTKMesh(), groomed, CorrespondenceEvaluation::DistanceMethod::PointToCell, &pull, &disagreement, &push);

  // pull belongs to the reconstruction's vertices; the other two to the groomed mesh's, where a gap can show
  ASSERT_TRUE(pull && disagreement && push);
  ASSERT_EQ(pull->GetNumberOfTuples(), static_cast<vtkIdType>(half.numPoints()));
  ASSERT_EQ(disagreement->GetNumberOfTuples(), static_cast<vtkIdType>(groomed.numPoints()));
  ASSERT_EQ(push->GetNumberOfTuples(), static_cast<vtkIdType>(groomed.numPoints()));
  ASSERT_EQ(std::string(disagreement->GetName()), "disagreement");
  ASSERT_EQ(std::string(push->GetName()), "push");
}

//---------------------------------------------------------------------------
TEST(CorrespondenceEvaluationTests, disagreementAggregatesExcludeTheTemplate) {
  CorrespondenceQualityReport report;
  auto add_row = [&report](const std::string& subject, double value, bool is_template) {
    CorrespondenceQualityRow row;
    row.subject = subject;
    row.is_template = is_template;
    row.push.mean = value / 2.0;
    row.disagreement.mean = value;
    row.disagreement.norm_mean = value / 10.0;
    report.rows.push_back(row);
  };
  add_row("a", 1.0, false);
  add_row("b", 3.0, false);
  add_row("t", 99.0, true);

  CorrespondenceEvaluation::compute_aggregates(report);

  ASSERT_NEAR(report.agg_disagreement_raw.mean, 2.0, 1e-9);
  ASSERT_NEAR(report.agg_disagreement_norm.max, 0.3, 1e-9);
  ASSERT_NEAR(report.agg_push_raw.mean, 1.0, 1e-9);
}
