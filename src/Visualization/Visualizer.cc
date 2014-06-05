#include <Visualization/Visualizer.h>
#include <Visualization/DisplayObject.h>
#include <Data/Shape.h>

#include <Application/Preferences.h>

const QString Visualizer::MODE_ORIGINAL_C( "Original" );
const QString Visualizer::MODE_GROOMED_C( "Groomed" );
const QString Visualizer::MODE_RECONSTRUCTION_C( "Reconstructed" );

//-----------------------------------------------------------------------------
Visualizer::Visualizer()
{
  this->display_mode_ = Visualizer::MODE_ORIGINAL_C;

  QObject::connect(
    &Preferences::Instance(), SIGNAL( glyph_properties_changed() ),
    this, SLOT( update_viewer_properties() ) );

  this->show_glyphs_ = true;
  this->show_surface_ = true;
}

//-----------------------------------------------------------------------------
Visualizer::~Visualizer()
{}

//-----------------------------------------------------------------------------
void Visualizer::set_lightbox( LightboxHandle lightbox )
{
  this->lightbox_ = lightbox;
}

//-----------------------------------------------------------------------------
void Visualizer::set_project( ProjectHandle project )
{
  this->project_ = project;
}

//-----------------------------------------------------------------------------
void Visualizer::set_display_mode( QString mode )
{
  this->display_mode_ = mode;
}

//-----------------------------------------------------------------------------
void Visualizer::set_center( bool center )
{
  this->center_ = center;
}

//-----------------------------------------------------------------------------
void Visualizer::update_display()
{
  this->update_viewer_properties();

  QVector < QSharedPointer < DisplayObject > > display_objects;

  QVector < QSharedPointer < Shape > > shapes = this->project_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<DisplayObject> object = QSharedPointer<DisplayObject>( new DisplayObject() );

    QSharedPointer<Mesh> mesh;
    QString filename;
    if ( this->display_mode_ == Visualizer::MODE_ORIGINAL_C )
    {
      mesh = shapes[i]->get_initial_mesh();
      filename = shapes[i]->get_initial_filename();
    }
    else if ( this->display_mode_ == Visualizer::MODE_GROOMED_C )
    {
      mesh = shapes[i]->get_groomed_mesh();
      filename = shapes[i]->get_groomed_filename();
    }
    else if ( this->display_mode_ == Visualizer::MODE_RECONSTRUCTION_C )
    {
      mesh = shapes[i]->get_reconstructed_mesh();
      filename = shapes[i]->get_point_filename();
    }
    object->set_mesh( mesh );

    object->set_correspondence_points( shapes[i]->get_correspondence_points() );

    QStringList annotations;
    annotations << filename;
    annotations << "";
    annotations << QString::number( shapes[i]->get_id() );
    annotations << "";
    object->set_annotations( annotations );

    if ( this->center_ )
    {
      object->set_transform( mesh->get_center_transform() );
    }
    display_objects << object;
  }

  this->lightbox_->set_display_objects( display_objects );
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::display_mean()
{
  std::vector < vnl_vector < double > > points;
  foreach( ShapeHandle shape, this->project_->get_shapes() ) {
    points.push_back( shape->get_correspondence_points() );
  }

  this->stats.ImportPoints( points );

  this->stats.ComputeModes();

  vnl_vector<double> mean_shape = this->stats.Mean();

  MeshHandle mesh = MeshHandle( new Mesh() );
  mesh->create_from_pointset( mean_shape );

  DisplayObjectHandle object = DisplayObjectHandle( new DisplayObject() );

  object->set_mesh( mesh );
  object->set_correspondence_points( mean_shape );

  QStringList annotations;
  annotations << "mean";
  annotations << "";
  annotations << "";
  annotations << "";
  object->set_annotations( annotations );

  QVector<DisplayObjectHandle> list;
  list << object;
  this->lightbox_->set_display_objects( list );
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_glyphs( bool show )
{
  this->show_glyphs_ = show;
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_surface( bool show )
{
  this->show_surface_ = show;
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::update_viewer_properties()
{
  double size = Preferences::Instance().get_glyph_size();
  double quality = Preferences::Instance().get_glyph_quality();

  std::cerr << "** update viewer properties, size = " << size << "\n";

  if ( this->lightbox_ )
  {
    foreach( ViewerHandle viewer, this->lightbox_->get_viewers() ) {
      viewer->set_glyph_size_and_quality( size, quality );
      viewer->set_show_glyphs( this->show_glyphs_ );
      viewer->set_show_surface( this->show_surface_ );
    }

    this->lightbox_->redraw();
  }
}
