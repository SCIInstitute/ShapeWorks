#include <igl/readPLY.h>
#include <igl/boundary_loop.h>
#include <vtkXMLPolyDataWriter.h>
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPolyData.h>

/*
 * boundary_loop_extractor
 * boundary_loop_extractor <in_file.ply> <out_file.vtp>
 *
 * Given a .ply mesh, extract the boundary loop and export the boundary loop as a VTK .vtp file
 */

bool is_clockwise(const Eigen::MatrixXd& V,
                  const Eigen::MatrixXi& F,
                  const std::vector<int>& loop) {
  Eigen::RowVector3d centroid{0.0, 0.0, 0.0};
  for(const auto& i : loop) {
    centroid += V.row(i);
  }
  centroid /= loop.size();

  // todo this is arbitrary and works for the peanut data and initial tests on LA+Septum data
  // it enforces a consistent ordering in the boundary loop
  const auto v0 = V.row(loop[0]) - centroid;
  const auto v1 = V.row(loop[1]) - centroid;
  const double angle0 = atan2(v0.z(), v0.y());
  const double angle1 = atan2(v1.z(), v1.y());
  return angle0 > angle1;
}

int main(int argc, char *argv[]) {

  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << "<in_file.ply> <out_file.vtp>";
    exit(1);
  }

  const std::string in_fname = argv[1];
  const std::string out_fname = argv[2];

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  igl::readPLY(in_fname, V, F);

  std::vector<std::vector<int>> loops;
  igl::boundary_loop(F, loops);
  assert(loops.size() == 1);

  const auto& loop = loops[0];
  const auto is_cw = is_clockwise(V, F, loop);

  auto pts = vtkSmartPointer<vtkPoints>::New();
  for(const auto& i : loop) {
    pts->InsertNextPoint(V(i, 0), V(i, 1), V(i, 2));
  }

  auto lines = vtkSmartPointer<vtkCellArray>::New();
  for(size_t i=0; i<loop.size(); i++) {
    auto line = vtkSmartPointer<vtkLine>::New();
    if(is_cw) {
      line->GetPointIds()->SetId(0, i);
      line->GetPointIds()->SetId(1, (i+1)%loop.size());
    } else {
      line->GetPointIds()->SetId(1, i);
      line->GetPointIds()->SetId(0, (i+1)%loop.size());
    }

    lines->InsertNextCell(line);
  }

  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(pts);
  polydata->SetLines(lines);

  auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(out_fname.c_str());
  writer->SetInputData(polydata);
  writer->Write();
}
