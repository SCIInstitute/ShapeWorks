#pragma once
/**
 * @file MeshGenerator.h
 * @brief Mesh generation
 *
 * The MeshGenerator performs the actual work of reconstructing
 * a mesh from a shape (list of points).
 */

#include <QSharedPointer>

#include "vnl/vnl_vector.h"

#include <Data/Mesh.h>
#include <Data/Preferences.h>
#include <Data/SurfaceReconstructor.h>
#include <Data/LegacyMeshGenerator.h>
#include <Data/MeshWorkQueue.h>

#include <Groom/ShapeWorksGroom.h>

class MeshGenerator
{
public:

  MeshGenerator(Preferences& prefs);

  ~MeshGenerator();

  MeshHandle build_mesh(const MeshWorkItem &item);

  MeshHandle build_mesh_from_points(const vnl_vector<double>& shape, int domain);

  MeshHandle build_mesh_from_image(ImageType::Pointer image, float iso_value = 0.0);

  MeshHandle build_mesh_from_file(string filename, float iso_value = 0.0);

  void set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor);

private:
  Preferences& prefs_;
  QSharedPointer<SurfaceReconstructor> surface_reconstructor_;
  QSharedPointer<LegacyMeshGenerator> legacy_reconstructor_;
};
