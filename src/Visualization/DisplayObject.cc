#include <Data/Mesh.h>
#include <Visualization/DisplayObject.h>

//-----------------------------------------------------------------------------
DisplayObject::DisplayObject()
{}

//-----------------------------------------------------------------------------
DisplayObject::~DisplayObject()
{}

//-----------------------------------------------------------------------------
void DisplayObject::set_mesh( QSharedPointer<Mesh> mesh )
{
  this->mesh_ = mesh;
}

//-----------------------------------------------------------------------------
QSharedPointer<Mesh> DisplayObject::get_mesh()
{
  return this->mesh_;
}

//-----------------------------------------------------------------------------
void DisplayObject::set_annotations( QStringList annotations )
{
  this->corner_annotations_ = annotations;
}

//-----------------------------------------------------------------------------
QStringList DisplayObject::get_annotations()
{
  return this->corner_annotations_;
}

//-----------------------------------------------------------------------------
void DisplayObject::set_correspondence_points( vnl_vector<double> points )
{
  this->correspondence_points_ = points;
}

//-----------------------------------------------------------------------------
vnl_vector<double> DisplayObject::get_correspondence_points()
{
  return this->correspondence_points_;
}

//-----------------------------------------------------------------------------
void DisplayObject::center()
{

}
