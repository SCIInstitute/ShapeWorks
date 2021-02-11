#include <Data/StudioMesh.h>
#include <Visualization/DisplayObject.h>
namespace shapeworks {

//-----------------------------------------------------------------------------
DisplayObject::DisplayObject()
{}

//-----------------------------------------------------------------------------
DisplayObject::~DisplayObject()
{}

//-----------------------------------------------------------------------------
void DisplayObject::set_mesh(QSharedPointer<StudioMesh> mesh)
{
  this->mesh_ = mesh;
}

//-----------------------------------------------------------------------------
QSharedPointer<StudioMesh> DisplayObject::get_mesh()
{
  return this->mesh_;
}

//-----------------------------------------------------------------------------
void DisplayObject::set_annotations(QStringList annotations)
{
  this->corner_annotations_ = annotations;
}

//-----------------------------------------------------------------------------
QStringList DisplayObject::get_annotations()
{
  return this->corner_annotations_;
}

//-----------------------------------------------------------------------------
void DisplayObject::set_correspondence_points(vnl_vector<double> points)
{
  this->correspondence_points_ = points;
}

//-----------------------------------------------------------------------------
vnl_vector<double> DisplayObject::get_correspondence_points()
{
  return this->correspondence_points_;
}

//-----------------------------------------------------------------------------
void DisplayObject::set_transform(const vnl_vector<double>& transform)
{
  this->transform_ = transform;
}

//-----------------------------------------------------------------------------
vnl_vector<double> DisplayObject::get_transform()
{
  return this->transform_;
}

//---------------------------------------------------------------------------
QList<Shape::Point> DisplayObject::get_exclusion_sphere_centers()
{
  return this->exclusion_sphere_centers_;
}

//---------------------------------------------------------------------------
void DisplayObject::set_exclusion_sphere_centers(QList<Shape::Point> centers)
{
  this->exclusion_sphere_centers_ = centers;
}

//---------------------------------------------------------------------------
QList<double> DisplayObject::get_exclusion_sphere_radii()
{
  return this->exclusion_sphere_radii_;
}

//---------------------------------------------------------------------------
void DisplayObject::set_exclusion_sphere_radii(QList<double> radii)
{
  this->exclusion_sphere_radii_ = radii;
}

//---------------------------------------------------------------------------
std::vector<Shape::Point> DisplayObject::get_vectors()
{
  return this->vectors_;
}

//---------------------------------------------------------------------------
void DisplayObject::set_vectors(std::vector<Shape::Point> vectors)
{
  this->vectors_ = vectors;
}
}