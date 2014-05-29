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

#include <vtkSphereSource.h>
#include <vtkPointData.h>
#include <vtkUnsignedLongArray.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>

#include <Visualization/Viewer.h>

#include <Data/Mesh.h>
#include <Data/Shape.h>

const QString Viewer::INITIAL_C( "initial" );
const QString Viewer::GROOMED_C( "groomed" );
const QString Viewer::RECONSTRUCTED_C( "reconstructed" );

//-----------------------------------------------------------------------------
Viewer::Viewer()
{

  this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->tile_layout_width_ = 4;
  this->tile_layout_height_ = 4;
  this->camera_ = this->renderer_->GetActiveCamera();
  this->start_row_ = 0;
  this->first_draw_ = true;
  this->auto_center_ = true;
  this->mesh_mode_ = Viewer::INITIAL_C;
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::set_interactor( vtkRenderWindowInteractor* interactor )
{
  this->interactor_ = interactor;
}

//-----------------------------------------------------------------------------
void Viewer::insert_shape_into_view( QSharedPointer<Shape> shape, int position, int id )
{
  if ( position >= this->mini_viewers_.size() )
  {
    return;
  }

  QSharedPointer<Mesh> mesh;

  if ( this->mesh_mode_ == Viewer::INITIAL_C )
  {
    mesh = shape->get_initial_mesh();
  }
  else if ( this->mesh_mode_ == Viewer::GROOMED_C )
  {
    mesh = shape->get_groomed_mesh();
  }
  else if ( this->mesh_mode_ == Viewer::RECONSTRUCTED_C )
  {
    mesh = shape->get_reconstructed_mesh();
  }

  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
  QString note = mesh->get_filename();

  vtkSmartPointer<vtkPolyDataMapper> mapper = this->mini_viewers_[position]->surface_mapper_;
  vtkSmartPointer<vtkActor> actor = this->mini_viewers_[position]->surface_actor_;
  vtkSmartPointer<vtkRenderer> ren = this->mini_viewers_[position]->renderer_;

  vnl_vector<double> correspondence_points = shape->get_correspondence_points();

  int num_points = correspondence_points.size() / 3;

  QSharedPointer<MiniViewer> mini_viewer = this->mini_viewers_[position];

  if ( num_points > 0 )
  {
    mini_viewer->glyphs_->SetRange( 0.0, (double) num_points + 1 );
    mini_viewer->glyph_mapper_->SetScalarRange( 0.0, (double) num_points + 1.0 );
    this->lut_->SetNumberOfTableValues( num_points + 1 );
    this->lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

    mini_viewer->glyph_points_->SetNumberOfPoints( num_points );

    ( (vtkUnsignedLongArray*)( mini_viewer->glyph_point_set_->GetPointData()->GetScalars() ) )->SetNumberOfTuples( num_points );

    unsigned int idx = 0;
    for ( int i = 0; i < num_points; i++ )
    {
      ( (vtkUnsignedLongArray*)( mini_viewer->glyph_point_set_->GetPointData()->GetScalars() ) )->InsertValue( i, i );
      double x = correspondence_points[idx++];
      double y = correspondence_points[idx++];
      double z = correspondence_points[idx++];

      mini_viewer->glyph_points_->InsertPoint( i, x, y, z );
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
       mini_viewer->glyph_points_->GetPoint( i, pos );
       pos[0] += center[0];
       pos[1] += center[1];
       pos[2] += center[2];
       mini_viewer->glyph_points_->InsertPoint( i, pos );
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
  ren->AddActor( mini_viewer->glyph_actor );

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
void Viewer::clear_renderers()
{
  for ( int i = 0; i < this->mini_viewers_.size(); i++ )
  {
    this->mini_viewers_[i]->renderer_->RemoveAllViewProps();
  }
}

//-----------------------------------------------------------------------------
void Viewer::display_meshes()
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
void Viewer::set_render_window( vtkRenderWindow* renderWindow )
{
  this->render_window_ = renderWindow;
  this->render_window_->AddRenderer( this->renderer_ );

  this->setup_renderers();
}

//-----------------------------------------------------------------------------
void Viewer::setup_renderers()
{
  for ( int i = 0; i < this->mini_viewers_.size(); i++ )
  {
    this->render_window_->RemoveRenderer( this->mini_viewers_[i]->renderer_ );
  }

  this->mini_viewers_.clear();

  int* size = this->render_window_->GetSize();

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  int total = width * height;

  float margin = 0.005;

  float tile_height = ( 1.0f - ( margin * ( height + 1 ) ) ) / height;
  float tile_width = ( 1.0f - ( margin * ( width + 1 ) ) ) / width;

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;

  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  this->lut_ = vtkSmartPointer<vtkLookupTable>::New();

  for ( int y = 0; y < height; y++ )
  {
    for ( int x = 0; x < width; x++ )
    {
      int i = ( y * width ) + x;

      vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
      ren->SetActiveCamera( this->camera_ );

      QSharedPointer<MiniViewer> mini_viewer = QSharedPointer<MiniViewer>( new MiniViewer() );
      mini_viewer->renderer_ = ren;

      mini_viewer->surface_actor_ = vtkSmartPointer<vtkActor>::New();
      mini_viewer->surface_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();

      mini_viewer->glyph_points_ = vtkSmartPointer<vtkPoints>::New();
      mini_viewer->glyph_points_->SetDataTypeToDouble();
      mini_viewer->glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
      mini_viewer->glyph_point_set_->SetPoints( mini_viewer->glyph_points_ );
      mini_viewer->glyph_point_set_->GetPointData()->SetScalars( vtkSmartPointer<vtkUnsignedLongArray>::New() );

      mini_viewer->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
      mini_viewer->glyphs_->SetInputData( mini_viewer->glyph_point_set_ );
      mini_viewer->glyphs_->ScalingOn();
      mini_viewer->glyphs_->ClampingOff();
      mini_viewer->glyphs_->SetScaleModeToDataScalingOff();
      mini_viewer->glyphs_->SetSourceConnection( sphereSource->GetOutputPort() );

      mini_viewer->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
      mini_viewer->glyphs_->SetInputData( mini_viewer->glyph_point_set_ );
      mini_viewer->glyphs_->ScalingOn();
      mini_viewer->glyphs_->ClampingOff();
      mini_viewer->glyphs_->SetScaleModeToDataScalingOff();
      mini_viewer->glyphs_->SetSourceConnection( sphereSource->GetOutputPort() );

      mini_viewer->glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
      mini_viewer->glyph_mapper_->SetInputConnection( mini_viewer->glyphs_->GetOutputPort() );
      mini_viewer->glyph_mapper_->SetLookupTable( this->lut_ );

      mini_viewer->glyph_actor = vtkSmartPointer<vtkActor>::New();
      mini_viewer->glyph_actor->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
      mini_viewer->glyph_actor->GetProperty()->SetDiffuse( 0.8 );
      mini_viewer->glyph_actor->GetProperty()->SetSpecular( 0.3 );
      mini_viewer->glyph_actor->GetProperty()->SetSpecularPower( 10.0 );
      mini_viewer->glyph_actor->SetMapper( mini_viewer->glyph_mapper_ );

      this->mini_viewers_.push_back( mini_viewer );

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
void Viewer::set_tile_layout( int width, int height )
{
  this->tile_layout_width_ = width;
  this->tile_layout_height_ = height;

  this->setup_renderers();
  this->display_meshes();
}

//-----------------------------------------------------------------------------
int Viewer::get_num_rows()
{
  return std::ceil( (float)this->shapes_.size() / (float)this->tile_layout_width_ );
}

//-----------------------------------------------------------------------------
int Viewer::get_num_rows_visible()
{
  return this->tile_layout_height_;
}

//-----------------------------------------------------------------------------
void Viewer::set_start_row( int row )
{
  this->start_row_ = row;
  this->display_meshes();
  this->render_window_->Render();
}

//-----------------------------------------------------------------------------
void Viewer::set_auto_center( bool center )
{
  this->auto_center_ = center;
  this->first_draw_ = true;
}

//-----------------------------------------------------------------------------
void Viewer::set_shapes( std::vector<QSharedPointer<Shape> > shapes )
{
  this->shapes_ = shapes;
  this->display_meshes();
}

//-----------------------------------------------------------------------------
void Viewer::set_mesh_mode( QString mode )
{
  this->mesh_mode_ = mode;
  this->display_meshes();
}
