#pragma once

#include <QSharedPointer>
#include <QStringList>

#include <vnl/vnl_vector.h>

#include <Data/Shape.h>

namespace shapeworks {

class StudioMesh;

class DisplayObject;
typedef QSharedPointer<DisplayObject> DisplayObjectHandle;

//! Representation of everything displayed in a single Viewer
/*!
 * The DisplayObject class encapsulates all the data displayed by a single viewer
 *
 */
class DisplayObject {
public:

  DisplayObject();
  ~DisplayObject();

  void set_mesh(QSharedPointer<StudioMesh> mesh);
  QSharedPointer<StudioMesh> get_mesh();

  void set_annotations(QStringList annotations);
  QStringList get_annotations();

  void set_correspondence_points(vnl_vector<double> points);
  vnl_vector<double> get_correspondence_points();

  void set_transform(const vnl_vector<double>& transform);
  vnl_vector<double> get_transform();

  QList<Shape::Point> get_exclusion_sphere_centers();
  void set_exclusion_sphere_centers(QList<Shape::Point> centers);

  QList<double> get_exclusion_sphere_radii();
  void set_exclusion_sphere_radii(QList<double> radii);

  std::vector<Shape::Point> get_vectors();
  void set_vectors(std::vector<Shape::Point> vectors);

private:
  QSharedPointer<StudioMesh> mesh_;
  vnl_vector<double> correspondence_points_;
  QStringList corner_annotations_;
  vnl_vector<double> transform_;

  std::vector<Shape::Point> vectors_;

  QList<Shape::Point> exclusion_sphere_centers_;
  QList<double> exclusion_sphere_radii_;
};
}