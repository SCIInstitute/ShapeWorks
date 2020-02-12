#include "stdafx.h"
#include "FEVTKExport.h"
#include "FEMesh.h"

#include <vtkPolyDataReader.h>
#include <fstream>
#include <sstream>

FEVTKExport::FEVTKExport(void)
{
  m_ops.bscalar_data = false;
  m_ops.bshellthick = false;
}

bool FEVTKExport::Export(FEMesh& mesh, const char* szfile)
{
  std::ofstream out(szfile, std::ofstream::out);

  bool value = this->ExportToStream(mesh, out);
  out.close();

  return value;
}

std::string FEVTKExport::ExportToString(FEMesh &mesh)
{
  std::ostringstream oss;
  this->ExportToStream(mesh, oss);
  return oss.str();
}

bool FEVTKExport::ExportToStream(FEMesh &mesh, std::ostream &out)
{
  int j, n;

  // count number of nodes
  int nodes = mesh.Nodes();

  // --- H E A D E R ---
  out << "# vtk DataFile Version 3.0\n";
  out << "vtk output\n";
  out << "ASCII\n";
  out << "DATASET POLYDATA\n";
  out << "POINTS " << nodes << " float\n";

  // --- N O D E S ---
  FEMesh& m = mesh;
  for (j = 0; j < m.Nodes(); ++j) {
    FENode& n = m.Node(j);
    out << n.r.x << " " << n.r.y << " " << n.r.z << "\n";
  }

  // --- F A C E S ---
  int faces = m.Faces();
  for (j = 0; j < faces; ++j) {
    FEFace& f = m.Face(j);
    n = f.Nodes();
    if (j == 0) {
      out << "POLYGONS " << faces << " " << faces * (n + 1) << "\n";
    }
    if (n == 3) {
      out << n << " " << f.n[0] << " " << f.n[1] << " " << f.n[2] << "\n";
    }
    else if (n == 4) {
      out << n << " " << f.n[0] << " " << f.n[1] << " " << f.n[2] << " " << f.n[3] << "\n";
    }
    else {
      assert(false);
    }
  }

  //----Shell Thickness ----
  if (m_ops.bshellthick) {
    out << "\n";
    out << "POINT_DATA " << nodes << "\n";
    out << "SCALARS ShellThickness float\n";
    out << "LOOKUP_TABLE default\n";

    FEMesh& m = mesh;

    vector<double> nodeShellThickness;
    nodeShellThickness.reserve(nodes);
    int nn[8];
    for (int k = 0; k < nodes; k++) {
      nodeShellThickness.push_back(0);
    }

    for (j = 0; j < m.Elements(); ++j) {
      FEElement& el = m.Element(j);
      if (!el.IsType(FE_TRI3) && !el.IsType(FE_QUAD4)) {
        break;
      }

      for (int k = 0; k < el.Nodes(); ++k) {
        nn[k] = el.m_node[k];
      }

      double* h = el.m_h;

      for (int k = 0; k < el.Nodes(); k++) {
        nodeShellThickness[nn[k]] = h[k];
      }
    }

    for (j = 0; j < m.Nodes();) {
      for (int k = 0; k < 9 && j + k < m.Nodes(); k++) {
        char buffer[1024];
        sprintf(buffer, "%15.10lg ", nodeShellThickness[j + k]);
        out << buffer << "\n";
      }
      out << "\n";
      j = j + 9;
    }
  }

  //-----Nodal Data-----------
  if (m_ops.bscalar_data) {
    out << "\n";
    out << "POINT_DATA " << nodes << "\n";
    out << "SCALARS ScalarData float\n";
    out << "LOOKUP_TABLE default\n";

    for (j = 0; j < m.Nodes();) {
      for (int k = 0; k < 9 && j + k < m.Nodes(); k++) {
        FENode& n = m.Node(j + k);
        char buffer[1024];
        sprintf(buffer, "%15.10lg ", n.m_ndata);
        out << buffer << "\n";
      }
      out << "\n";
      j = j + 9;
    }
  }

  return true;
}

vtkSmartPointer<vtkPolyData> FEVTKExport::ExportToVTK(FEMesh &mesh)
{
  std::string mesh_string = this->ExportToString(mesh);

  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetInputString(mesh_string);
  reader->SetReadFromInputString(true);
  reader->Update();
  vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
  vtk_mesh->DeepCopy(reader->GetOutput());
  return vtk_mesh;
}
