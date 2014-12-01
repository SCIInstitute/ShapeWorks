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

#include <Application/Preferences.h>
#include <Data/Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Viewer.h>
#include <Visualization/DisplayObject.h>
#include <Visualization/StudioInteractorStyle.h>

/*

   class ScenePickCallback : public vtkCommand
   {
   public:
   static ScenePickCallback *New();
   virtual void Execute(vtkObject *caller, unsigned long eventId, void* data);

   vtkSmartPointer<vtkActor> actorToPick;
   vtkSmartPointer<vtkGlyph3D> glyphsToPick;
   vtkSmartPointer<vtkRenderWindowInteractor> sceneRenderWindowInteractor;

   private:
   ScenePickCallback();
   };


   void ScenePickCallback::Execute(vtkObject *caller, unsigned long eventId, void* data)
   {
   if(eventId == vtkCommand::LeftButtonReleaseEvent)
   {
    int pickPosition[2];
    sceneRenderWindowInteractor->GetEventPosition(pickPosition);

    sceneRenderWindowInteractor->GetPicker()->Pick(pickPosition[0], pickPosition[1], 0, sceneRenderer);

    vtkCellPicker *cellPicker = reinterpret_cast<vtkCellPicker*>(picker.GetPointer());
    if(cellPicker)
    {
      // Check if correct actor was picked
      if(cellPicker->GetActor() == actorToPick.GetPointer())
      {
        vtkDataArray *inputIds = glyphsToPick->GetOutput()->GetPointData()->GetArray("InputPointIds");

        if(inputIds)
        {
          vtkCell *cell = glyphsToPick->GetOutput()->GetCell(cellPicker->GetCellId());

          if(cell && cell->GetNumberOfPoints() > 0)
          {
            // get first PointId from picked cell
            vtkIdType inputId = cell->GetPointId(0);

            // get matching Id from "InputPointIds" array
            vtkIdType selectedPointId = inputIds->GetTuple1(inputId);

            if(selectedPointId >= 0)
            {
              // POINT PICKED!
              // selectedPointId = Picked PointID (original input points)
            }
          }
        }

      }
      else
      {
        // NONE PICKED
      }
    }
   }
   }
 */

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

  this->lut_ = vtkSmartPointer<vtkLookupTable>::New();

  this->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs_->SetInputData( this->glyph_point_set_ );
  this->glyphs_->ScalingOn();
  this->glyphs_->ClampingOff();
  this->glyphs_->SetScaleModeToDataScalingOff();
  this->glyphs_->SetSourceConnection( sphere_source->GetOutputPort() );
  this->glyphs_->GeneratePointIdsOn();

  this->glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->glyph_mapper_->SetInputConnection( this->glyphs_->GetOutputPort() );
  this->glyph_mapper_->SetLookupTable( this->lut_ );

  this->glyph_actor_ = vtkSmartPointer<vtkActor>::New();
  this->glyph_actor_->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
  this->glyph_actor_->GetProperty()->SetDiffuse( 0.8 );
  this->glyph_actor_->GetProperty()->SetSpecular( 0.3 );
  this->glyph_actor_->GetProperty()->SetSpecularPower( 10.0 );
  this->glyph_actor_->SetMapper( this->glyph_mapper_ );

  this->glyph_size_ = 1.0f;
  this->glyph_quality_ = 5.0f;
  this->update_glyph_properties();

  this->style_ = vtkSmartPointer<StudioInteractorStyle>::New();
  this->style_->set_viewer( this );

  this->visible_ = false;
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::display_object( QSharedPointer<DisplayObject> object )
{
  this->visible_ = true;

  QSharedPointer<Mesh> mesh = object->get_mesh();
  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

  vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mapper_;
  vtkSmartPointer<vtkActor> actor = this->surface_actor_;
  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  vnl_vector<double> correspondence_points = object->get_correspondence_points();

  int num_points = correspondence_points.size() / 3;

  vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*)( this->glyph_point_set_->GetPointData()->GetScalars() );

  if ( num_points > 0 )
  {
    this->glyphs_->SetRange( 0.0, (double) num_points + 1 );
    this->glyph_mapper_->SetScalarRange( 0.0, (double) num_points + 1.0 );
    this->lut_->SetNumberOfTableValues( num_points + 1 );
    this->lut_->SetTableRange( 0.0, (double)num_points + 1.0 );

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

  this->style_->SetDefaultRenderer( this->renderer_ );

  this->renderer_->GetRenderWindow()->GetInteractor()->SetInteractorStyle( this->style_ );
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

void Viewer::handle_pick( int* click_pos )
{
  std::cerr << "handle pick!\n";

  vtkSmartPointer<vtkPointPicker> picker = vtkSmartPointer<vtkPointPicker>::New();

  picker->AddPickList( this->glyph_actor_ );
  picker->PickFromListOn();

  picker->Pick( click_pos[0],
                click_pos[1],
                0, // always zero.
                this->renderer_ );

  int id = picker->GetPointId();

  if ( id == -1 )
  {
    // miss
    return;
  }

  vtkIdType glyph_id = vtkIdTypeArray::SafeDownCast(this->glyphs_->GetOutput()->GetPointData()->GetArray("InputPointIds"))
    ->GetValue(id);


  std::cerr << "picked point :" << glyph_id << "\n";
}
