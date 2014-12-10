#include <vtkPointData.h>
#include <vtkUnsignedLongArray.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkPointPicker.h>
#include <vtkIdTypeArray.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkCell.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageMapper.h>

#include <Application/Preferences.h>
#include <Data/Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Viewer.h>
#include <Visualization/DisplayObject.h>

//-----------------------------------------------------------------------------
Viewer::Viewer()
{
  this->surface_actor_ = vtkSmartPointer<vtkActor>::New();
  this->surface_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();

  this->glyph_points_ = vtkSmartPointer<vtkPoints>::New();
  this->glyph_points_->SetDataTypeToDouble();
  this->glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  this->glyph_point_set_->SetPoints( this->glyph_points_ );
  this->glyph_point_set_->GetPointData()->SetScalars( vtkSmartPointer<vtkUnsignedLongArray>::New() );

  this->sphere_source = vtkSmartPointer<vtkSphereSource>::New();

  this->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs_->SetInputData( this->glyph_point_set_ );
  this->glyphs_->ScalingOn();
  this->glyphs_->ClampingOff();
  this->glyphs_->SetScaleModeToDataScalingOff();
  this->glyphs_->SetSourceConnection( sphere_source->GetOutputPort() );
  this->glyphs_->GeneratePointIdsOn();

  this->glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->glyph_mapper_->SetInputConnection( this->glyphs_->GetOutputPort() );

  this->glyph_actor_ = vtkSmartPointer<vtkActor>::New();
  this->glyph_actor_->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
  this->glyph_actor_->GetProperty()->SetDiffuse( 0.8 );
  this->glyph_actor_->GetProperty()->SetSpecular( 0.3 );
  this->glyph_actor_->GetProperty()->SetSpecularPower( 10.0 );
  this->glyph_actor_->SetMapper( this->glyph_mapper_ );

  this->glyph_size_ = 1.0f;
  this->glyph_quality_ = 5.0f;
  this->update_glyph_properties();

  this->visible_ = false;
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::display_object( QSharedPointer<DisplayObject> object )
{
  this->visible_ = true;

  this->object_ = object;

  QSharedPointer<Mesh> mesh = object->get_mesh();

  vtkSmartPointer<vtkCornerAnnotation> corner_annotation =
    vtkSmartPointer<vtkCornerAnnotation>::New();
  corner_annotation->SetLinearFontScaleFactor( 2 );
  corner_annotation->SetNonlinearFontScaleFactor( 1 );
  corner_annotation->SetMaximumFontSize( 16 );

  QStringList annotations = object->get_annotations();

  corner_annotation->SetText( 0, ( annotations[0] ).toStdString().c_str() );
  corner_annotation->SetText( 1, ( annotations[1] ).toStdString().c_str() );
  corner_annotation->SetText( 2, ( annotations[2] ).toStdString().c_str() );
  corner_annotation->SetText( 3, ( annotations[3] ).toStdString().c_str() );

  corner_annotation->GetTextProperty()->SetColor( 0.50, 0.5, 0.5 );

  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  if ( !mesh )
  {
    // display loading message
    corner_annotation->SetText( 0, "Loading..." );

    std::string pixmap_name = std::string(":/Studio/Images/shapes-icon.png");
    QPixmap pixmap;
    pixmap.load( QString::fromStdString( pixmap_name ) ); 

    vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource =
      vtkSmartPointer<vtkQImageToImageSource>::New();
    QImage qimage = pixmap.toImage();
    qimageToImageSource->SetQImage(&qimage);
    qimageToImageSource->Update();

    vtkImageData* image = qimageToImageSource->GetOutput();

    // Map 2D image file
    vtkImageMapper *imageMapper = vtkImageMapper::New();
    imageMapper->SetInputConnection(qimageToImageSource->GetOutputPort());


    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);

    vtkSmartPointer<vtkImageActor> imageActor =
      vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInputData(image);



    // Actor in scene
//    vtkActor2D *mapActor = vtkActor2D::New();
//    mapActor->SetLayerNumber(0);
//    mapActor->SetMapper(imageMapper);	
    ren->AddViewProp( imageActor );

    ren->ResetCamera();

  }
  else
  {

    vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
    vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mapper_;
    vtkSmartPointer<vtkActor> actor = this->surface_actor_;

    vnl_vector<double> correspondence_points = object->get_correspondence_points();

    int num_points = correspondence_points.size() / 3;

    vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*)( this->glyph_point_set_->GetPointData()->GetScalars() );

    if ( num_points > 0 )
    {
      this->glyphs_->SetRange( 0.0, (double) num_points + 1 );
      this->glyph_mapper_->SetScalarRange( 0.0, (double) num_points + 1.0 );

      this->glyph_points_->Reset();
      this->glyph_points_->SetNumberOfPoints( num_points );

      scalars->Reset();
      scalars->SetNumberOfTuples( num_points );

      unsigned int idx = 0;
      for ( int i = 0; i < num_points; i++ )
      {
        scalars->InsertValue( i, i );
        double x = correspondence_points[idx++];
        double y = correspondence_points[idx++];
        double z = correspondence_points[idx++];

        this->glyph_points_->InsertPoint( i, x, y, z );
      }
    }
    else
    {
      this->glyph_points_->Reset();
      scalars->Reset();
    }
    this->glyph_points_->Modified();

    vnl_vector<double> transform = object->get_transform();

    if ( transform.size() == 3 )
    {
      double tx = -transform[0];
      double ty = -transform[1];
      double tz = -transform[2];

      vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
      translation->Translate( tx, ty, tz );

      vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transformFilter->SetInputData( poly_data );
      transformFilter->SetTransform( translation );
      transformFilter->Update();
      poly_data = transformFilter->GetOutput();
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
    //ren->AddActor( actor );
    //ren->AddActor( this->glyph_actor_ );
  }

  ren->AddViewProp( corner_annotation );

  this->update_actors();
  this->update_glyph_properties();
}

//-----------------------------------------------------------------------------
void Viewer::clear_viewer()
{
  this->renderer_->RemoveAllViewProps();
  this->visible_ = false;
}

//-----------------------------------------------------------------------------
void Viewer::reset_camera()
{
  this->renderer_->ResetCamera();
}

//-----------------------------------------------------------------------------
void Viewer::set_renderer( vtkSmartPointer<vtkRenderer> renderer )
{
  this->renderer_ = renderer;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkRenderer> Viewer::get_renderer()
{
  return this->renderer_;
}

//-----------------------------------------------------------------------------
void Viewer::set_glyph_size_and_quality( double size, double quality )
{
  this->glyph_size_ = size;
  this->glyph_quality_ = quality;
  this->update_glyph_properties();
}

//-----------------------------------------------------------------------------
void Viewer::update_glyph_properties()
{
//  std::cerr << "update glyph props\n";
  this->glyphs_->SetScaleFactor( this->glyph_size_ );
  //this->arrowGlayphs->SetScaleFactor( this->glyph_size_ );

  this->sphere_source->SetThetaResolution( this->glyph_quality_ );
  this->sphere_source->SetPhiResolution( this->glyph_quality_ );

  //this->arrowSource->SetTipResolution( this->glyph_quality_ );
  //this->arrowSource->SetShaftResolution( this->glyph_quality_ );

  this->glyphs_->Update();
  //this->arrowGlyphs->Update();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_glyphs( bool show )
{
  this->show_glyphs_ = show;
  this->update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_surface( bool show )
{
  this->show_surface_ = show;
  this->update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::update_actors()
{
  if ( !this->visible_ )
  {
    return;
  }

  this->renderer_->RemoveActor( this->glyph_actor_ );
  //this->renderer_->RemoveActor( this->arrowGlyphActor );

  this->renderer_->RemoveActor( this->surface_actor_ );

/*
   for ( int i = 0; i < this->numDomains; i++ )
   {
    this->renderer->RemoveActor( this->surfaceActors[i] );
   }
 */

  if ( this->show_glyphs_ )
  {
    this->renderer_->AddActor( this->glyph_actor_ );
/*    if ( this->arrowsVisible )
    {
      this->renderer->AddActor( this->arrowGlyphActor );
    }*/
  }

  if ( this->show_surface_ )
  {
/*    for ( int i = 0; i < this->numDomains; i++ )
    {
      this->renderer->AddActor( this->surfaceActors[i] );
    }*/
    this->renderer_->AddActor( this->surface_actor_ );
  }

  //this->displayShape( this->currentShape );

  //this->renderer_->Render();
}

int Viewer::handle_pick( int* click_pos )
{

  // First determine what was picked
  vtkSmartPointer<vtkPropPicker> prop_picker = vtkSmartPointer<vtkPropPicker>::New();
  prop_picker->Pick( click_pos[0], click_pos[1], 0, this->renderer_ );
  if ( prop_picker->GetActor() != this->glyph_actor_ )
  {
    return -1;
  }

  vtkSmartPointer<vtkCellPicker> cell_picker = vtkSmartPointer<vtkCellPicker>::New();
  cell_picker->Pick( click_pos[0], click_pos[1], 0, this->renderer_ );

  vtkDataArray* input_ids = this->glyphs_->GetOutput()->GetPointData()->GetArray( "InputPointIds" );

  if ( input_ids )
  {
    vtkCell* cell = this->glyphs_->GetOutput()->GetCell( cell_picker->GetCellId() );

    if ( cell && cell->GetNumberOfPoints() > 0 )
    {
      // get first PointId from picked cell
      vtkIdType input_id = cell->GetPointId( 0 );

      // get matching Id from "InputPointIds" array
      vtkIdType glyph_id = input_ids->GetTuple1( input_id );

      if ( glyph_id >= 0 )
      {
        std::cerr << "picked correspondence point :" << glyph_id << "\n";
        return glyph_id;
      }
    }
  }

  return -1;
}

//-----------------------------------------------------------------------------
void Viewer::set_lut( vtkSmartPointer<vtkLookupTable> lut )
{
  this->lut_ = lut;
  this->glyph_mapper_->SetLookupTable( this->lut_ );
}
