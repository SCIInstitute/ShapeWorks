#include <gtest/gtest.h>

#include "TestConfiguration.h"

#include "ShapeWorksRunApp.h"
#include "itkParticleShapeStatistics.h"

TEST(OptimizeTests, sample_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sphere");
  chdir(test_location.c_str());
  std::string paramfile = std::string("sphere.xml");
  ShapeWorksRunApp<> app(paramfile.c_str());
  app.Run();

  ParticleShapeStatistics<3> stats;

  stats.ReadPointFiles("analyze.xml");
  stats.ComputeModes();
  stats.PrincipalComponentProjections();

  auto values = stats.Eigenvalues();
  for (int i = 0; i < values.size(); i++) {
    std::cerr << "Eigenvalue " << i << " : " << values[i] << "\n";
  }

  // check the first mode of variation.
  // If Procrustes scaling is working, this should be tiny.
  // Otherwise it is quite large (>8000).
  double value = values[values.size() - 1];
  ASSERT_LT(value, 1);
}
