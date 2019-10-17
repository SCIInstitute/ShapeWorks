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

#include <QSharedPointer>

#include "vnl/vnl_vector.h"

#include <Data/Preferences.h>

#include <Data/SurfaceReconstructor.h>

class MeshGenerator
{
public:
  MeshGenerator(Preferences& prefs_);
  ~MeshGenerator();
  vtkSmartPointer<vtkPolyData> buildMesh(const vnl_vector<double>& shape);

  void set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor);

private:
  Preferences& prefs_;
  QSharedPointer<SurfaceReconstructor> surface_reconstructor_;
};

#endif // ifndef MESH_GENERATOR_H
