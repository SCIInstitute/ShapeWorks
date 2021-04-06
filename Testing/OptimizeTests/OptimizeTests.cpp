#include <cstdio>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkApproximateSignedDistanceMapImageFilter.h>

#include "Testing.h"

#include "Optimize.h"
#include "OptimizeParameterFile.h"
#include <Libs/Project/Project.h>
#include <Libs/Optimize/OptimizeParameters.h>
#include "ParticleShapeStatistics.h"
#include "VtkMeshWrapper.h"
#include <vtkPLYReader.h>
#include <Libs/Mesh/MeshUtils.h>

using namespace shapeworks;

// until we have a "groom" library we can call
static void prep_distance_transform(std::string input, std::string output)
{
  using ImageType = itk::Image<float, 3>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using WriterType = itk::ImageFileWriter<ImageType>;

  // Create and setup a reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(input.c_str());

  using FilterType = itk::ApproximateSignedDistanceMapImageFilter<ImageType, ImageType>;
  FilterType::Pointer dt = FilterType::New();
  dt->SetInput(reader->GetOutput());
  dt->SetInsideValue(0);
  dt->SetOutsideValue(1);
  dt->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(output.c_str());
  writer->SetInput(dt->GetOutput());
  writer->SetUseCompression(true);
  writer->Update();
}

TEST(OptimizeTests, sample) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sphere");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("output/sphere10_DT_world.particles");

  // run with parameter file
  std::string paramfile = std::string("sphere.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // If Procrustes scaling is working, this should be small.
  // Otherwise it is quite large (>4000).
  double value = values[values.size() - 1];
  ASSERT_LT(value, 100);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, open_mesh_test) {

    std::cerr << "open_mesh_test\n";
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/hemisphere");
  chdir(test_location.c_str());

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("output/hemisphere_world.particles");

  // run with parameter file
  std::string paramfile = std::string("hemisphere.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();
    std::cerr << "finished running\n";

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // If Procrustes scaling is working, this should be small.
  // Otherwise it is quite large (>4000).
  double value = values[values.size() - 1];
  ASSERT_LT(value, 100);
}

TEST(OptimizeTests, fixedDomain) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/fixed_domain");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out the output file of interest
  std::remove("output/sphere40_DT_world.particles");

  // run with parameter file
  std::string paramfile = std::string("fixed_domain.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // Procrustes scaling is off for this fixed domain case, so it should be quite large. (>5000)
  // If the new non-fixed domain doesn't optimize, the value will be about 2800
  double value = values[values.size() - 1];
  ASSERT_GT(value, 5000);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, fixed_mesh_domain_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/fixed_mesh_domain");
  chdir(test_location.c_str());

  // make sure we clean out the output file of interest
  std::remove("shape_models/id0000_ss3_world.particles");

  // run with parameter file
  std::string paramfile = std::string("fixed_mesh_domain.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  double value = values[values.size() - 1];
  ASSERT_GT(value, 250);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, use_normals_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/use_normals");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out at least one output file
  std::remove("output/sphere10_DT_world.particles");

  // run with parameter file
  std::string paramfile = std::string("use_normals.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  ASSERT_GT(values[values.size() - 1], 2500);
  ASSERT_LT(values[values.size() - 2], 300);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, mesh_use_normals_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/mesh_use_normals");
  chdir(test_location.c_str());

  // make sure we clean out at least one output file
  std::remove("output/sphere_00_world.particles");

  // run with parameter file
  std::string paramfile = std::string("mesh_use_normals.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  ASSERT_GT(values[values.size() - 1], 750.0);
  ASSERT_LT(values[values.size() - 2], 10);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, cutting_plane_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/cutting_plane_multi");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out at least one output file
  std::remove("output/sphere10_DT_world.particles");

  // run with parameter file
  std::string paramfile = std::string("cutting_plane.xml");
  Optimize app;
  OptimizeParameterFile param;
  std::cout << "Loading parameters-----" << std::endl;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  std::cout << "Running-----" << std::endl;
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  // ASSERT_GT(values[values.size() - 1], 2500);
  // ASSERT_LT(values[values.size() - 2], 150);

  // next check that the cutting plane works.  Takes the means of the points and tests whether they violate the cutting plane on the first domain.
  auto points = stats.Mean();
  app.GetSampler()->GetParticleSystem()->GetDomain(0)->GetConstraints()->PrintAll();
  for (int i = 0; i < points.size(); i += 3) {
    itk::FixedArray<double, 3> p;
    p[0] = points[i]; p[1] = points[i+1]; p[2] = points[i+2];
    std::cout << p ;
    bool violated = app.GetSampler()->GetParticleSystem()->GetDomain(0)->GetConstraints()->IsAnyViolated(p);
    if(violated) std::cout << "viol"<< std::endl; else std::cout << "good"<< std::endl;
    ASSERT_TRUE(!violated);
  }
}

TEST(OptimizeTests, sphereConstraint)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sphere_constraint");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out at least one output file
  std::remove("output/sphere10_DT_world.particles");

  // run with parameter file
  std::string paramfile = std::string("sphere_constraint.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  ASSERT_GT(values[values.size() - 1], 2500);
  ASSERT_LT(values[values.size() - 2], 150);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, embedded_python_test) {
  pythonEnvSetup();

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/simple");
  chdir(test_location.c_str());

  // run with parameter file
  std::string paramfile = std::string("python_embedded.xml");
  Optimize app;
  OptimizeParameterFile param;
  ASSERT_TRUE(param.load_parameter_file(paramfile.c_str(), &app));
  bool result = app.Run();

  ASSERT_TRUE(result);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, project_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sphere");
  chdir(test_location.c_str());

  // prep/groom
  prep_distance_transform("sphere10.nrrd", "sphere10_DT.nrrd");
  prep_distance_transform("sphere20.nrrd", "sphere20_DT.nrrd");
  prep_distance_transform("sphere30.nrrd", "sphere30_DT.nrrd");
  prep_distance_transform("sphere40.nrrd", "sphere40_DT.nrrd");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("output/sphere10_DT_world.particles");

  Optimize app;

  // set up optimizer from project
  ProjectHandle project = std::make_shared<Project>();
  project->load("optimize.xlsx");
  OptimizeParameters params(project);
  params.set_up_optimize(&app);
  app.SetOutputDir("output");

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats;
  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  // print out eigenvalues (for debugging)
  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // If Procrustes scaling is working, this should be small.
  // Otherwise it is quite large (>4000).
  double value = values[values.size() - 1];
  ASSERT_LT(value, 100);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, mesh_geodesics_test) {
  const std::string sphere_mesh_path = std::string(TEST_DATA_DIR) + "/sphere_highres.ply";
  const auto sw_mesh = MeshUtils::threadSafeReadMesh(sphere_mesh_path);
  VtkMeshWrapper mesh(sw_mesh.getVTKMesh(), true, 1000000);

  auto polar2cart = [](double theta, double phi) {
    const double x = sin(theta) * cos(phi);
    const double y = sin(theta) * sin(phi);
    const double z = cos(theta);
    const itk::Point<double, 3> pt({x, y, z});
    return pt;
  };

  // sample a bunch of points (deterministically) on the sphere and check whether the returned
  // geodesic distance is close to the analytically computed value
  for(int i=0; i<100; i++) {
    for(int j=0; j<100; j++) {
      const double theta0 = M_2PI * (i % 10) / 10.0;
      const double phi0   = M_2PI * (i / 10) / 10.0;
      const double theta1 = M_2PI * (j % 10) / 10.0;
      const double phi1   = M_2PI * (j / 10) / 10.0;

      const auto pt_a = polar2cart(theta0, phi0);
      const auto pt_b = polar2cart(theta1, phi1);
      const double a_dot_b = std::max(std::min(dot_product(pt_a.GetVnlVector(), pt_b.GetVnlVector()), 1.0), -1.0);

      const double computed = mesh.ComputeDistance(pt_a, -1, pt_b, -1);
      const double truth = acos(a_dot_b);

      // std::cerr << "Geodesics test: " << computed << " " << truth << "\n";
      ASSERT_NEAR(computed, truth, 0.2);
    }
  }
}
