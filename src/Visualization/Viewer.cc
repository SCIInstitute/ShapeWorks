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

  vtkSmartPointer<vtkSphereSource> sphere_source = vtkSmartPointer<vtkSphereSource>::New();

  this->lut_ = vtkSmartPointer<vtkLookupTable>::New();

  this->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs_->SetInputData( this->glyph_point_set_ );
  this->glyphs_->ScalingOn();
  this->glyphs_->ClampingOff();
  this->glyphs_->SetScaleModeToDataScalingOff();
  this->glyphs_->SetSourceConnection( sphere_source->GetOutputPort() );

  this->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs_->SetInputData( this->glyph_point_set_ );
  this->glyphs_->ScalingOn();
  this->glyphs_->ClampingOff();
  this->glyphs_->SetScaleModeToDataScalingOff();
  this->glyphs_->SetSourceConnection( sphere_source->GetOutputPort() );

  this->glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->glyph_mapper_->SetInputConnection( this->glyphs_->GetOutputPort() );
  this->glyph_mapper_->SetLookupTable( this->lut_ );

  this->glyph_actor = vtkSmartPointer<vtkActor>::New();
  this->glyph_actor->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
  this->glyph_actor->GetProperty()->SetDiffuse( 0.8 );
  this->glyph_actor->GetProperty()->SetSpecular( 0.3 );
  this->glyph_actor->GetProperty()->SetSpecularPower( 10.0 );
  this->glyph_actor->SetMapper( this->glyph_mapper_ );
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::display_object( QSharedPointer<DisplayObject> object )
{
  std::cerr << "display\n";
  QSharedPointer<Mesh> mesh = object->get_mesh();

  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

  vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mapper_;
  vtkSmartPointer<vtkActor> actor = this->surface_actor_;
  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  vnl_vector<double> correspondence_points = object->get_correspondence_points();

  int num_points = correspondence_points.size() / 3;

  if ( num_points > 0 )
  {
    this->glyphs_->SetRange( 0.0, (double) num_points + 1 );
    this->glyph_mapper_->SetScalarRange( 0.0, (double) num_points + 1.0 );
    this->lut_->SetNumberOfTableValues( num_points + 1 );
    this->lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

    this->glyph_points_->Reset();
    this->glyph_points_->SetNumberOfPoints( num_points );

    vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*)( this->glyph_point_set_->GetPointData()->GetScalars() );

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
  ren->AddActor( actor );
  ren->AddActor( this->glyph_actor );

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

  ren->AddViewProp( corner_annotation );
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
