#include <cmath>

#include <QFileInfo>

#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>

#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkCenterOfMass.h>

#include <vtkPointData.h>
#include <vtkUnsignedLongArray.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>

#include <Visualization/Lightbox.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

const QString Lightbox::INITIAL_C( "initial" );
const QString Lightbox::GROOMED_C( "groomed" );
const QString Lightbox::RECONSTRUCTED_C( "reconstructed" );

//-----------------------------------------------------------------------------
Lightbox::Lightbox()
{

  this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->tile_layout_width_ = 4;
  this->tile_layout_height_ = 4;
  this->camera_ = this->renderer_->GetActiveCamera();
  this->start_row_ = 0;
  this->first_draw_ = true;
  this->auto_center_ = true;
  this->mesh_mode_ = Lightbox::INITIAL_C;
}

//-----------------------------------------------------------------------------
Lightbox::~Lightbox()
{}

//-----------------------------------------------------------------------------
void Lightbox::set_interactor( vtkRenderWindowInteractor* interactor )
{
  this->interactor_ = interactor;
}

//-----------------------------------------------------------------------------
void Lightbox::insert_shape_into_view( QSharedPointer<Shape> shape, int position, int id )
{
  if ( position >= this->viewers_.size() )
  {
    return;
  }

  QSharedPointer<Mesh> mesh;

  if ( this->mesh_mode_ == Lightbox::INITIAL_C )
  {
    mesh = shape->get_initial_mesh();
  }
  else if ( this->mesh_mode_ == Lightbox::GROOMED_C )
  {
    mesh = shape->get_groomed_mesh();
  }
  else if ( this->mesh_mode_ == Lightbox::RECONSTRUCTED_C )
  {
    mesh = shape->get_reconstructed_mesh();
  }

  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
  QString note = mesh->get_filename();

  QSharedPointer<Viewer> viewer = this->viewers_[position];

  vtkSmartPointer<vtkPolyDataMapper> mapper = viewer->surface_mapper_;
  vtkSmartPointer<vtkActor> actor = viewer->surface_actor_;
  vtkSmartPointer<vtkRenderer> ren = viewer->renderer_;

  vnl_vector<double> correspondence_points = shape->get_correspondence_points();

  int num_points = correspondence_points.size() / 3;


  if ( num_points > 0 )
  {
    viewer->glyphs_->SetRange( 0.0, (double) num_points + 1 );
    viewer->glyph_mapper_->SetScalarRange( 0.0, (double) num_points + 1.0 );
    viewer->lut_->SetNumberOfTableValues( num_points + 1 );
    viewer->lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

    viewer->glyph_points_->SetNumberOfPoints( num_points );

    ( (vtkUnsignedLongArray*)( viewer->glyph_point_set_->GetPointData()->GetScalars() ) )->SetNumberOfTuples( num_points );

    unsigned int idx = 0;
    for ( int i = 0; i < num_points; i++ )
    {
      ( (vtkUnsignedLongArray*)( viewer->glyph_point_set_->GetPointData()->GetScalars() ) )->InsertValue( i, i );
      double x = correspondence_points[idx++];
      double y = correspondence_points[idx++];
      double z = correspondence_points[idx++];

      viewer->glyph_points_->InsertPoint( i, x, y, z );
    }
  }

  if ( this->auto_center_ )
  {
    // Compute the center of mass
    vtkSmartPointer<vtkCenterOfMass> center_of_mass_filter = vtkSmartPointer<vtkCenterOfMass>::New();
    center_of_mass_filter->SetInputData( poly_data );
    center_of_mass_filter->SetUseScalarsAsWeights( false );
    center_of_mass_filter->Update();

    double center[3];
    center_of_mass_filter->GetCenter( center );
    double tx = -center[0];
    double ty = -center[1];
    double tz = -center[2];

    vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
    translation->Translate( tx, ty, tz );

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputData( poly_data );
    transformFilter->SetTransform( translation );
    transformFilter->Update();
    poly_data = transformFilter->GetOutput();

    /*    // translate correspondence points
       for ( int i = 0; i < num_points; i++ )
       {
       double pos[3];
       mini_lightbox->glyph_points_->GetPoint( i, pos );
       pos[0] += center[0];
       pos[1] += center[1];
       pos[2] += center[2];
       mini_lightbox->glyph_points_->InsertPoint( i, pos );
       }
     */
  }

#if VTK_MAJOR_VERSION <= 5
  mapper->SetInput( poly_data );
#else
  mapper->SetInputData( poly_data );
#endif
  actor->SetMapper( mapper );
  actor->GetProperty()->SetDiffuseColor( 1, 191.0 / 255.0, 0 );
  actor->GetProperty()->SetSpecular( 0.2 );
  actor->GetProperty()->SetSpecularPower( 15 );
  mapper->ScalarVisibilityOff();

  ren->RemoveAllViewProps();
  ren->AddActor( actor );
  ren->AddActor( viewer->glyph_actor );

  if ( this->first_draw_ )
  {
    this->first_draw_ = false;
    ren->ResetCamera();
  }

  vtkSmartPointer<vtkCornerAnnotation> corner_annotation =
    vtkSmartPointer<vtkCornerAnnotation>::New();
  corner_annotation->SetLinearFontScaleFactor( 2 );
  corner_annotation->SetNonlinearFontScaleFactor( 1 );
  corner_annotation->SetMaximumFontSize( 16 );
  corner_annotation->SetText( 2, QString::number( id ).toStdString().c_str() );
  corner_annotation->SetText( 0, note.toStdString().c_str() );
  corner_annotation->GetTextProperty()->SetColor( 0.50, 0.5, 0.5 );

  ren->AddViewProp( corner_annotation );
}

//-----------------------------------------------------------------------------
void Lightbox::clear_renderers()
{
  for ( int i = 0; i < this->viewers_.size(); i++ )
  {
    this->viewers_[i]->renderer_->RemoveAllViewProps();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::display_meshes()
{
  this->clear_renderers();

  // skip based on scrollbar
  int start_mesh = this->start_row_ * this->tile_layout_width_;

  int position = 0;
  for ( int i = start_mesh; i < this->shapes_.size(); i++ )
  {
    int id = i + 1;
    this->insert_shape_into_view( shapes_[i], position, id );
    position++;
  }

  this->render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_render_window( vtkRenderWindow* renderWindow )
{
  this->render_window_ = renderWindow;
  this->render_window_->AddRenderer( this->renderer_ );

  this->setup_renderers();
}

//-----------------------------------------------------------------------------
void Lightbox::setup_renderers()
{
  for ( int i = 0; i < this->viewers_.size(); i++ )
  {
    this->render_window_->RemoveRenderer( this->viewers_[i]->renderer_ );
  }

  this->viewers_.clear();

  int* size = this->render_window_->GetSize();

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  int total = width * height;

  float margin = 0.005;

  float tile_height = ( 1.0f - ( margin * ( height + 1 ) ) ) / height;
  float tile_width = ( 1.0f - ( margin * ( width + 1 ) ) ) / width;

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;


  for ( int y = 0; y < height; y++ )
  {
    for ( int x = 0; x < width; x++ )
    {
      int i = ( y * width ) + x;

      vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
      ren->SetActiveCamera( this->camera_ );

      QSharedPointer<Viewer> viewer = QSharedPointer<Viewer>( new Viewer() );
      viewer->renderer_ = ren;




      this->viewers_.push_back( viewer );

      double viewport[4] = {0.0, 0.0, 0.0, 0.0};

      // horizontal
      viewport[0] = margin + ( x * step_x );
      viewport[2] = viewport[0] + tile_width;

      // vertical
      viewport[1] = margin + ( ( ( height - 1 ) - y ) * step_y );
      viewport[3] = viewport[1] + tile_height;

      ren->SetViewport( viewport );

      double color = 0.2f;

      ren->SetBackground( color, color, color );

      this->render_window_->AddRenderer( ren );
    }
  }

  if ( this->render_window_->IsDrawable() )
  {
    this->render_window_->Render();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_tile_layout( int width, int height )
{
  this->tile_layout_width_ = width;
  this->tile_layout_height_ = height;

  this->setup_renderers();
  this->display_meshes();
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows()
{
  return std::ceil( (float)this->shapes_.size() / (float)this->tile_layout_width_ );
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows_visible()
{
  return this->tile_layout_height_;
}

//-----------------------------------------------------------------------------
void Lightbox::set_start_row( int row )
{
  this->start_row_ = row;
  this->display_meshes();
  this->render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_auto_center( bool center )
{
  this->auto_center_ = center;
  this->first_draw_ = true;
}

//-----------------------------------------------------------------------------
void Lightbox::set_shapes( std::vector<QSharedPointer<Shape> > shapes )
{
  this->shapes_ = shapes;
  this->display_meshes();
}

//-----------------------------------------------------------------------------
void Lightbox::set_mesh_mode( QString mode )
{
  this->mesh_mode_ = mode;
  this->display_meshes();
}
