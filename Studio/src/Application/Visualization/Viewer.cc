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
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkImageGradient.h>
#include <vtkColorTransferFunction.h>
#include <vtkArrowSource.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataNormals.h>
#include <vtkImageGaussianSmooth.h>

#include <QDebug>

#include <Application/Data/CustomSurfaceReconstructionFilter.h>
#include <Data/Preferences.h>
#include <Data/Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Viewer.h>
#include <Visualization/DisplayObject.h>

//-----------------------------------------------------------------------------
Viewer::Viewer()
{
  this->image_actor_ = vtkSmartPointer<vtkImageActor>::New();

  this->surface_actor_ = vtkSmartPointer<vtkActor>::New();
  this->surface_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();

  this->sphere_source = vtkSmartPointer<vtkSphereSource>::New();

  this->differenceLUT = vtkSmartPointer<vtkColorTransferFunction>::New();
  this->differenceLUT->SetColorSpaceToHSV();

  this->glyph_points_ = vtkSmartPointer<vtkPoints>::New();
  this->glyph_points_->SetDataTypeToDouble();
  this->glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  this->glyph_point_set_->SetPoints(this->glyph_points_);
  this->glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkUnsignedLongArray>::New());

  this->glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs_->SetInputData(this->glyph_point_set_);
  this->glyphs_->ScalingOn();
  this->glyphs_->ClampingOff();
  this->glyphs_->SetScaleModeToDataScalingOff();
  this->glyphs_->SetSourceConnection(sphere_source->GetOutputPort());
  this->glyphs_->GeneratePointIdsOn();

  this->glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->glyph_mapper_->SetInputConnection(this->glyphs_->GetOutputPort());

  this->glyph_actor_ = vtkSmartPointer<vtkActor>::New();
  this->glyph_actor_->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  this->glyph_actor_->GetProperty()->SetDiffuse(0.8);
  this->glyph_actor_->GetProperty()->SetSpecular(0.3);
  this->glyph_actor_->GetProperty()->SetSpecularPower(10.0);
  this->glyph_actor_->SetMapper(this->glyph_mapper_);

  // exclusion spheres
  this->exclusion_sphere_points_ = vtkSmartPointer<vtkPoints>::New();
  this->exclusion_sphere_points_->SetDataTypeToDouble();
  this->exclusion_sphere_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  this->exclusion_sphere_point_set_->SetPoints(this->exclusion_sphere_points_);
  this->exclusion_sphere_point_set_->GetPointData()->SetScalars(
    vtkSmartPointer<vtkUnsignedLongArray>::New());

  this->exclusion_sphere_glyph_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->exclusion_sphere_glyph_->SetInputData(this->exclusion_sphere_point_set_);
  this->exclusion_sphere_glyph_->ScalingOn();
  this->exclusion_sphere_glyph_->ClampingOff();
  this->exclusion_sphere_glyph_->SetScaleModeToScaleByScalar();
  this->exclusion_sphere_glyph_->SetSourceConnection(sphere_source->GetOutputPort());
  //this->exclusion_sphere_glyph_->SetColorModeToColorByScale();

  this->exclusion_sphere_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->exclusion_sphere_mapper_->SetInputConnection(this->exclusion_sphere_glyph_->GetOutputPort());
  this->exclusion_sphere_mapper_->SetScalarVisibility(0);

  this->exclusion_sphere_actor_ = vtkSmartPointer<vtkActor>::New();
  //this->exclusion_sphere_actor_->GetProperty()->SetSpecularColor( 1.0, 1.0, 0.0 );
  //this->exclusion_sphere_actor_->GetProperty()->SetDiffuse( 0.8 );
  //this->exclusion_sphere_actor_->GetProperty()->SetSpecular( 0.3 );
  //this->exclusion_sphere_actor_->GetProperty()->SetSpecularPower( 10.0 );
  this->exclusion_sphere_actor_->GetProperty()->SetOpacity(0.5);
  this->exclusion_sphere_actor_->GetProperty()->SetColor(0, 1, 0);
  this->exclusion_sphere_actor_->SetMapper(this->exclusion_sphere_mapper_);

  this->arrowFlipFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->arrowGlyphs = vtkSmartPointer<vtkGlyph3D>::New();
  this->arrowGlyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->arrowGlyphActor = vtkSmartPointer<vtkActor>::New();

  // Arrow glyphs
  this->arrowSource = vtkSmartPointer<vtkArrowSource>::New();
  this->arrowSource->SetTipResolution(6);
  this->arrowSource->SetShaftResolution(6);

  vtkSmartPointer<vtkTransform> t1 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t2 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t3 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t4 = vtkSmartPointer<vtkTransform>::New();
  t1->Translate(-0.5, 0.0, 0.0);
  t2->RotateY(180);
  t3->Translate(0.5, 0.0, 0.0);
  t4->RotateY(180);
  t3->Concatenate(t4);
  t2->Concatenate(t3);
  t1->Concatenate(t2);
  this->transform180 = vtkSmartPointer<vtkTransform>::New();
  this->transform180->Concatenate(t1);

  this->arrowFlipFilter->SetTransform(this->transform180);
  this->arrowFlipFilter->SetInputConnection(this->arrowSource->GetOutputPort());

  this->arrowGlyphs->SetSourceConnection(this->arrowFlipFilter->GetOutputPort());
  this->arrowGlyphs->SetInputData(this->glyph_point_set_);
  this->arrowGlyphs->ScalingOn();
  this->arrowGlyphs->ClampingOff();

  this->arrowGlyphs->SetVectorModeToUseVector();
  this->arrowGlyphs->SetScaleModeToScaleByVector();

  this->arrowGlyphMapper->SetInputConnection(this->arrowGlyphs->GetOutputPort());

  this->arrowGlyphActor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  this->arrowGlyphActor->GetProperty()->SetDiffuse(0.8);
  this->arrowGlyphActor->GetProperty()->SetSpecular(0.3);
  this->arrowGlyphActor->GetProperty()->SetSpecularPower(10.0);
  this->arrowGlyphActor->SetMapper(this->arrowGlyphMapper);

  this->visible_ = false;
  this->scheme_ = 0;

  this->glyph_size_ = 1.0f;
  this->glyph_quality_ = 5.0f;
  this->update_glyph_properties();
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::set_color_scheme(int scheme)
{
  this->scheme_ = scheme;
  this->surface_actor_->GetProperty()->SetDiffuseColor(m_ColorSchemes[scheme].foreground.r,
                                                       m_ColorSchemes[scheme].foreground.g,
                                                       m_ColorSchemes[scheme].foreground.b);
  this->renderer_->SetBackground(m_ColorSchemes[scheme].background.r,
                                 m_ColorSchemes[scheme].background.g,
                                 m_ColorSchemes[scheme].background.b);
}

//-----------------------------------------------------------------------------
void Viewer::display_vector_field()
{
  qInfo() << "display_vector_field\n";
  std::vector<Point> vecs = this->object_->get_vectors();
  if (vecs.empty()) {
    return;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Step 1. Assign values at each correspondence point based on the image gradient
  /////////////////////////////////////////////////////////////////////////////////

  // Dot product difference vectors with the surface normals.
  vtkSmartPointer<vtkFloatArray> magnitudes = vtkSmartPointer<vtkFloatArray>::New();
  magnitudes->SetNumberOfComponents(1);

  vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
  vectors->SetNumberOfComponents(3);

  this->compute_point_differences(vecs, magnitudes, vectors);

  /////////////////////////////////////////////////////////////////////////////////
  // Step 2. Assign values at each mesh point based on the closest correspondence points
  this->computeSurfaceDifferences(magnitudes, vectors);
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  // Step 3. Assign the vectors and magnitudes to the glyphs and surface
  /////////////////////////////////////////////////////////////////////////////////

  // assign glyph vectors and magnitudes
  this->glyph_point_set_->GetPointData()->SetVectors(vectors);
  this->glyph_point_set_->GetPointData()->SetScalars(magnitudes);

  this->glyphs_->SetSourceConnection(this->arrowSource->GetOutputPort());
  this->glyphs_->SetScaleModeToScaleByVector();

  // update glyph rendering
  this->glyph_mapper_->SetLookupTable(this->differenceLUT);
  this->arrowGlyphMapper->SetLookupTable(this->differenceLUT);
  this->arrowsVisible = true;

  if (this->show_glyphs_) {
    this->renderer_->AddActor(this->arrowGlyphActor);
  }

  // update surface rendering
  /// TODO : multi-domain support
  //for (int i = 0; i < this->numDomains; i++) {
  this->surface_mapper_->SetLookupTable(this->differenceLUT);
  this->surface_mapper_->InterpolateScalarsBeforeMappingOn();
  this->surface_mapper_->SetColorModeToMapScalars();
  this->surface_mapper_->ScalarVisibilityOn();
  //}

  this->arrowsVisible = true;
}

//-----------------------------------------------------------------------------
void Viewer::compute_point_differences(const std::vector<Point> &vecs,
                                       vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors)
{
  qInfo() << "compute_point_difference\n";

  double minmag = 1.0e20;
  double maxmag = 0.0;

  /// TODO: multi-domain support
//  for (int domain = 0; domain < this->numDomains; domain++) {

  vtkSmartPointer<vtkPolyData> pointSet = this->glyph_point_set_;

  if (!this->object_->get_mesh()) {
    return;
  }

  vtkSmartPointer<vtkPolyData> poly_data = this->object_->get_mesh()->get_poly_data();
  if (!poly_data || poly_data->GetNumberOfPoints() == 0) {
    return;
  }

  std::cerr << "number of poly points = " << poly_data->GetNumberOfPoints() << "\n";


  vtkSmartPointer<CustomSurfaceReconstructionFilter> surfaceReconstruction =
    vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
  surfaceReconstruction->SetInputData(pointSet);
  //surfaceReconstruction->SetNeighborhoodSize(this->meshManager.getNeighborhoodSize());
  //surfaceReconstruction->SetSampleSpacing(this->meshManager.getSampleSpacing());
  surfaceReconstruction->Update();

  vtkImageGaussianSmooth* smoother = vtkImageGaussianSmooth::New();
  smoother->SetStandardDeviations(surfaceReconstruction->GetOutput()->GetSpacing()[0],
                                  surfaceReconstruction->GetOutput()->GetSpacing()[1],
                                  surfaceReconstruction->GetOutput()->GetSpacing()[2]);

  smoother->SetInputConnection(surfaceReconstruction->GetOutputPort());

  // Compute normals from the isosurface volume
  vtkSmartPointer<vtkImageGradient> grad = vtkSmartPointer<vtkImageGradient>::New();
  grad->SetDimensionality(3);
  grad->SetInputConnection(smoother->GetOutputPort());
  grad->Update();


  vnl_vector_fixed<double, 3> normal;

  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling
  // happens properly.

  //vtkDataArray* normals = poly_data->GetPointData()->GetNormals();
  //vtkFloatArray* normals = vtkFloatArray::SafeDownCast(poly_data->GetPointData()->GetNormals());

  //int num_normals = normals->GetNumberOfTuples();
  //std::cerr << "num_normals = " << num_normals << "\n";

  for (unsigned int i = 0; i < pointSet->GetNumberOfPoints(); i++) {
    double x = pointSet->GetPoint(i)[0];
    double y = pointSet->GetPoint(i)[1];
    double z = pointSet->GetPoint(i)[2];

    float xd = vecs[i].x;
    float yd = vecs[i].y;
    float zd = vecs[i].z;

    this->trilinearInterpolate(grad->GetOutput(), x, y, z, normal);

    float mag = xd * normal(0) + yd * normal(1) + zd * normal(2);

    if (mag < minmag) {minmag = mag; }
    if (mag > maxmag) {maxmag = mag; }

    vectors->InsertNextTuple3(normal(0) * mag, normal(1) * mag, normal(2) * mag);
    magnitudes->InsertNextTuple1(mag);
  }
  this->updateDifferenceLUT(minmag, maxmag);
}

//-----------------------------------------------------------------------------
void Viewer::computeSurfaceDifferences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors)
{
  qInfo() << "compute_surface_difference\n";

  vtkPolyData* polyData = this->surface_mapper_->GetInput();
  if (!polyData) {
    return;
  }

  vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
  pointData->SetPoints(this->glyph_points_);

  vtkPointLocator* pointLocator = vtkPointLocator::New();
  pointLocator->SetDataSet(pointData);
  pointLocator->SetDivisions(100, 100, 100);
  pointLocator->BuildLocator();

  /// TODO: multi-domain support
  //for (int domain = 0; domain < this->numDomains; domain++) {
  //vtkPolyData* polyData = this->surface_mapper_->GetInput();

  vtkFloatArray* surfaceMagnitudes = vtkFloatArray::New();
  surfaceMagnitudes->SetNumberOfComponents(1);
  surfaceMagnitudes->SetNumberOfTuples(polyData->GetPoints()->GetNumberOfPoints());

  vtkFloatArray* surfaceVectors = vtkFloatArray::New();
  surfaceVectors->SetNumberOfComponents(3);
  surfaceVectors->SetNumberOfTuples(polyData->GetPoints()->GetNumberOfPoints());

  for (unsigned int i = 0; i < surfaceMagnitudes->GetNumberOfTuples(); i++) {
    // find the 8 closest correspondence points the to current point
    vtkSmartPointer<vtkIdList> closestPoints = vtkSmartPointer<vtkIdList>::New();
    pointLocator->FindClosestNPoints(8, polyData->GetPoint(i), closestPoints);

    // assign scalar value based on a weighted scheme
    float weightedScalar = 0.0f;
    float distanceSum = 0.0f;
    float distance[8];
    for (unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++) {
      // get a particle position
      vtkIdType id = closestPoints->GetId(p);

      // compute distance to current particle
      double x = polyData->GetPoint(i)[0] - pointData->GetPoint(id)[0];
      double y = polyData->GetPoint(i)[1] - pointData->GetPoint(id)[1];
      double z = polyData->GetPoint(i)[2] - pointData->GetPoint(id)[2];
      distance[p] = 1.0f / (x * x + y * y + z * z);

      // multiply scalar value by weight and add to running sum
      distanceSum += distance[p];
    }

    float vecX = 0.0f;
    float vecY = 0.0f;
    float vecZ = 0.0f;

    for (unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++) {
      vtkIdType currID = closestPoints->GetId(p);
      weightedScalar += distance[p] / distanceSum * magnitudes->GetValue(currID);
      vecX += distance[p] / distanceSum * vectors->GetComponent(currID, 0);
      vecY += distance[p] / distanceSum * vectors->GetComponent(currID, 1);
      vecZ += distance[p] / distanceSum * vectors->GetComponent(currID, 2);
    }

    surfaceMagnitudes->SetValue(i, weightedScalar);

    surfaceVectors->SetComponent(i, 0, vecX);
    surfaceVectors->SetComponent(i, 1, vecY);
    surfaceVectors->SetComponent(i, 2, vecZ);
  }

  // surface coloring
  polyData->GetPointData()->SetScalars(surfaceMagnitudes);
  polyData->GetPointData()->SetVectors(surfaceVectors);
  //}
}

//-----------------------------------------------------------------------------
void Viewer::display_object(QSharedPointer<DisplayObject> object)
{
  this->visible_ = true;

  this->object_ = object;

  QSharedPointer<Mesh> mesh = object->get_mesh();

  vtkSmartPointer<vtkCornerAnnotation> corner_annotation =
    vtkSmartPointer<vtkCornerAnnotation>::New();
  corner_annotation->SetLinearFontScaleFactor(2);
  corner_annotation->SetNonlinearFontScaleFactor(1);
  corner_annotation->SetMaximumFontSize(16);

  QStringList annotations = object->get_annotations();

  corner_annotation->SetText(0, (annotations[0]).toStdString().c_str());
  corner_annotation->SetText(1, (annotations[1]).toStdString().c_str());
  corner_annotation->SetText(2, (annotations[2]).toStdString().c_str());
  corner_annotation->SetText(3, (annotations[3]).toStdString().c_str());

  corner_annotation->GetTextProperty()->SetColor(0.50, 0.5, 0.5);

  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  ren->RemoveAllViewProps();

  if (!mesh) {
    // display loading message
    corner_annotation->SetText(0, "Loading...");

    ren->AddViewProp(this->image_actor_);

    ren->ResetCamera();
    //this->renderer_->ResetCameraClippingRange();
  }
  else {

    vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
    vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mapper_;
    vtkSmartPointer<vtkActor> actor = this->surface_actor_;

    this->update_points();

    this->draw_exclusion_spheres(object);

    vnl_vector<double> transform = object->get_transform();

    if (transform.size() == 3) {
      double tx = -transform[0];
      double ty = -transform[1];
      double tz = -transform[2];

      vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
      translation->Translate(tx, ty, tz);

      vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transformFilter->SetInputData(poly_data);
      transformFilter->SetTransform(translation);
      transformFilter->Update();
      poly_data = transformFilter->GetOutput();
    }

    mapper->SetInputData(poly_data);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(m_ColorSchemes[this->scheme_].foreground.r,
                                          m_ColorSchemes[this->scheme_].foreground.g,
                                          m_ColorSchemes[this->scheme_].foreground.b);
    actor->GetProperty()->SetSpecular(0.2);
    actor->GetProperty()->SetSpecularPower(15);
    mapper->ScalarVisibilityOff();

    ren->RemoveAllViewProps();
    //ren->AddActor( actor );
    //ren->AddActor( this->glyph_actor_ );
  }

  ren->AddViewProp(corner_annotation);

  this->display_vector_field();
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
void Viewer::reset_camera(std::array<double, 3> c)
{
  this->renderer_->ResetCamera();

//  this->renderer_->GetActiveCamera()->SetViewUp(0, 1, 0);
//  this->renderer_->GetActiveCamera()->SetFocalPoint(0, 0, 0);
//  this->renderer_->GetActiveCamera()->SetPosition(c[0], c[1], c[2]);
}

//-----------------------------------------------------------------------------
void Viewer::set_renderer(vtkSmartPointer<vtkRenderer> renderer)
{
  this->renderer_ = renderer;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkRenderer> Viewer::get_renderer()
{
  return this->renderer_;
}

//-----------------------------------------------------------------------------
void Viewer::set_glyph_size_and_quality(double size, double quality)
{
  this->glyph_size_ = size;
  this->glyph_quality_ = quality;
  this->update_glyph_properties();
}

//-----------------------------------------------------------------------------
void Viewer::update_glyph_properties()
{
  //  std::cerr << "update glyph props\n";
  this->glyphs_->SetScaleFactor(this->glyph_size_);
  this->arrowGlyphs->SetScaleFactor(this->glyph_size_);

  this->sphere_source->SetThetaResolution(this->glyph_quality_);
  this->sphere_source->SetPhiResolution(this->glyph_quality_);

  this->arrowSource->SetTipResolution(this->glyph_quality_);
  this->arrowSource->SetShaftResolution(this->glyph_quality_);

  this->glyphs_->Update();
  this->arrowGlyphs->Update();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_glyphs(bool show)
{
  this->show_glyphs_ = show;
  this->update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_surface(bool show)
{
  this->show_surface_ = show;
  this->update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::update_points()
{
  if (!this->object_) {
    return;
  }
  vnl_vector<double> correspondence_points = this->object_->get_correspondence_points();

  int num_points = correspondence_points.size() / 3;

  vtkUnsignedLongArray* scalars =
    (vtkUnsignedLongArray*)(this->glyph_point_set_->GetPointData()->GetScalars());

  if (num_points > 0) {
    this->glyphs_->SetRange(0.0, (double)num_points + 1);
    this->glyph_mapper_->SetScalarRange(0.0, (double)num_points + 1.0);

    this->glyph_points_->Reset();
    this->glyph_points_->SetNumberOfPoints(num_points);

    scalars->Reset();
    scalars->SetNumberOfTuples(num_points);

    unsigned int idx = 0;
    for (int i = 0; i < num_points; i++) {
      scalars->InsertValue(i, i);
      double x = correspondence_points[idx++];
      double y = correspondence_points[idx++];
      double z = correspondence_points[idx++];

      this->glyph_points_->InsertPoint(i, x, y, z);
    }
  }
  else {
    this->glyph_points_->Reset();
    scalars->Reset();
  }
  this->glyph_points_->Modified();
}

//-----------------------------------------------------------------------------
void Viewer::update_actors()
{
  if (!this->visible_) {
    return;
  }

  this->renderer_->RemoveActor(this->glyph_actor_);
  //this->renderer_->RemoveActor( this->arrowGlyphActor );

  this->renderer_->RemoveActor(this->surface_actor_);

  /*
     for ( int i = 0; i < this->numDomains; i++ )
     {
     this->renderer->RemoveActor( this->surfaceActors[i] );
     }
   */

  if (this->show_glyphs_) {
    this->renderer_->AddActor(this->glyph_actor_);

    this->renderer_->AddActor(this->exclusion_sphere_actor_);
    /*    if ( this->arrowsVisible )
        {
        this->renderer->AddActor( this->arrowGlyphActor );
        }*/
  }

  if (this->show_surface_) {
    /*    for ( int i = 0; i < this->numDomains; i++ )
        {
        this->renderer->AddActor( this->surfaceActors[i] );
        }*/
    this->renderer_->AddActor(this->surface_actor_);
  }

  //this->displayShape( this->currentShape );

  //this->renderer_->Render();
}

//-----------------------------------------------------------------------------
int Viewer::handle_pick(int* click_pos)
{

  // First determine what was picked
  vtkSmartPointer<vtkPropPicker> prop_picker = vtkSmartPointer<vtkPropPicker>::New();
  prop_picker->Pick(click_pos[0], click_pos[1], 0, this->renderer_);
  if (prop_picker->GetActor() != this->glyph_actor_) {
    return -1;
  }

  vtkSmartPointer<vtkCellPicker> cell_picker = vtkSmartPointer<vtkCellPicker>::New();
  cell_picker->Pick(click_pos[0], click_pos[1], 0, this->renderer_);

  vtkDataArray* input_ids = this->glyphs_->GetOutput()->GetPointData()->GetArray("InputPointIds");

  if (input_ids) {
    vtkCell* cell = this->glyphs_->GetOutput()->GetCell(cell_picker->GetCellId());

    if (cell && cell->GetNumberOfPoints() > 0) {
      // get first PointId from picked cell
      vtkIdType input_id = cell->GetPointId(0);

      // get matching Id from "InputPointIds" array
      vtkIdType glyph_id = input_ids->GetTuple1(input_id);

      if (glyph_id >= 0) {
        std::cerr << "picked correspondence point :" << glyph_id << "\n";
        return glyph_id;
      }
    }
  }

  return -1;
}

//-----------------------------------------------------------------------------
void Viewer::set_lut(vtkSmartPointer<vtkLookupTable> lut)
{
  this->lut_ = lut;
  this->glyph_mapper_->SetLookupTable(this->lut_);
}

//-----------------------------------------------------------------------------
void Viewer::set_loading_screen(vtkSmartPointer<vtkImageData> loading_screen)
{
  this->image_actor_->SetInputData(loading_screen);
}

//-----------------------------------------------------------------------------
void Viewer::draw_exclusion_spheres(QSharedPointer<DisplayObject> object)
{
  QList<Point> centers = object->get_exclusion_sphere_centers();
  QList<double> radii = object->get_exclusion_sphere_radii();

  int num_points = centers.size();

  if (num_points <= 0) {
    this->exclusion_sphere_points_->Reset();
  }
  else {

    vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*)(
      this->exclusion_sphere_point_set_->GetPointData()->GetScalars());
    scalars->Reset();
    scalars->SetNumberOfTuples(num_points);

    this->exclusion_sphere_glyph_->SetRange(0.0, (double)num_points + 1);
    this->exclusion_sphere_mapper_->SetScalarRange(0.0, (double)num_points + 1.0);

    this->exclusion_sphere_points_->Reset();
    this->exclusion_sphere_points_->SetNumberOfPoints(num_points);

    for (int i = 0; i < num_points; i++) {
      Point p = centers[i];
      scalars->InsertValue(i, radii[i]);
      this->exclusion_sphere_points_->InsertPoint(i, p.x, p.y, p.z);
    }
  }

  this->exclusion_sphere_points_->Modified();
}

//-----------------------------------------------------------------------------
void Viewer::trilinearInterpolate(vtkImageData* grad, double x, double y, double z,
                                  vnl_vector_fixed<double, 3> &ans) const
{
  // Access gradient image information.
  const double* gradData = (const double*)(grad->GetScalarPointer());
  //const double* spacing = grad->GetSpacing();

  // See, e.g. http://en.wikipedia.org/wiki/Trilinear_interpolation for description
  // Identify the surrounding 8 points (corners).  c is the closest grid point.
  vtkIdType idx = grad->FindPoint(x, y, z);
  const double* c = grad->GetPoint(idx);

  //std::cout << "idx = " << idx << std::endl;
  //std::cout << "c = " << c[0] << " " << c[1] << " " << c[2] << std::endl;

  ans[0] = gradData[idx * 3];
  ans[1] = gradData[idx * 3 + 1];
  ans[2] = gradData[idx * 3 + 2];
  return;
}

//-----------------------------------------------------------------------------
void Viewer::updateDifferenceLUT(float r0, float r1)
{

  double black[3] = { 0.0, 0.0, 0.0 };
  double white[3] = { 1.0, 1.0, 1.0 };
  double red[3] = { 1.0, 0.3, 0.3 };
  double red_pure[3] = { 1.0, 0.0, 0.0 };
  double green[3] = { 0.3, 1.0, 0.3 };
  double green_pure[3] = { 0.0, 1.0, 0.0 };
  double blue[3] = { 0.3, 0.3, 1.0 };
  double blue_pure[3] = { 0.0, 0.0, 1.0 };
  double yellow[3] = { 1.0, 1.0, 0.3 };
  double yellow_pure[3] = { 1.0, 1.0, 0.0 };
  double magenta[3] = { 1.0, 0.3, 1.0 };
  double cyan[3] = { 0.3, 1.0, 1.0 };
  double orange[3] = { 1.0, 0.5, 0.0 };
  double violet[3] = { 2.0 / 3.0, 0.0, 1.0 };

  this->differenceLUT->RemoveAllPoints();

  //const float yellow = 0.86666;
  //const float blue = 0.66666;
  //const float yellow = 0.33;
  //const float blue = 0.66;
  const unsigned int resolution = 100;
  const float resinv = 1.0 / static_cast<float>(resolution);
  float maxrange;
  if (fabs(r0) > fabs(r1)) {maxrange = fabs(r0); }
  else {maxrange = fabs(r1); }

  std::cerr << "r0 = " << r0 << "\n";
  std::cerr << "r1 = " << r1 << "\n";
  std::cerr << "maxrange = " << maxrange << "\n";
  //this->differenceLUT->SetScalarRange(-maxrange, maxrange);

  //this->differenceLUT->SetColorSpaceToHSV();
  //this->differenceLUT->AddHSVPoint(-maxrange, 0.33, 1.0, 1.0);
  //this->differenceLUT->AddHSVPoint(maxrange, 0.66, 1.0, 1.0);

  double rd = r1 - r0;

  this->differenceLUT->SetColorSpaceToHSV();
  this->differenceLUT->AddRGBPoint(r0, blue[0], blue[1], blue[2]);
  this->differenceLUT->AddRGBPoint(r0 + rd * 0.5, green[0], green[1], green[2]);
  this->differenceLUT->AddRGBPoint(r1, red[0], red[1], red[2]);

  return;

  const float pip = fabs(maxrange) * resinv;
  for (unsigned int i = 0; i < resolution; i++) {
    float fi = static_cast<float>(i);

    //this->differenceLUT->AddHSVPoint( -maxrange + ( fi * pip ), yellow, 1.0 - ( fi * resinv ), 1.0 );
    //this->differenceLUT->AddHSVPoint( maxrange - ( fi * pip ), blue, 1.0 - ( fi * resinv ), 1.0 );
  }

  this->differenceLUT->AddHSVPoint(0.0, 0.0, 0.0, 1.0);
}
