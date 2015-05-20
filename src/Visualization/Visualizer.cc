#include <vtkMath.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <Visualization/Visualizer.h>
#include <Data/Shape.h>

const QString Visualizer::MODE_ORIGINAL_C( "Original" );
const QString Visualizer::MODE_GROOMED_C( "Groomed" );
const QString Visualizer::MODE_RECONSTRUCTION_C( "Reconstructed" );

//-----------------------------------------------------------------------------
Visualizer::Visualizer(Preferences &prefs) : preferences_(prefs)
{
  this->display_mode_ = Visualizer::MODE_ORIGINAL_C;

  QObject::connect(
    &preferences_, SIGNAL( glyph_properties_changed() ),
    this, SLOT( update_viewer_properties() ) );

  this->show_glyphs_ = true;
  this->show_surface_ = true;
  this->stats_ready_ = false;

  this->glyph_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  this->selected_point_one_ = -1;
  this->selected_point_two_ = -1;
  this->cached_ = false;
}

//-----------------------------------------------------------------------------
Visualizer::~Visualizer()
{}

//-----------------------------------------------------------------------------
void Visualizer::set_lightbox( LightboxHandle lightbox )
{
  this->lightbox_ = lightbox;
  this->lightbox_->set_visualizer( this );
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
void Visualizer::display_samples()
{

  this->update_viewer_properties();

  QVector < QSharedPointer < DisplayObject > > display_objects;

  QVector < QSharedPointer < Shape > > shapes = this->project_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<DisplayObject> object = QSharedPointer<DisplayObject>( new DisplayObject() );

    QSharedPointer<Mesh> mesh;
    QString filename;

    //load based on preference, but always load something

    mesh = shapes[i]->get_original_mesh();
    filename = shapes[i]->get_original_filename();
    object->set_correspondence_points( shapes[i]->get_local_correspondence_points() );

    if ( !mesh || this->display_mode_ == Visualizer::MODE_GROOMED_C )
    {
      mesh = shapes[i]->get_groomed_mesh();
      filename = shapes[i]->get_groomed_filename();
    }

    if ( !mesh || this->display_mode_ == Visualizer::MODE_RECONSTRUCTION_C )
    {
      mesh = shapes[i]->get_reconstructed_mesh();
      filename = shapes[i]->get_global_point_filename();

      // use global correspondence points for reconstructed mesh
      object->set_correspondence_points( shapes[i]->get_global_correspondence_points() );
    }
    object->set_mesh( mesh );

    if ( this->display_mode_ != Visualizer::MODE_RECONSTRUCTION_C )
    {
      object->set_exclusion_sphere_centers( shapes[i]->get_exclusion_sphere_centers() );
      object->set_exclusion_sphere_radii( shapes[i]->get_exclusion_sphere_radii() );
    }

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
  if ( !this->compute_stats() )
  {
    return;
  }

  this->display_shape( this->stats.Mean() , 0.);
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape( const vnl_vector<double> &points , double value)
{
  
  QVector<DisplayObjectHandle> *list_ptr = getList(points,value);
  this->lightbox_->set_display_objects( *list_ptr );
  this->lightbox_->redraw();
}


//-----------------------------------------------------------------------------
QVector<DisplayObjectHandle> * Visualizer::getList( const vnl_vector<double> &points , double value) {
  QVector<DisplayObjectHandle> *list_ptr = NULL;
  //std::cerr << "mapped value: " << (value) << std::endl;
  std::map<double,QVector<DisplayObjectHandle>>::iterator it = disp_handles_.find(value);
  if (it == disp_handles_.end()) {
	  MeshHandle mesh = MeshHandle( new Mesh() );
	  mesh->create_from_pointset( points );

	  DisplayObjectHandle object = DisplayObjectHandle( new DisplayObject() );

	  object->set_mesh( mesh );
	  object->set_correspondence_points( points );

	  QStringList annotations;
	  annotations << "computed shape";
	  annotations << "";
	  annotations << "";
	  annotations << "";
	  object->set_annotations( annotations );
	  list_ptr = new QVector<DisplayObjectHandle>();
	  *list_ptr << object;
	  disp_handles_.insert(std::pair<double,QVector<DisplayObjectHandle>>(value,*list_ptr) );
  } else 
	  list_ptr = &it->second;
  return list_ptr;
}

//-----------------------------------------------------------------------------
void Visualizer::cache_data(int mode, double value)
{
  //std::cerr << "cached value: " << (value + 10. * mode) << std::endl;
  this->getList(getShape(mode,value),value + 10. * mode);
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_glyphs( bool show )
{
  this->show_glyphs_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_surface( bool show )
{
  this->show_surface_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::update_viewer_properties()
{
  double size = preferences_.get_glyph_size();
  double quality = preferences_.get_glyph_quality();

  std::cerr << "** update viewer properties, size = " << size << "\n";

  if ( this->lightbox_ )
  {
    foreach( ViewerHandle viewer, this->lightbox_->get_viewers() ) {
      viewer->set_glyph_size_and_quality( size, quality );
      viewer->set_show_glyphs( this->show_glyphs_ );
      viewer->set_show_surface( this->show_surface_ );
    }

    this->update_lut();

    this->lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
bool Visualizer::compute_stats()
{
  if ( this->stats_ready_ )
  {
    return true;
  }

  if ( this->project_->get_shapes().size() == 0 || !this->project_->reconstructed_present() )
  {
    return false;
  }

  std::vector < vnl_vector < double > > points;
  foreach( ShapeHandle shape, this->project_->get_shapes() ) {
    points.push_back( shape->get_global_correspondence_points() );
  }

  this->stats.ImportPoints( points );
  this->stats.ComputeModes();
  this->stats_ready_ = true;

  return true;
}

//-----------------------------------------------------------------------------
vnl_vector<double> Visualizer::getShape( int mode, double value ) {
  unsigned int m = this->stats.Eigenvectors().columns() - ( mode );

  vnl_vector<double> e = this->stats.Eigenvectors().get_column( m );

  double lambda = sqrt( this->stats.Eigenvalues()[m] );

  emit pca_labels_changed( QString::number( value, 'g', 2 ),
                           QString::number( this->stats.Eigenvalues()[m] ),
                           QString::number( value * lambda ) );

  std::map<double,QVector<DisplayObjectHandle>>::iterator it = disp_handles_.find(value + 10. * mode);
  if (it != disp_handles_.end()) return it->second[0]->get_correspondence_points();

  return this->stats.Mean() + ( e * ( value * lambda ) );
}

//-----------------------------------------------------------------------------
void Visualizer::display_pca( int mode, double value )
{
  if ( !this->compute_stats() )
  {
    return;
  }
  this->display_shape( getShape(mode,value) , value + mode * 10.);
}

//-----------------------------------------------------------------------------
void Visualizer::update_lut()
{
  if ( !this->compute_stats() )
  {
    return;
  }

  vnl_vector<double> mean_shape = this->stats.Mean();

  int num_points = mean_shape.size() / 3;

  this->glyph_lut_->SetNumberOfTableValues( num_points + 1 );
  this->glyph_lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

  if ( this->selected_point_one_ < 0 )
  {
    this->glyph_lut_->ForceBuild();
  }
  else
  {

    if ( this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0 )
    {
      // measurement mode

      for ( int i = 0; i < num_points; i++ )
      {
        this->glyph_lut_->SetTableValue( i, 0, 0, 0 );

        if ( this->selected_point_one_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 1 );
        }

        if ( this->selected_point_two_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 0 );
        }
      }
    }
    else
    {
      // color by distance from the selected point

      double p1[3];
      p1[0] = mean_shape[ this->selected_point_one_ * 3 + 0];
      p1[1] = mean_shape[ this->selected_point_one_ * 3 + 1];
      p1[2] = mean_shape[ this->selected_point_one_ * 3 + 2];

      std::vector<double> distances;
      double max_distance = 0;
      for ( int i = 0; i < num_points; i++ )
      {

        double p2[3];
        p2[0] = mean_shape[ i * 3 + 0];
        p2[1] = mean_shape[ i * 3 + 1];
        p2[2] = mean_shape[ i * 3 + 2];

        double distance = sqrt( vtkMath::Distance2BetweenPoints( p1, p2 ) );
        distances.push_back( distance );
        if ( distance > max_distance )
        {
          max_distance = distance;
        }
      }

      vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
      lut->SetTableRange( 0, max_distance );
      lut->SetHueRange( 0, 1 );
      lut->SetSaturationRange( 1, 1 );
      lut->SetValueRange( 1, 1 );
      lut->Build();

      for ( int i = 0; i < num_points; i++ )
      {

        unsigned char* color = lut->MapValue( max_distance - distances[i] );

        //this->glyph_lut_->SetTableValue( i, distances[i] / max_distance, 0, 1 );
        this->glyph_lut_->SetTableValue( i, color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f );

        if ( this->selected_point_one_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 1, 1, 1 );
        }

        if ( this->selected_point_two_ == i )
        {
          this->glyph_lut_->SetTableValue( i, 0, 0, 0 );
        }
      }
    }
  }

  this->glyph_lut_->Modified();
  this->lightbox_->set_glyph_lut( this->glyph_lut_ );
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_one( int id )
{
  this->selected_point_one_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_two( int id )
{
  this->selected_point_two_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::compute_measurements()
{
  if ( this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0 )
  {}
}
