#include <Mesh/MeshUtils.h>
#include <Optimize/OptimizeParameters.h>
#include <Project/Project.h>
#include <itkApproximateSignedDistanceMapImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <cstdio>

#include "Libs/Optimize/Domain/MeshWrapper.h"
#include "Optimize.h"
#include "OptimizeParameterFile.h"
#include "ParticleShapeStatistics.h"
#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
static void prep_temp(std::string data, std::string name) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/optimize/shared", "shared");
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + data, name);
}

//---------------------------------------------------------------------------
static bool check_constraint_violations(Optimize &app, double slack) {
  // Check that points don't violate the constraints
  size_t domains_per_shape = app.GetSampler()->GetParticleSystem()->GetDomainsPerShape();
  size_t num_doms = app.GetSampler()->GetParticleSystem()->GetNumberOfDomains();

  std::vector<std::vector<itk::FixedArray<double, 3> > > lists;

  for (size_t domain = 0; domain < num_doms; domain++) {
    std::vector<itk::FixedArray<double, 3> > list;
    for (auto k = 0; k < app.GetSampler()->GetParticleSystem()->GetPositions(domain)->GetSize(); k++) {
      list.push_back(app.GetSampler()->GetParticleSystem()->GetPositions(domain)->Get(k));
    }
    lists.push_back(list);
  }

  bool good = true;
  std::vector<std::string> types;
  types.push_back("plane");
  types.push_back("free form");
  for (size_t domain = 0; domain < num_doms; domain++) {
    for (size_t i = 0; i < lists[domain].size(); i++) {
      itk::FixedArray<double, 3> p = lists[domain][i];

      auto violation_report_data =
          app.GetSampler()->GetParticleSystem()->GetDomain(domain)->GetConstraints()->violationReportData(p);

      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < violation_report_data[j].size(); k++) {
          if (violation_report_data[j][k] > slack) {
            std::cout << "VIOLATION: Shape# " << int(domain / domains_per_shape) << " domain# "
                      << domain % domains_per_shape << " point# " << i << " " << types[j] << " constraint " << k
                      << " of magnitude " << violation_report_data[j][k] << " by point " << p << std::endl;
          }
          // else std::cout << "Good point: Shape# " << int(domain/domains_per_shape) << " domain# "
          // << domain%domains_per_shape  << " point# " << i << " " << types[j] << " constraint "
          // << k << " with evaluation " << violation_report_data[j][k] << " by point "
          // << p << std::endl;
          if (violation_report_data[j][k] > slack) {
            good = false;
          }
        }
      }
    }
  }
  return good;
};

//---------------------------------------------------------------------------
TEST(OptimizeTests, sample) {
  prep_temp("/optimize/sphere", "sample");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("optimize_particles/sphere10_DT_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
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
  prep_temp("/optimize/hemisphere", "open_mesh_test");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("optimize_particles/hemisphere00_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
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
TEST(OptimizeTests, fixed_domain) {
  prep_temp("/optimize/fixed_domain", "fixed_domain");

  // make sure we clean out the output file of interest
  std::remove("optimize_particles/sphere40_DT_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
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
  prep_temp("/optimize/fixed_mesh_domain", "fixed_mesh_domain");

  // make sure we clean out the output file of interest
  std::remove("optimize_particles/id0002_ss3_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  double value = values[values.size() - 1];
  ASSERT_GT(value, 250);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, use_normals_test) {
  prep_temp("/optimize/use_normals", "use_normals");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_DT_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
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
  prep_temp("/optimize/mesh_use_normals", "mesh_use_normals");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere_00_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  ASSERT_GT(values[values.size() - 1], 750.0);
  ASSERT_LT(values[values.size() - 2], 10);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, embedded_python_test) {
  prep_temp("/simple", "embedded_python");

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
  prep_temp("/optimize/sphere", "project");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("optimize_particles/sphere10_DT_world.particles");

  Optimize app;

  // set up optimizer from project
  ProjectHandle project = std::make_shared<Project>();
  project->load("optimize.xlsx");
  OptimizeParameters params(project);
  params.set_up_optimize(&app);

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
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
TEST(OptimizeTests, contour_domain_test) {
  prep_temp("/optimize/supershapes_2d", "contour_domain");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/ss_0_groomed_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // Check the modes of variation.  The first mode should contain almost all the variation and the 2nd
  // and higher modes should contain very little
  ASSERT_GT(values[values.size() - 1], 2000.0);
  ASSERT_LT(values[values.size() - 2], 1.0);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, procrustes_disabled_test) {
  prep_temp("/optimize/procrustes", "procrustes_disabled_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("procrustes.xlsx");
  OptimizeParameters params(project);
  params.set_use_procrustes(false);

  Optimize app;
  params.set_up_optimize(&app);

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats(project);
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }
  ASSERT_GT(values[values.size() - 1], 700.0);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, procrustes_no_scale_test) {
  prep_temp("/optimize/procrustes", "procrustes_no_scale_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("procrustes.xlsx");
  OptimizeParameters params(project);
  params.set_use_procrustes(true);
  params.set_use_procrustes_rotation_translation(true);
  params.set_use_procrustes_scaling(false);

  Optimize app;
  params.set_up_optimize(&app);

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats(project);
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }
  ASSERT_GT(values[values.size() - 1], 150.0);
  ASSERT_LT(values[values.size() - 1], 200.0);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, procrustes_both_enabled_test) {
  prep_temp("/optimize/procrustes", "procrustes_both_enabled_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("procrustes.xlsx");
  OptimizeParameters params(project);
  params.set_use_procrustes(true);
  params.set_use_procrustes_rotation_translation(true);
  params.set_use_procrustes_scaling(true);

  Optimize app;
  params.set_up_optimize(&app);

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats(project);
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }
  // should be tiny with all of procrustes enabled
  ASSERT_LT(values[values.size() - 1], 1.0);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, procrustes_scale_only_test) {
  prep_temp("/optimize/procrustes", "procrustes_scale_only_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("procrustes.xlsx");
  OptimizeParameters params(project);
  params.set_use_procrustes(true);
  params.set_use_procrustes_rotation_translation(false);
  params.set_use_procrustes_scaling(true);

  Optimize app;
  params.set_up_optimize(&app);

  // run optimize
  bool success = app.Run();
  ASSERT_TRUE(success);

  // compute stats
  ParticleShapeStatistics stats(project);
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }
  ASSERT_GT(values[values.size() - 1], 275.0);
  ASSERT_LT(values[values.size() - 1], 345.0);
}

// TODO Move this to mesh tests?
//---------------------------------------------------------------------------
TEST(OptimizeTests, mesh_geodesics_test) {
  const std::string sphere_mesh_path = std::string(TEST_DATA_DIR) + "/sphere_highres.ply";
  const auto sw_mesh = MeshUtils::threadSafeReadMesh(sphere_mesh_path);
  MeshWrapper mesh(sw_mesh.getVTKMesh(), true, 1000000);

  auto polar2cart = [](double theta, double phi) {
    const double x = sin(theta) * cos(phi);
    const double y = sin(theta) * sin(phi);
    const double z = cos(theta);
    const itk::Point<double, 3> pt({x, y, z});
    return pt;
  };

  // sample a bunch of points (deterministically) on the sphere and check whether the returned
  // geodesic distance is close to the analytically computed value
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      const double theta0 = M_2PI * (i % 10) / 10.0;
      const double phi0 = M_2PI * (i / 10) / 10.0;
      const double theta1 = M_2PI * (j % 10) / 10.0;
      const double phi1 = M_2PI * (j / 10) / 10.0;

      const auto pt_a = polar2cart(theta0, phi0);
      const auto pt_b = polar2cart(theta1, phi1);
      const double a_dot_b = std::max(std::min(dot_product(pt_a.GetVnlVector(), pt_b.GetVnlVector()), 1.0), -1.0);

      const double computed = mesh.ComputeDistance(pt_a, -1, pt_b, -1);
      const double truth = acos(a_dot_b);

      // std::cerr << "Geodesics test: " << computed << " " << truth << "\n";
      ASSERT_NEAR(computed, truth, 0.06);
    }
  }
}

// Constraint tests
//---------------------------------------------------------------------------
TEST(OptimizeTests, cutting_plane_test) {
  prep_temp("/optimize/cutting_plane_multi", "cutting_plane_test");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_DT_world.particles");
  
  auto start = shapeworks::ShapeWorksUtils::now();

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  bool good = check_constraint_violations(app, 1.5e-1);
  
  auto end = shapeworks::ShapeWorksUtils::now();
  std::cout << "Time taken to run cutting_plane optimize test: "
            << shapeworks::ShapeWorksUtils::elapsed(start, end, false) << "sec \n";

  ASSERT_TRUE(good);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, ffc_test) {
  prep_temp("/optimize/ffc", "ffc_test");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_DT_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.xlsx"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  bool good = check_constraint_violations(app, 6.0e-1);

  ASSERT_TRUE(good);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, multi_domain_constraint) {
  prep_temp("/optimize/multidomain_constraints", "multi_domain_constraint");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_DT_world.particles");
  std::remove("optimize_particles/sphere10_DT_50_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  bool good = check_constraint_violations(app, 7.5e-1);

  ASSERT_TRUE(good);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, mesh_ffc_test) {
  prep_temp("/optimize/mesh_constraints", "mesh_ffc_test");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_world.particles");
  std::remove("optimize_particles/sphere20_world.particles");
  std::remove("optimize_particles/sphere30_world.particles");
  std::remove("optimize_particles/sphere40_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  bool good = check_constraint_violations(app, 4.0e-1);
  ASSERT_TRUE(good);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, mesh_ffc_test_aug_lag) {
  prep_temp("/optimize/mesh_constraints_aug_lag", "mesh_ffc_test_aug_lag");

  // make sure we clean out at least one output file
  std::remove("optimize_particles/sphere10_world.particles");
  std::remove("optimize_particles/sphere20_world.particles");
  std::remove("optimize_particles/sphere30_world.particles");
  std::remove("optimize_particles/sphere40_world.particles");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  bool good = check_constraint_violations(app, 20.0e-1);
  ASSERT_TRUE(good);
}

//---------------------------------------------------------------------------
TEST(OptimizeTests, vtk_output) {
  prep_temp("/optimize/sphere", "vtk_output");

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("vtk_output_particles/sphere10_DT_world.vtk");

  // run with parameter file
  Optimize app;
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("vtk_output.swproj"));
  OptimizeParameters params(project);
  ASSERT_TRUE(params.set_up_optimize(&app));
  app.Run();

  // compute stats
  ParticleShapeStatistics stats;
  stats.read_point_files("analyze_vtk_output.xml");
  stats.compute_modes();
  stats.principal_component_projections();

  // print out eigenvalues (for debugging)
  auto values = stats.get_eigen_values();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // If Procrustes scaling is working, this should be small.
  // Otherwise it is quite large (>4000).
  double value = values[values.size() - 1];
  ASSERT_LT(value, 100);
}
