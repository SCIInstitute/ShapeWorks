/*
 * Shapeworks license
 */

/**
 * @file MeshGenerator.h
 * @brief Mesh generation
 *
 * The MeshGenerator performs the actual work of reconstructing
 * a mesh from a shape (list of points).
 */

#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include "vnl/vnl_vector.h"
#include <Reconstruction.h>
#include <Data/Preferences.h>

class MeshGenerator
{
  public:
    MeshGenerator(Preferences& prefs_, Reconstruction& construct);
    ~MeshGenerator();
    vtkSmartPointer<vtkPolyData> buildMesh(const vnl_vector<double>& shape);

  private:
    Preferences& prefs_;
    Reconstruction& construct_;
};

#endif // ifndef MESH_GENERATOR_H
