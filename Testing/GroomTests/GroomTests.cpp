//#include <cstdio>

#include "Testing.h"

#include <Groom/Groom.h>
#include <Groom/GroomParameters.h>
#include <Mesh/MeshUtils.h>
#include <Project/Project.h>
#include <StringUtils.h>
#include <vtkDoubleArray.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(GroomTests, basic_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/optimize/sphere");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
  chdir(test_location.c_str());
#pragma GCC diagnostic pop

  // make sure we clean out at least one necessary file to make sure we re-run
  std::remove("../shared/sphere/sphere10_DT.nrrd");

  ProjectHandle project = std::make_shared<Project>();
  project->load("groom.xlsx");
  Groom app(project);
  bool success = app.run();
  ASSERT_TRUE(success);

  Image image("groomed/sphere10_DT.nrrd");
  Image ground_truth("../shared/spheres/sphere10_DT_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);

}

//---------------------------------------------------------------------------
TEST(GroomTests, mesh_scalars_survive_grooming_test) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/optimize/mesh_use_normals", "groom_mesh_scalars");

  // meshes carrying a scalar field, which grooming has to hand to the optimizer intact
  ProjectHandle project = std::make_shared<Project>();
  ASSERT_TRUE(project->load("optimize.swproj"));

  for (auto& subject : project->get_subjects()) {
    std::vector<std::string> filenames;
    for (const auto& filename : subject->get_original_filenames()) {
      Mesh mesh = MeshUtils::threadSafeReadMesh(filename);

      auto height = vtkSmartPointer<vtkDoubleArray>::New();
      height->SetName("height");
      height->SetNumberOfValues(mesh.numPoints());
      for (int i = 0; i < mesh.numPoints(); i++) {
        height->SetValue(i, mesh.getPoint(i)[2]);
      }
      mesh.setField("height", height, Mesh::Point);

      // .ply cannot carry fields, so keep the copy with the field in a format that can
      auto name = StringUtils::getBaseFilenameWithoutExtension(filename) + "_scalars.vtk";
      mesh.write(name);
      filenames.push_back(name);
    }
    subject->set_original_filenames(filenames);
    subject->set_groomed_filenames({});
  }
  project->update_subjects();

  // the steps that rebuild the surface: repair runs regardless, then these
  GroomParameters params(project);
  params.set_fill_mesh_holes_tool(true);
  params.set_remesh(true);
  params.set_remesh_percent_mode(true);
  params.set_remesh_percent(50);
  params.set_mesh_smooth(true);
  params.save_to_project();

  Groom app(project);
  ASSERT_TRUE(app.run());

  for (auto& subject : project->get_subjects()) {
    for (const auto& filename : subject->get_groomed_filenames()) {
      Mesh groomed = MeshUtils::threadSafeReadMesh(filename);
      auto names = groomed.getFieldNames();
      ASSERT_NE(std::find(names.begin(), names.end(), "height"), names.end());
      ASSERT_NO_THROW(groomed.getFieldValue("height", groomed.numPoints() - 1));
    }
  }
}
