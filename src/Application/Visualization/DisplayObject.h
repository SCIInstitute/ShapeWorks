#ifndef STUDIO_VISUALIZATION_DISPLAYOBJECT_H
#define STUDIO_VISUALIZATION_DISPLAYOBJECT_H

#include <QSharedPointer>
#include <QStringList>

#include <vnl/vnl_vector.h>

#include <Data/Shape.h>

class Mesh;

class DisplayObject;
typedef QSharedPointer< DisplayObject > DisplayObjectHandle;

//! Representation of everything displayed in a single Viewer
/*!
 * The DisplayObject class encapsulates all the data displayed by a single viewer
 *
 */
class DisplayObject
{
public:
  DisplayObject();
  ~DisplayObject();

  void set_mesh( QSharedPointer<Mesh> mesh );
  QSharedPointer<Mesh> get_mesh();

  void set_annotations( QStringList annotations );
  QStringList get_annotations();

  void set_correspondence_points( vnl_vector<double> points );
  vnl_vector<double> get_correspondence_points();

  void set_transform( const vnl_vector<double>& transform );
  vnl_vector<double> get_transform();

  QList<Point> get_exclusion_sphere_centers();
  void set_exclusion_sphere_centers(QList<Point> centers);

  QList<double> get_exclusion_sphere_radii();
  void set_exclusion_sphere_radii(QList<double> radii);

private:
  QSharedPointer<Mesh> mesh_;
  vnl_vector<double> correspondence_points_;
  QStringList corner_annotations_;
  vnl_vector<double> transform_;

  QList<Point> exclusion_sphere_centers_;
  QList<double> exclusion_sphere_radii_;

};

#endif /* STUDIO_VISUALIZATION_DISPLAYOBJECT_H */
