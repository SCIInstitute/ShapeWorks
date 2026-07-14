
#include <Project/Project.h>

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
// #1985: columns that are blank for every subject (e.g. an orphan "world_particles_ghost"
// header left over from a copy/pasted spreadsheet) should be ignored, not turned into a
// phantom domain or a blank file read.
TEST(ProjectTests, ignore_blank_columns_test) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/project", "project_ignore_blank_columns_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("ignore_blank_columns.swproj");

  // the all-blank "ghost" domain should be dropped, leaving only the real domains
  auto domain_names = project->get_domain_names();
  ASSERT_EQ(domain_names.size(), 2);
  EXPECT_EQ(domain_names[0], "femur");
  EXPECT_EQ(domain_names[1], "pelvis");

  auto subjects = project->get_subjects();
  ASSERT_EQ(subjects.size(), 2);
  for (auto& subject : subjects) {
    EXPECT_EQ(subject->get_number_of_domains(), 2);

    // no blank filenames should be produced from the ghost columns
    EXPECT_EQ(subject->get_original_filenames().size(), 2);
    EXPECT_EQ(subject->get_groomed_filenames().size(), 2);
    EXPECT_EQ(subject->get_local_particle_filenames().size(), 2);
    ASSERT_EQ(subject->get_world_particle_filenames().size(), 2);
    for (const auto& filename : subject->get_world_particle_filenames()) {
      EXPECT_FALSE(filename.empty());
    }

    // the all-blank extra column should be ignored
    EXPECT_EQ(subject->get_extra_values().count("custom_blank"), 0u);
    // the populated group column is preserved
    EXPECT_EQ(subject->get_group_values().count("side"), 1u);
  }
}

//---------------------------------------------------------------------------
// #2455: after a failed/aborted optimization, the project should not be left referencing
// particle files that were never written. clear_particle_filenames() clears them for
// non-fixed subjects (fixed subjects keep their pre-existing input particles).
TEST(ProjectTests, clear_particle_filenames_test) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/project", "project_clear_particle_filenames_test");

  ProjectHandle project = std::make_shared<Project>();
  project->load("json_read.swproj");

  auto& subjects = project->get_subjects();
  ASSERT_GE(subjects.size(), 2);

  // mark the first subject fixed; its particles are pre-existing inputs and must be kept
  subjects[0]->set_fixed(true);
  ASSERT_FALSE(subjects[1]->get_world_particle_filenames().empty());

  project->clear_particle_filenames();

  // the fixed subject keeps its particle filenames
  EXPECT_FALSE(subjects[0]->get_local_particle_filenames().empty());
  EXPECT_FALSE(subjects[0]->get_world_particle_filenames().empty());

  // non-fixed subjects are cleared
  for (size_t i = 1; i < subjects.size(); i++) {
    EXPECT_TRUE(subjects[i]->get_local_particle_filenames().empty());
    EXPECT_TRUE(subjects[i]->get_world_particle_filenames().empty());
  }

  // and a saved+reloaded project no longer references the missing particle files
  project->save("cleared.swproj");
  ProjectHandle reloaded = std::make_shared<Project>();
  reloaded->load("cleared.swproj");
  auto reloaded_subjects = reloaded->get_subjects();
  ASSERT_EQ(reloaded_subjects.size(), subjects.size());
  for (size_t i = 1; i < reloaded_subjects.size(); i++) {
    EXPECT_TRUE(reloaded_subjects[i]->get_local_particle_filenames().empty());
    EXPECT_TRUE(reloaded_subjects[i]->get_world_particle_filenames().empty());
  }
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
