//#include <cstdio>

#include "Testing.h"

#include <Libs/Groom/Groom.h>
#include <Libs/Project/Project.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(GroomTests, basic_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sphere");
  chdir(test_location.c_str());

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("sphere10_DT.nrrd");

  ProjectHandle project = std::make_shared<Project>();
  project->load("groom.xlsx");
  Groom app(project);
  bool success = app.run();
  ASSERT_TRUE(success);

  Image image("sphere10_DT.nrrd");
  Image ground_truth("sphere10_DT_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);

}
