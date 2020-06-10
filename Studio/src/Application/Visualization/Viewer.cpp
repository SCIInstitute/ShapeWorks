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
#include <vtkKdTreePointLocator.h>
#include <vtkScalarBarActor.h>

#include <Application/Data/CustomSurfaceReconstructionFilter.h>
#include <Data/Preferences.h>
#include <Data/Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Viewer.h>
#include <Visualization/Visualizer.h>

//-----------------------------------------------------------------------------
Viewer::Viewer()
{
  //this->image_actor_ = vtkSmartPointer<vtkImageActor>::New();

  this->surface_actor_ = vtkSmartPointer<vtkActor>::New();
  this->surface_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();

  this->sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();

  this->difference_lut_ = vtkSmartPointer<vtkColorTransferFunction>::New();
  this->difference_lut_->SetColorSpaceToHSV();

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
  this->glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
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
  this->exclusion_sphere_glyph_->SetSourceConnection(sphere_source_->GetOutputPort());
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

  this->arrow_flip_filter_ = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->arrow_glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  this->arrow_glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->arrow_glyph_actor_ = vtkSmartPointer<vtkActor>::New();

  // Arrow glyphs
  this->arrow_source_ = vtkSmartPointer<vtkArrowSource>::New();
  this->arrow_source_->SetTipResolution(6);
  this->arrow_source_->SetShaftResolution(6);

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
  this->transform_180_ = vtkSmartPointer<vtkTransform>::New();
  this->transform_180_->Concatenate(t1);

  this->arrow_flip_filter_->SetTransform(this->transform_180_);
  this->arrow_flip_filter_->SetInputConnection(this->arrow_source_->GetOutputPort());

  this->arrow_glyphs_->SetSourceConnection(this->arrow_flip_filter_->GetOutputPort());
  this->arrow_glyphs_->SetInputData(this->glyph_point_set_);
  this->arrow_glyphs_->ScalingOn();
  this->arrow_glyphs_->ClampingOff();

  this->arrow_glyphs_->SetVectorModeToUseVector();
  this->arrow_glyphs_->SetScaleModeToScaleByVector();

  this->arrow_glyph_mapper_->SetInputConnection(this->arrow_glyphs_->GetOutputPort());

  this->arrow_glyph_actor_->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  this->arrow_glyph_actor_->GetProperty()->SetDiffuse(0.8);
  this->arrow_glyph_actor_->GetProperty()->SetSpecular(0.3);
  this->arrow_glyph_actor_->GetProperty()->SetSpecularPower(10.0);
  this->arrow_glyph_actor_->SetMapper(this->arrow_glyph_mapper_);

  this->arrow_glyphs_->SetColorModeToColorByScalar();
  this->arrow_glyph_mapper_->SetColorModeToMapScalars();
  this->arrow_glyph_mapper_->ScalarVisibilityOn();

  this->visible_ = false;
  this->scheme_ = 0;

  this->glyph_size_ = 1.0f;
  this->glyph_quality_ = 5.0f;
  this->update_glyph_properties();

  this->scalar_bar_actor_ = vtkSmartPointer<vtkScalarBarActor>::New();
  this->scalar_bar_actor_->SetTitle("");
  this->scalar_bar_actor_->SetLookupTable(this->difference_lut_);
  this->scalar_bar_actor_->SetOrientationToHorizontal();
  this->scalar_bar_actor_->SetMaximumNumberOfColors(1000);
  this->scalar_bar_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->scalar_bar_actor_->GetPositionCoordinate()->SetValue(.2, .05);
  this->scalar_bar_actor_->SetWidth(0.8);
  this->scalar_bar_actor_->SetHeight(0.05);
  this->scalar_bar_actor_->SetPosition(0.1, 0.01);
  this->scalar_bar_actor_->SetLabelFormat("%.0f");
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontSize(4);
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontSize(4);
  this->scalar_bar_actor_->GetLabelTextProperty()->SetJustificationToCentered();
  this->scalar_bar_actor_->GetLabelTextProperty()->SetColor(1, 1, 1);

  this->corner_annotation_ = vtkSmartPointer<vtkCornerAnnotation>::New();
  corner_annotation_->SetLinearFontScaleFactor(2);
  corner_annotation_->SetNonlinearFontScaleFactor(1);
  corner_annotation_->SetMaximumFontSize(16);
}

//-----------------------------------------------------------------------------
Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::set_color_scheme(int scheme)
{
  this->scheme_ = scheme;
  this->surface_actor_->GetProperty()->SetDiffuseColor(color_schemes_[scheme].foreground.r,
                                                       color_schemes_[scheme].foreground.g,
                                                       color_schemes_[scheme].foreground.b);
  this->renderer_->SetBackground(color_schemes_[scheme].background.r,
                                 color_schemes_[scheme].background.g,
                                 color_schemes_[scheme].background.b);
}

//-----------------------------------------------------------------------------
void Viewer::handle_new_mesh()
{
  if (!this->mesh_ready_ && this->shape_ &&
      this->shape_->get_mesh(this->visualizer_->get_display_mode())) {
    this->display_shape(this->shape_);
  }
}

//-----------------------------------------------------------------------------
bool Viewer::is_viewer_ready()
{
  return this->viewer_ready_;
}

//-----------------------------------------------------------------------------
void Viewer::set_visualizer(Visualizer* visualizer)
{
  this->visualizer_ = visualizer;
}

//-----------------------------------------------------------------------------
void Viewer::display_vector_field()
{
  std::vector<Point> vecs = this->shape_->get_vectors();
  if (vecs.empty()) {
    // restore things to normal
    this->glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
    this->glyphs_->ScalingOn();
    this->glyphs_->ClampingOff();
    this->glyphs_->SetScaleModeToDataScalingOff();
    this->glyph_mapper_->SetLookupTable(this->lut_);

    this->glyph_points_->SetDataTypeToDouble();
    this->glyph_point_set_->SetPoints(this->glyph_points_);
    this->glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkUnsignedLongArray>::New());

    this->glyphs_->SetInputData(this->glyph_point_set_);
    this->glyphs_->ScalingOn();
    this->glyphs_->ClampingOff();
    this->glyphs_->SetScaleModeToDataScalingOff();
    this->glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
    this->glyphs_->GeneratePointIdsOn();

    this->update_points();
    this->arrows_visible_ = false;
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
  /////////////////////////////////////////////////////////////////////////////////
  this->compute_surface_differences(magnitudes, vectors);

  /////////////////////////////////////////////////////////////////////////////////
  // Step 3. Assign the vectors and magnitudes to the glyphs and surface
  /////////////////////////////////////////////////////////////////////////////////

  // assign glyph vectors and magnitudes
  this->glyph_point_set_->GetPointData()->SetVectors(vectors);
  this->glyph_point_set_->GetPointData()->SetScalars(magnitudes);

  this->glyphs_->SetSourceConnection(this->arrow_source_->GetOutputPort());
  this->glyphs_->SetScaleModeToScaleByVector();

  // update glyph rendering
  this->glyph_mapper_->SetLookupTable(this->difference_lut_);
  this->arrow_glyph_mapper_->SetLookupTable(this->difference_lut_);

  // update surface rendering
  /// TODO : multi-domain support
  //for (int i = 0; i < this->numDomains; i++) {
  this->surface_mapper_->SetLookupTable(this->difference_lut_);
  this->surface_mapper_->InterpolateScalarsBeforeMappingOn();
  this->surface_mapper_->SetColorModeToMapScalars();
  this->surface_mapper_->ScalarVisibilityOn();
  //}

  // Set the color modes
  this->arrow_glyphs_->SetColorModeToColorByScalar();
  this->glyphs_->SetColorModeToColorByScalar();
  this->glyph_mapper_->SetColorModeToMapScalars();
  this->glyph_mapper_->ScalarVisibilityOn();
  this->arrow_glyph_mapper_->SetColorModeToMapScalars();
  this->arrow_glyph_mapper_->ScalarVisibilityOn();

  this->arrows_visible_ = true;
}

//-----------------------------------------------------------------------------
void Viewer::compute_point_differences(const std::vector<Point> &vecs,
                                       vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors)
{
  double minmag = 1.0e20;
  double maxmag = 0.0;

  /// TODO: multi-domain support
//  for (int domain = 0; domain < this->numDomains; domain++) {

  vtkSmartPointer<vtkPolyData> pointSet = this->glyph_point_set_;

  if (!this->shape_->get_mesh(this->visualizer_->get_display_mode())) {
    return;
  }

  vtkSmartPointer<vtkPolyData> poly_data = this->shape_->get_mesh(
    this->visualizer_->get_display_mode())->get_poly_data();
  if (!poly_data || poly_data->GetNumberOfPoints() == 0) {
    return;
  }

  vtkSmartPointer<vtkPolyDataNormals> polydata_normals =
    vtkSmartPointer<vtkPolyDataNormals>::New();
  polydata_normals->SetInputData(poly_data);
  polydata_normals->Update();
  polydata_normals->SetSplitting(false); // must be sure not to split normals
  poly_data = polydata_normals->GetOutput();

  vtkSmartPointer<vtkKdTreePointLocator> locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  locator->SetDataSet(poly_data);
  locator->BuildLocator();

  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling
  // happens properly.
  for (unsigned int i = 0; i < pointSet->GetNumberOfPoints(); i++) {

    auto id = locator->FindClosestPoint(pointSet->GetPoint(i));
    double* normal = poly_data->GetPointData()->GetNormals()->GetTuple(id);

    float xd = vecs[i].x;
    float yd = vecs[i].y;
    float zd = vecs[i].z;

    float mag = xd * normal[0] + yd * normal[1] + zd * normal[2];

    if (mag < minmag) {minmag = mag; }
    if (mag > maxmag) {maxmag = mag; }

    vectors->InsertNextTuple3(normal[0] * mag, normal[1] * mag, normal[2] * mag);
    magnitudes->InsertNextTuple1(mag);
  }
  this->updateDifferenceLUT(minmag, maxmag);
}

//-----------------------------------------------------------------------------
void Viewer::compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                         vtkSmartPointer<vtkFloatArray> vectors)
{
  vtkPolyData* polyData = this->surface_mapper_->GetInput();
  if (!polyData) {
    return;
  }

  vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
  point_data->SetPoints(this->glyph_points_);

  vtkPointLocator* point_locator = vtkPointLocator::New();
  point_locator->SetDataSet(point_data);
  point_locator->SetDivisions(100, 100, 100);
  point_locator->BuildLocator();

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
    point_locator->FindClosestNPoints(8, polyData->GetPoint(i), closestPoints);

    // assign scalar value based on a weighted scheme
    float weightedScalar = 0.0f;
    float distanceSum = 0.0f;
    float distance[8];
    for (unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++) {
      // get a particle position
      vtkIdType id = closestPoints->GetId(p);

      // compute distance to current particle
      double x = polyData->GetPoint(i)[0] - point_data->GetPoint(id)[0];
      double y = polyData->GetPoint(i)[1] - point_data->GetPoint(id)[1];
      double z = polyData->GetPoint(i)[2] - point_data->GetPoint(id)[2];
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

    //std::cerr << "scalar = " << weightedScalar << "\n";
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
void Viewer::display_shape(QSharedPointer<Shape> shape)
{
  this->visible_ = true;

  this->shape_ = shape;

  //std::cerr << "asking for mesh\n";
  this->mesh_ = shape->get_mesh(this->visualizer_->get_display_mode());

  if (!this->mesh_ && this->loading_displayed_) {
    // no need to proceed
    this->mesh_ready_ = false;
    //return;
  }

  //QSharedPointer<Mesh> mesh;

  QStringList annotations = shape->get_annotations();
  this->corner_annotation_->SetText(0, (annotations[0]).toStdString().c_str());
  this->corner_annotation_->SetText(1, (annotations[1]).toStdString().c_str());
  this->corner_annotation_->SetText(2, (annotations[2]).toStdString().c_str());
  this->corner_annotation_->SetText(3, (annotations[3]).toStdString().c_str());
  this->corner_annotation_->GetTextProperty()->SetColor(0.50, 0.5, 0.5);

  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  ren->RemoveAllViewProps();

  if (!this->mesh_) {
    this->mesh_ready_ = false;
    this->viewer_ready_ = false;
    // display loading message
    //corner_annotation->SetText(0, "Loading...");
    ///this->corner_annotation_->SetText(2, "Loading...");
    //corner_annotation->SetText(2, "Loading...");

    //ren->AddViewProp(this->image_actor_);

    //ren->ResetCamera();
    //this->renderer_->ResetCameraClippingRange();
    this->loading_displayed_ = true;

    this->update_points();

  }
  else {
    //this->corner_annotation_->SetText(3, "Ready...");
    //corner_annotation->SetText(0, "Ready...");
    //corner_annotation->SetText(2, "Ready...");
    //corner_annotation->SetText(2, "Ready...");
    //corner_annotation->SetText(3, "Ready...");
    this->loading_displayed_ = false;
    this->viewer_ready_ = true;
    //std::cerr << "mesh is ready!\n";

    vtkSmartPointer<vtkPolyData> poly_data = this->mesh_->get_poly_data();
    vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mapper_;
    vtkSmartPointer<vtkActor> actor = this->surface_actor_;

    this->update_points();

    this->draw_exclusion_spheres(shape);

    vnl_vector<double> transform;
    if (this->visualizer_->get_display_mode() == Visualizer::MODE_ORIGINAL_C) {
      transform = shape->get_transform();
    }

    if (transform.size() == 3) {
      double tx = -transform[0];
      double ty = -transform[1];
      double tz = -transform[2];

      vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
      translation->Translate(tx, ty, tz);

      vtkSmartPointer<vtkTransformPolyDataFilter> transform_filter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transform_filter->SetInputData(poly_data);
      transform_filter->SetTransform(translation);
      transform_filter->Update();
      poly_data = transform_filter->GetOutput();
    }

    mapper->SetInputData(poly_data);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(color_schemes_[this->scheme_].foreground.r,
                                          color_schemes_[this->scheme_].foreground.g,
                                          color_schemes_[this->scheme_].foreground.b);
    actor->GetProperty()->SetSpecular(0.2);
    actor->GetProperty()->SetSpecularPower(15);
    mapper->ScalarVisibilityOff();

    //ren->AddActor( actor );
    //ren->AddActor( this->glyph_actor_ );

    this->display_vector_field();


  }

  this->update_actors();
  this->update_glyph_properties();

  ///this->update_actors();
  ///this->update_glyph_properties();

  //this->renderer_->ResetCameraClippingRange();

  ren->AddViewProp(this->corner_annotation_);
}

//-----------------------------------------------------------------------------
void Viewer::clear_viewer()
{
  this->renderer_->RemoveAllViewProps();
  this->visible_ = false;
  this->viewer_ready_ = false;
  this->mesh_ready_ = false;
  this->loading_displayed_ = false;
}

//-----------------------------------------------------------------------------
void Viewer::reset_camera(std::array<double, 3> c)
{
  //this->renderer_->ResetCamera();

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
  this->arrow_glyphs_->SetScaleFactor(this->glyph_size_);

  this->sphere_source_->SetThetaResolution(this->glyph_quality_);
  this->sphere_source_->SetPhiResolution(this->glyph_quality_);

  this->arrow_source_->SetTipResolution(this->glyph_quality_);
  this->arrow_source_->SetShaftResolution(this->glyph_quality_);

  this->glyphs_->Update();
  this->arrow_glyphs_->Update();
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
  if (!this->shape_) {
    return;
  }
  vnl_vector<double> correspondence_points = this->shape_->get_global_correspondence_points();

  int num_points = correspondence_points.size() / 3;

  vtkUnsignedLongArray* scalars =
    (vtkUnsignedLongArray*)(this->glyph_point_set_->GetPointData()->GetScalars());

  if (num_points > 0) {
    this->viewer_ready_ = true;
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
  this->renderer_->RemoveActor(this->arrow_glyph_actor_);
  this->renderer_->RemoveActor(this->surface_actor_);
  this->renderer_->RemoveActor(this->scalar_bar_actor_);

  /*
     for ( int i = 0; i < this->numDomains; i++ )
     {
     this->renderer->RemoveActor( this->surfaceActors[i] );
     }
   */

  if (this->show_glyphs_) {
    this->renderer_->AddActor(this->glyph_actor_);

    this->renderer_->AddActor(this->exclusion_sphere_actor_);
    if (this->arrows_visible_) {
      this->renderer_->AddActor(this->arrow_glyph_actor_);
      this->renderer_->AddActor(this->scalar_bar_actor_);
    }
  }

  if (this->show_surface_ && this->mesh_) {
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
  if (!this->arrows_visible_) {
    this->glyph_mapper_->SetLookupTable(this->lut_);
  }
}

//-----------------------------------------------------------------------------
void Viewer::set_loading_screen(vtkSmartPointer<vtkImageData> loading_screen)
{
  //this->image_actor_->SetInputData(loading_screen);
}

//-----------------------------------------------------------------------------
void Viewer::draw_exclusion_spheres(QSharedPointer<Shape> object)
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

  this->difference_lut_->RemoveAllPoints();

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

  this->difference_lut_->SetColorSpaceToHSV();
  this->difference_lut_->AddRGBPoint(r0, blue[0], blue[1], blue[2]);
  this->difference_lut_->AddRGBPoint(r0 + rd * 0.5, green[0], green[1], green[2]);
  this->difference_lut_->AddRGBPoint(r1, red[0], red[1], red[2]);

  return;
}
