#include <vtkPointData.h>
#include <vtkUnsignedLongArray.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

#include <Visualization/Viewer.h>

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
{

}

