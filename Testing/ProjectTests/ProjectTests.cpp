
#include <Libs/Project/Project.h>

#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
static void compare_projects(ProjectHandle p1, ProjectHandle p2) {
  ASSERT_EQ(p1->get_subjects().size(), p2->get_subjects().size());
  for (int i = 0; i < p1->get_subjects().size(); i++) {
    auto s1 = p1->get_subjects()[i];
    auto s2 = p2->get_subjects()[i];
    EXPECT_EQ(s1->get_original_filenames(), s2->get_original_filenames());
    EXPECT_EQ(s1->get_groomed_filenames(), s2->get_groomed_filenames());
    EXPECT_EQ(s1->get_landmarks_filenames(), s2->get_landmarks_filenames());
    EXPECT_EQ(s1->get_local_particle_filenames(), s2->get_local_particle_filenames());
    EXPECT_EQ(s1->get_world_particle_filenames(), s2->get_world_particle_filenames());
    EXPECT_EQ(s1->get_constraints_filenames(), s2->get_constraints_filenames());
    EXPECT_EQ(s1->get_number_of_domains(), s2->get_number_of_domains());
    EXPECT_EQ(s1->get_feature_filenames(), s2->get_feature_filenames());

    EXPECT_EQ(s1->get_groomed_transforms(), s2->get_groomed_transforms());
    EXPECT_EQ(s1->get_procrustes_transforms(), s2->get_procrustes_transforms());
    EXPECT_EQ(s1->get_group_values(), s2->get_group_values());

    EXPECT_EQ(s1->get_extra_values(), s2->get_extra_values());
    EXPECT_EQ(s1->get_table_values(), s2->get_table_values());
    EXPECT_EQ(s1->get_display_name(), s2->get_display_name());
  }

  for (auto params : {Parameters::OPTIMIZE_PARAMS, Parameters::GROOM_PARAMS, Parameters::ANALYSIS_PARAMS,
                      Parameters::DEEPSSM_PARAMS, Parameters::PROJECT_PARAMS, Parameters::STUDIO_PARAMS}) {
    EXPECT_EQ(p1->get_parameters(params).get_map(), p2->get_parameters(params).get_map());
  }
}

//---------------------------------------------------------------------------
TEST(ProjectTests, conversion_to_json_test) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/project", "project_convert_to_json_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("excel_read.xlsx");
  project->save("json_write.swproj");

  ProjectHandle project_compare1 = std::make_shared<Project>();
  project_compare1->load("excel_read.xlsx");
  ProjectHandle project_compare2 = std::make_shared<Project>();
  project_compare2->load("json_write.swproj");

  compare_projects(project_compare1, project_compare2);
}

//---------------------------------------------------------------------------
TEST(ProjectTests, conversion_to_excel_test) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/project", "project_convert_to_excel_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("json_read.swproj");
  project->save("excel_write.xlsx");

  ProjectHandle project_compare1 = std::make_shared<Project>();
  project_compare1->load("json_read.swproj");
  ProjectHandle project_compare2 = std::make_shared<Project>();
  project_compare2->load("excel_write.xlsx");

  compare_projects(project_compare1, project_compare2);
}
