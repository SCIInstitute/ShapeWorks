#include "ParticleFile.h"

#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>

#include <fstream>

namespace shapeworks::particles {
//---------------------------------------------------------------------------
void write_vtk_particles(std::string filename, const Eigen::VectorXd& points) {
  auto vtk_points = vtkSmartPointer<vtkPoints>::New();
  int num_points = points.size() / 3;
  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double x = points[idx++];
    double y = points[idx++];
    double z = points[idx++];
    vtk_points->InsertNextPoint(x, y, z);
  }
  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(vtk_points);
  auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(polydata);
  writer->Write();
}

//---------------------------------------------------------------------------
Eigen::VectorXd read_vtk_particles(std::string filename) {
  auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  auto polydata = reader->GetOutput();
  auto vtk_points = polydata->GetPoints();
  int num_points = vtk_points->GetNumberOfPoints();
  Eigen::VectorXd points;
  points.setZero();
  points.resize(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }

  return points;
}

//---------------------------------------------------------------------------
Eigen::VectorXd read_particles(std::string filename) {
  if (filename.substr(filename.size() - 4) == ".vtk") {
    return read_vtk_particles(filename);
  }

  std::ifstream in(filename);
  if (!in.good()) {
    throw std::runtime_error("Unable to read file: " + filename);
  }
  Eigen::VectorXd points;

  auto vtk_points = vtkSmartPointer<vtkPoints>::New();
  int num_points = 0;
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) {
      break;
    }
    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  in.close();
  points.setZero();
  points.resize(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }
  return points;
}

//---------------------------------------------------------------------------
void write_particles(std::string filename, const Eigen::VectorXd& points) {
  if (filename.substr(filename.size() - 4) == ".vtk") {
    write_vtk_particles(filename, points);
    return;
  }
  std::ofstream out(filename.c_str());
  if (!out) {
    throw std::runtime_error("Unable to write file: " + filename);
  }

  // write out points, 3 per line
  int num_points = points.size() / 3;
  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    out << points[idx++] << " " << points[idx++] << " " << points[idx++] << "\n";
  }
  out.close();
}

}  // namespace shapeworks::particles
