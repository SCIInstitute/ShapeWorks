//#include <cstdio>

#include "Testing.h"

#include <Groom/Groom.h>
#include <Groom/GroomParameters.h>
#include <Mesh/MeshUtils.h>
#include <Project/Project.h>
#include <StringUtils.h>
#include <vtkCellArray.h>
#include <vtkClipClosedSurface.h>
#include <vtkDoubleArray.h>
#include <vtkFeatureEdges.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkTriangleFilter.h>

#include <boost/filesystem.hpp>
#include <fstream>

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

//---------------------------------------------------------------------------
namespace {
struct TestSubject {
  std::string name;
  std::string file;  // relative to the project directory
  bool excluded = false;
};

// Write a single-domain mesh project into a fresh test output directory and load it, the same way a
// user's .swproj is read. Grooming writes its output under that directory.
ProjectHandle make_mesh_project(const std::string& test_name, const std::vector<TestSubject>& subjects,
                                const std::map<std::string, std::string>& groom_params) {
  auto dir = TestUtils::Instance().get_output_dir(test_name);
  boost::filesystem::create_directories(dir);
  boost::filesystem::current_path(dir);
  std::ofstream out("project.swproj");
  out << "{\n  \"data\": [\n";
  for (size_t i = 0; i < subjects.size(); i++) {
    out << "    {\"name\": \"" << subjects[i].name << "\", \"shape_1\": \"" << subjects[i].file
        << "\", \"excluded\": \"" << (subjects[i].excluded ? "true" : "false") << "\"}"
        << (i + 1 < subjects.size() ? "," : "") << "\n";
  }
  out << "  ],\n  \"groom\": {\"1\": {";
  bool first = true;
  for (const auto& [key, value] : groom_params) {
    out << (first ? "" : ", ") << "\"" << key << "\": \"" << value << "\"";
    first = false;
  }
  out << "}}\n}\n";
  out.close();
  ProjectHandle project = std::make_shared<Project>();
  EXPECT_TRUE(project->load("project.swproj"));
  return project;
}

std::map<std::string, std::string> remesh_params(const std::string& percent) {
  // no alignment, so each subject is groomed independently; remesh in percent mode
  return {{"alignment_enabled", "0"}, {"center", "0"},          {"icp", "0"},
          {"remesh", "1"},            {"remesh_percent_mode", "1"}, {"remesh_percent", percent},
          {"mesh_smooth", "0"},       {"fill_mesh_holes", "0"}};
}

void copy_test_file(const std::string& name, const std::string& target) {
  boost::filesystem::copy_file(std::string(TEST_DATA_DIR) + "/" + name, target,
                               boost::filesystem::copy_options::overwrite_existing);
}

int count_edges(Mesh& mesh, bool boundary) {
  auto features = vtkSmartPointer<vtkFeatureEdges>::New();
  features->SetInputData(mesh.getVTKMesh());
  features->SetBoundaryEdges(boundary);
  features->SetNonManifoldEdges(!boundary);
  features->FeatureEdgesOff();
  features->ManifoldEdgesOff();
  features->Update();
  return features->GetOutput()->GetNumberOfCells();
}

// ellipsoid_01.vtk clipped by a capped plane just above one of its vertices: the cap is a fan of large
// triangles and the cut leaves thin slivers along the rim, as a cropped bone mesh does
vtkSmartPointer<vtkPolyData> clipped_ellipsoid(double offset) {
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_01.vtk");
  auto poly_data = ellipsoid.getVTKMesh();
  double z_cut = 0.0, best = 1e30;
  for (vtkIdType i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    double z = poly_data->GetPoint(i)[2];
    if (std::abs(z - 3.0) < best) {
      best = std::abs(z - 3.0);
      z_cut = z;
    }
  }
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetOrigin(0, 0, z_cut + offset);
  plane->SetNormal(0, 0, -1);
  auto planes = vtkSmartPointer<vtkPlaneCollection>::New();
  planes->AddItem(plane);
  auto clip = vtkSmartPointer<vtkClipClosedSurface>::New();
  clip->SetInputData(poly_data);
  clip->SetClippingPlanes(planes);
  clip->GenerateFacesOn();
  auto triangles = vtkSmartPointer<vtkTriangleFilter>::New();
  triangles->SetInputConnection(clip->GetOutputPort());
  triangles->Update();
  return triangles->GetOutput();
}

Mesh groomed_mesh(ProjectHandle project, int subject) {
  auto filenames = project->get_subjects()[subject]->get_groomed_filenames();
  EXPECT_EQ(filenames.size(), 1);
  return MeshUtils::threadSafeReadMesh(filenames[0]);
}
}  // namespace

//---------------------------------------------------------------------------
TEST(GroomTests, open_mesh_remesh_test) {
  // An open mesh must still be remeshed without ForceManifold, which piles vertices along the open
  // boundary (#2672). femur.vtk is open.
  auto project = make_mesh_project("groom_open_mesh_remesh", {{"femur", "femur.vtk"}}, remesh_params("40"));
  copy_test_file("femur.vtk", "femur.vtk");
  Mesh input("femur.vtk");
  auto num_points = input.numPoints();

  Groom app(project);
  ASSERT_TRUE(app.run());

  Mesh groomed = groomed_mesh(project, 0);
  ASSERT_NEAR(groomed.numPoints(), num_points * 0.40, num_points * 0.02);
  ASSERT_GT(count_edges(groomed, true), 0);   // still open
  ASSERT_EQ(count_edges(groomed, false), 0);  // no non-manifold edges
}

//---------------------------------------------------------------------------
TEST(GroomTests, open_mesh_fill_holes_remesh_test) {
  // An open mesh closed by fill holes is remeshed as a closed surface (with ForceManifold), even
  // though it was open when grooming started.
  auto params = remesh_params("40");
  params["fill_mesh_holes"] = "1";
  auto project = make_mesh_project("groom_open_mesh_fill_holes", {{"femur", "femur.vtk"}}, params);
  copy_test_file("femur.vtk", "femur.vtk");

  Groom app(project);
  ASSERT_TRUE(app.run());

  Mesh groomed = groomed_mesh(project, 0);
  ASSERT_EQ(count_edges(groomed, true), 0);
  ASSERT_EQ(count_edges(groomed, false), 0);
}

//---------------------------------------------------------------------------
TEST(GroomTests, closed_mesh_small_triangle_remesh_test) {
  // ellipsoid_0.ply is closed but contains a tiny, well-shaped triangle that the old zero-area test
  // (relative to the largest triangle) removed, opening the mesh before remeshing (#2690).
  auto project =
      make_mesh_project("groom_closed_small_triangle", {{"ellipsoid", "ellipsoid_0.ply"}}, remesh_params("50"));
  copy_test_file("ellipsoid_0.ply", "ellipsoid_0.ply");

  Groom app(project);
  ASSERT_TRUE(app.run());

  Mesh groomed = groomed_mesh(project, 0);
  ASSERT_EQ(count_edges(groomed, true), 0);
  ASSERT_EQ(count_edges(groomed, false), 0);
}

//---------------------------------------------------------------------------
TEST(GroomTests, closed_mesh_opened_by_repair_test) {
  // A closed mesh whose repair removes truly degenerate rim slivers (opening it) must still be
  // remeshed as a closed surface: whether to force manifold output is decided from the mesh before
  // repair (#2690).
  auto clipped = clipped_ellipsoid(1e-6);
  ASSERT_TRUE(MeshUtils::is_closed(clipped));
  ASSERT_FALSE(MeshUtils::is_closed(MeshUtils::repair_mesh(clipped)));

  auto project = make_mesh_project("groom_closed_opened_by_repair", {{"clipped", "clipped.vtk"}}, remesh_params("80"));
  Mesh(clipped).write("clipped.vtk");

  Groom app(project);
  ASSERT_TRUE(app.run());

  Mesh groomed = groomed_mesh(project, 0);
  ASSERT_EQ(count_edges(groomed, true), 0);
  ASSERT_EQ(count_edges(groomed, false), 0);
}

//---------------------------------------------------------------------------
TEST(GroomTests, failed_subject_test) {
  // One failing subject is reported once grooming finishes, and does not take the others down with
  // it (an exception escaping the parallel loop cancelled the work for every other subject, #2690).
  std::vector<TestSubject> subjects = {{"a", "a.vtk"}, {"b", "b.vtk"}, {"missing", "missing.vtk"}, {"c", "c.vtk"}};
  auto project = make_mesh_project("groom_failed_subject", subjects, remesh_params("50"));
  for (auto name : {"a.vtk", "b.vtk", "c.vtk"}) {
    copy_test_file("ellipsoid_01.vtk", name);
  }

  Groom app(project);
  try {
    app.run();
    FAIL() << "expected grooming to fail for the missing subject";
  } catch (const std::runtime_error& e) {
    std::string message = e.what();
    ASSERT_NE(message.find("1 subject"), std::string::npos) << message;
    ASSERT_NE(message.find("missing"), std::string::npos) << message;
  }

  for (int i : {0, 1, 3}) {
    auto filenames = project->get_subjects()[i]->get_groomed_filenames();
    ASSERT_EQ(filenames.size(), 1);
    ASSERT_TRUE(boost::filesystem::exists(filenames[0]));
  }
}

//---------------------------------------------------------------------------
TEST(GroomTests, excluded_subject_test) {
  // Only the excluded subject is skipped; the others in the same range are still groomed.
  std::vector<TestSubject> subjects;
  for (int i = 0; i < 8; i++) {
    std::string name = "s" + std::to_string(i);
    subjects.push_back({name, name + ".vtk", i == 2 || i == 5});
  }
  auto project = make_mesh_project("groom_excluded_subject", subjects, remesh_params("50"));
  for (const auto& subject : subjects) {
    copy_test_file("ellipsoid_01.vtk", subject.file);
  }

  Groom app(project);
  ASSERT_TRUE(app.run());

  for (int i = 0; i < 8; i++) {
    auto filenames = project->get_subjects()[i]->get_groomed_filenames();
    if (i == 2 || i == 5) {
      ASSERT_TRUE(filenames.empty()) << "subject " << i;
    } else {
      ASSERT_EQ(filenames.size(), 1) << "subject " << i;
      ASSERT_TRUE(boost::filesystem::exists(filenames[0])) << "subject " << i;
    }
  }
}
