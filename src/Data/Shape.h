#ifndef STUDIO_DATA_SHAPE_H
#define STUDIO_DATA_SHAPE_H

#include <QSharedPointer>

#include <Data/Mesh.h>

//! Representation of a single shape/patient.
class Shape
{

public:
  Shape();
  ~Shape();

  /// Import the initial raw image file
  void import_initial_file( QString filename );

  /// Retrieve the initial mesh
  QSharedPointer<Mesh> get_initial_mesh();

private:

  QSharedPointer<Mesh> initial_mesh_;
  QSharedPointer<Mesh> groomed_mesh_;
  QSharedPointer<Mesh> reconstructed_mesh_;
};

#endif /* STUDIO_DATA_SHAPE_H */