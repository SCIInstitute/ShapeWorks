#include <vtkPointData.h>
#include <vtkUnsignedLongArray.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkCell.h>
#include <vtkFloatArray.h>
#include <vtkArrowSource.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataNormals.h>
#include <vtkKdTreePointLocator.h>
#include <vtkScalarBarActor.h>
#include <vtkReverseSense.h>

#include <Data/Preferences.h>
#include <Data/Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Viewer.h>
#include <Visualization/Visualizer.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
Viewer::Viewer()
{

  this->sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();
  this->reverse_sphere_ = vtkSmartPointer<vtkReverseSense>::New();
  this->reverse_sphere_->SetInputConnection(this->sphere_source_->GetOutputPort());
  this->reverse_sphere_->ReverseNormalsOff();
  this->reverse_sphere_->ReverseCellsOn();

  this->surface_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  this->surface_lut_->SetTableRange(0, 1);
  this->surface_lut_->SetHueRange(0.667, 0.0);
  this->surface_lut_->SetSaturationRange(1, 1);
  this->surface_lut_->SetValueRange(1, 1);
  this->surface_lut_->SetIndexedLookup(false);
  this->surface_lut_->Build();

  this->glyph_points_ = vtkSmartPointer<vtkPoints>::New();
  this->glyph_points_->SetDataTypeToDouble();
  this->glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  this->glyph_point_set_->SetPoints(this->glyph_points_);
  this->glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkFloatArray>::New());

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
  this->exclusion_sphere_mapper_->SetInputConnection(
    this->exclusion_sphere_glyph_->GetOutputPort());
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

  this->update_glyph_properties();

  this->scalar_bar_actor_ = vtkSmartPointer<vtkScalarBarActor>::New();
  this->scalar_bar_actor_->SetTitle("");
  this->scalar_bar_actor_->SetLookupTable(this->surface_lut_);
  //this->scalar_bar_actor_->SetOrientationToHorizontal();
  this->scalar_bar_actor_->SetOrientationToVertical();
  this->scalar_bar_actor_->SetMaximumNumberOfColors(1000);
  this->scalar_bar_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->scalar_bar_actor_->GetPositionCoordinate()->SetValue(.3, .2);
  this->scalar_bar_actor_->SetWidth(0.10);
  this->scalar_bar_actor_->SetHeight(0.7);
  this->scalar_bar_actor_->SetPosition(0.9, 0.1);
  this->scalar_bar_actor_->SetLabelFormat("%.1f");
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontSize(4);
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontSize(4);
  this->scalar_bar_actor_->GetLabelTextProperty()->SetJustificationToCentered();
  this->scalar_bar_actor_->GetLabelTextProperty()->SetColor(1, 1, 1);

  this->corner_annotation_ = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->corner_annotation_->SetLinearFontScaleFactor(2);
  this->corner_annotation_->SetNonlinearFontScaleFactor(1);
  this->corner_annotation_->SetMaximumFontSize(32);
  this->corner_annotation_->SetMaximumLineHeight(0.03);
}

//-----------------------------------------------------------------------------
void Viewer::set_color_scheme(int scheme)
{
  this->scheme_ = scheme;

  for (size_t i = 0; i < this->surface_actors_.size(); i++) {
    int scheme = (this->scheme_ + i) % this->color_schemes_.size();

    this->surface_actors_[i]->GetProperty()->SetDiffuseColor(color_schemes_[scheme].foreground.r,
                                                             color_schemes_[scheme].foreground.g,
                                                             color_schemes_[scheme].foreground.b);
  }

  this->renderer_->SetBackground(color_schemes_[scheme].background.r,
                                 color_schemes_[scheme].background.g,
                                 color_schemes_[scheme].background.b);

  double average = (color_schemes_[scheme].background.r + color_schemes_[scheme].background.g +
                    color_schemes_[scheme].background.b) / 3.0;

  double color = 1;
  if (average > 0.5) {
    color = 0;
  }

  this->scalar_bar_actor_->GetLabelTextProperty()->SetColor(color, color, color);
}

//-----------------------------------------------------------------------------
void Viewer::handle_new_mesh()
{
  if (!this->mesh_ready_ && this->shape_ &&
      this->shape_->get_meshes(this->visualizer_->get_display_mode()).valid()) {
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
  std::vector<Shape::Point> vecs = this->shape_->get_vectors();
  if (vecs.empty()) {
    // restore things to normal
    this->glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
    this->glyphs_->ScalingOn();
    this->glyphs_->ClampingOff();
    this->glyphs_->SetScaleModeToDataScalingOff();
    this->glyph_mapper_->SetLookupTable(this->lut_);

    this->glyph_points_->SetDataTypeToDouble();
    this->glyph_point_set_->SetPoints(this->glyph_points_);
    this->glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkFloatArray>::New());

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

  this->update_points();
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
  this->glyph_mapper_->SetLookupTable(this->surface_lut_);
  this->arrow_glyph_mapper_->SetLookupTable(this->surface_lut_);

  // update surface rendering
  for (size_t i = 0; i < this->surface_mappers_.size(); i++) {
    this->surface_mappers_[i]->SetLookupTable(this->surface_lut_);
    this->surface_mappers_[i]->InterpolateScalarsBeforeMappingOn();
    this->surface_mappers_[i]->SetColorModeToMapScalars();
    this->surface_mappers_[i]->ScalarVisibilityOn();
  }

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
void Viewer::compute_point_differences(const std::vector<Shape::Point>& points,
                                       vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors)
{
  auto mesh_group = this->shape_->get_meshes(this->visualizer_->get_display_mode());
  if (!mesh_group.valid() || points.empty()) {
    return;
  }

  vtkSmartPointer<vtkPolyData> point_set = this->glyph_point_set_;
  std::vector<vtkSmartPointer<vtkPolyData>> polys;
  std::vector<vtkSmartPointer<vtkKdTreePointLocator>> locators;

  for (size_t i = 0; i < mesh_group.meshes().size(); i++) {
    vtkSmartPointer<vtkPolyData> poly_data = mesh_group.meshes()[i]->get_poly_data();

    auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputData(poly_data);
    normals->Update();
    normals->SetSplitting(false); // must be sure not to split normals
    poly_data = normals->GetOutput();
    polys.push_back(poly_data);

    auto locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    locator->SetDataSet(poly_data);
    locator->BuildLocator();
    locators.push_back(locator);
  }

  double minmag = std::numeric_limits<double>::max();
  double maxmag = std::numeric_limits<double>::min();

  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling
  // happens properly.
  for (unsigned int i = 0; i < point_set->GetNumberOfPoints(); i++) {

    int domain = this->shape_->get_particles().get_domain_for_combined_id(i);

    auto id = locators[domain]->FindClosestPoint(point_set->GetPoint(i));
    double* normal = polys[domain]->GetPointData()->GetNormals()->GetTuple(id);

    float xd = points[i].x;
    float yd = points[i].y;
    float zd = points[i].z;

    float mag = xd * normal[0] + yd * normal[1] + zd * normal[2];
    if (std::isnan(mag)) {
      mag = 0;
    }
    if (mag < minmag) { minmag = mag; }
    if (mag > maxmag) { maxmag = mag; }

    vectors->InsertNextTuple3(normal[0] * mag, normal[1] * mag, normal[2] * mag);
    magnitudes->InsertNextTuple1(mag);
  }
  this->update_difference_lut(minmag, maxmag);
}

//-----------------------------------------------------------------------------
void Viewer::compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                         vtkSmartPointer<vtkFloatArray> vectors)
{
  if (!this->mesh_ready_) {
    return;
  }

  for (size_t i = 0; i < this->surface_mappers_.size(); i++) {

    vtkPolyData* poly_data = this->surface_mappers_[i]->GetInput();
    if (!poly_data || poly_data->GetNumberOfPoints() < 0) {
      return;
    }

    vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
    point_data->SetPoints(this->glyph_points_);

    vtkPointLocator* point_locator = vtkPointLocator::New();
    point_locator->SetDataSet(point_data);
    point_locator->SetDivisions(100, 100, 100);
    point_locator->BuildLocator();

    vtkFloatArray* surface_magnitudes = vtkFloatArray::New();
    surface_magnitudes->SetName("surface_difference");
    surface_magnitudes->SetNumberOfComponents(1);
    surface_magnitudes->SetNumberOfTuples(poly_data->GetPoints()->GetNumberOfPoints());

    vtkFloatArray* surface_vectors = vtkFloatArray::New();
    surface_vectors->SetNumberOfComponents(3);
    surface_vectors->SetName("surface_vectors");
    surface_vectors->SetNumberOfTuples(poly_data->GetPoints()->GetNumberOfPoints());

    for (unsigned int i = 0; i < surface_magnitudes->GetNumberOfTuples(); i++) {
      // find the 8 closest correspondence points the to current point
      vtkSmartPointer<vtkIdList> closest_points = vtkSmartPointer<vtkIdList>::New();
      point_locator->FindClosestNPoints(8, poly_data->GetPoint(i), closest_points);
      // assign scalar value based on a weighted scheme
      float weighted_scalar = 0.0f;
      float distance_sum = 0.0f;
      float distance[8];

      bool exactly_on_point = false;
      float exact_scalar = 0.0f;

      for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
        // get a particle position
        vtkIdType id = closest_points->GetId(p);

        // compute distance to current particle
        double x = poly_data->GetPoint(i)[0] - point_data->GetPoint(id)[0];
        double y = poly_data->GetPoint(i)[1] - point_data->GetPoint(id)[1];
        double z = poly_data->GetPoint(i)[2] - point_data->GetPoint(id)[2];

        if (x == 0 && y == 0 && z == 0) {
          distance[p] = 0;
          exactly_on_point = true;
          exact_scalar = magnitudes->GetValue(id);
        }
        else {
          distance[p] = 1.0f / (x * x + y * y + z * z);
        }

        // multiply scalar value by weight and add to running sum
        distance_sum += distance[p];
      }

      float vecX = 0.0f;
      float vecY = 0.0f;
      float vecZ = 0.0f;

      for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
        vtkIdType currID = closest_points->GetId(p);
        weighted_scalar += distance[p] / distance_sum * magnitudes->GetValue(currID);
        vecX += distance[p] / distance_sum * vectors->GetComponent(currID, 0);
        vecY += distance[p] / distance_sum * vectors->GetComponent(currID, 1);
        vecZ += distance[p] / distance_sum * vectors->GetComponent(currID, 2);
      }

      if (exactly_on_point) {
        weighted_scalar = exact_scalar;
      }

      surface_magnitudes->SetValue(i, weighted_scalar);

      surface_vectors->SetComponent(i, 0, vecX);
      surface_vectors->SetComponent(i, 1, vecY);
      surface_vectors->SetComponent(i, 2, vecZ);
    }

    // surface coloring
    poly_data->GetPointData()->SetScalars(surface_magnitudes);
    poly_data->GetPointData()->SetVectors(surface_vectors);
  }
}

//-----------------------------------------------------------------------------
std::string Viewer::get_displayed_feature_map()
{
  auto feature_map = this->visualizer_->get_feature_map();

  if (this->shape_->get_override_feature() != "") {
    feature_map = this->shape_->get_override_feature();
  }
  return feature_map;
}

//-----------------------------------------------------------------------------
void Viewer::display_shape(QSharedPointer<Shape> shape)
{

  this->visible_ = true;

  this->shape_ = shape;

  this->meshes_ = shape->get_meshes(this->visualizer_->get_display_mode());

  if (!this->meshes_.valid() && this->loading_displayed_) {
    // no need to proceed
    this->mesh_ready_ = false;
    return;
  }
  this->mesh_ready_ = true;

  QStringList annotations = shape->get_annotations();
  this->corner_annotation_->SetText(0, (annotations[0]).toStdString().c_str());
  this->corner_annotation_->SetText(1, (annotations[1]).toStdString().c_str());
  this->corner_annotation_->SetText(2, (annotations[2]).toStdString().c_str());
  this->corner_annotation_->SetText(3, (annotations[3]).toStdString().c_str());
  this->corner_annotation_->GetTextProperty()->SetColor(0.50, 0.5, 0.5);

  vtkSmartPointer<vtkRenderer> ren = this->renderer_;

  ren->RemoveAllViewProps();

  if (!this->meshes_.valid()) {
    this->mesh_ready_ = false;
    this->viewer_ready_ = false;
    this->loading_displayed_ = true;
    this->update_points();
  }
  else {
    this->loading_displayed_ = false;
    this->viewer_ready_ = true;

    this->number_of_domains_ = this->meshes_.meshes().size();
    this->initialize_surfaces();

    for (size_t i = 0; i < this->meshes_.meshes().size(); i++) {

      MeshHandle mesh = this->meshes_.meshes()[i];

      vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

      auto feature_map = this->get_displayed_feature_map();

      std::vector<Shape::Point> vecs = this->shape_->get_vectors();
      if (!vecs.empty()) {
        feature_map = "";
      }

      if (feature_map != "" && poly_data) {
        auto scalar_array = poly_data->GetPointData()->GetArray(feature_map.c_str());
        if (!scalar_array) {
          this->shape_->load_feature(this->visualizer_->get_display_mode(), feature_map);
        }
      }

      vtkSmartPointer<vtkPolyDataMapper> mapper = this->surface_mappers_[i];
      vtkSmartPointer<vtkActor> actor = this->surface_actors_[i];

      //this->update_points();

      this->draw_exclusion_spheres(shape);

      auto transform = this->get_transform(this->visualizer_->get_alignment_domain(), i);
      if (Viewer::is_reverse(transform)) { // if it's been reflected we need to reverse
        vtkSmartPointer<vtkReverseSense> reverse_filter = vtkSmartPointer<vtkReverseSense>::New();
        reverse_filter->SetInputData(poly_data);
        reverse_filter->ReverseNormalsOff();
        reverse_filter->ReverseCellsOn();
        reverse_filter->Update();
        poly_data = reverse_filter->GetOutput();
      }
      actor->SetUserTransform(transform);
      mapper->SetInputData(poly_data);

      int domain_scheme = (this->scheme_ + i) % this->color_schemes_.size();

      actor->SetMapper(mapper);
      actor->GetProperty()->SetDiffuseColor(color_schemes_[domain_scheme].foreground.r,
                                            color_schemes_[domain_scheme].foreground.g,
                                            color_schemes_[domain_scheme].foreground.b);
      actor->GetProperty()->SetSpecular(0.2);
      actor->GetProperty()->SetSpecularPower(15);

      if (feature_map != "" && poly_data) {
        poly_data->GetPointData()->SetActiveScalars(feature_map.c_str());
        mapper->ScalarVisibilityOn();
        mapper->SetScalarModeToUsePointData();

        auto scalars = poly_data->GetPointData()->GetScalars(feature_map.c_str());
        if (scalars) {
          double range[2];
          scalars->GetRange(range);
          this->update_difference_lut(range[0], range[1]);
          mapper->SetScalarRange(range[0], range[1]);
          this->visualizer_->update_feature_range(range);
        }
      }
      else {
        mapper->ScalarVisibilityOff();
      }
    }
    this->display_vector_field();
  }

  this->update_actors();
  this->update_glyph_properties();

  ren->AddViewProp(this->corner_annotation_);
}

//-----------------------------------------------------------------------------
void Viewer::clear_viewer()
{
  this->renderer_->RemoveAllViewProps();
  this->shape_ = nullptr;
  this->visible_ = false;
  this->viewer_ready_ = false;
  this->mesh_ready_ = false;
  this->loading_displayed_ = false;
}

//-----------------------------------------------------------------------------
void Viewer::reset_camera(std::array<double, 3> c)
{
  this->renderer_->ResetCamera();

//  this->renderer_->GetActiveCamera()->SetViewUp(0, 1, 0);
//this->renderer_->GetActiveCamera()->SetFocalPoint(0, 0, 0);
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

  vnl_vector<double> correspondence_points;
  if (this->visualizer_->get_display_mode() == Visualizer::MODE_RECONSTRUCTION_C) {
    correspondence_points = this->shape_->get_global_correspondence_points_for_display();
  }
  else {
    correspondence_points = this->shape_->get_local_correspondence_points();
  }

  int num_points = correspondence_points.size() / 3;

  vtkFloatArray* scalars =
    (vtkFloatArray*) (this->glyph_point_set_->GetPointData()->GetScalars());

  Eigen::VectorXf scalar_values;
  if (this->showing_feature_map()) {
    auto feature_map = this->get_displayed_feature_map();
    scalar_values = this->shape_->get_point_features(feature_map);
  }

  if (num_points > 0) {
    this->viewer_ready_ = true;
    this->glyphs_->SetRange(0.0, (double) num_points + 1);
    this->glyph_mapper_->SetScalarRange(0.0, (double) num_points + 1.0);

    this->glyph_points_->Reset();
    this->glyph_points_->SetNumberOfPoints(num_points);

    scalars->Reset();
    scalars->SetNumberOfTuples(num_points);

    unsigned int idx = 0;
    for (int i = 0; i < num_points; i++) {
      if (scalar_values.size() > i) {
        scalars->InsertValue(i, scalar_values[i]);
      }
      else {
        scalars->InsertValue(i, i);
      }
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

  if (this->showing_feature_map()) {
    this->glyph_mapper_->SetScalarRange(this->surface_lut_->GetRange());
    this->glyph_point_set_->GetPointData()->SetScalars(scalars);
    this->glyphs_->SetColorModeToColorByScalar();
    this->glyphs_->SetScaleModeToDataScalingOff();
    this->glyph_mapper_->SetColorModeToMapScalars();
    this->glyph_mapper_->ScalarVisibilityOn();
    this->glyph_mapper_->SetLookupTable(this->surface_lut_);
  }

  int alignment_domain = this->visualizer_->get_alignment_domain();

  //this->glyph_actor_->SetUserTransform(this->get_transform(alignment_domain));

  bool reverse = false;
  if (this->visualizer_->get_display_mode() == Visualizer::MODE_ORIGINAL_C ||
      this->visualizer_->get_display_mode() == Visualizer::MODE_GROOMED_C) {
    if (this->visualizer_->get_center()) {
      auto transform = this->shape_->get_alignment(alignment_domain);
      reverse = Viewer::is_reverse(transform);
      this->glyph_actor_->SetUserTransform(transform);
    }
    else {
      if (!this->shape_->has_alignment()) {
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->DeepCopy(this->shape_->get_original_transform());
        transform->Inverse();
        reverse = Viewer::is_reverse(transform);
        this->glyph_actor_->SetUserTransform(transform);
      }
    }
  }

  if (reverse) {
    this->glyphs_->SetSourceConnection(this->reverse_sphere_->GetOutputPort());
  } else {
    this->glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
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
  this->renderer_->RemoveActor(this->scalar_bar_actor_);

  for (size_t i = 0; i < this->surface_actors_.size(); i++) {
    this->renderer_->RemoveActor(this->surface_actors_[i]);
  }

  if (this->show_glyphs_) {
    this->renderer_->AddActor(this->glyph_actor_);

    this->renderer_->AddActor(this->exclusion_sphere_actor_);
    if (this->arrows_visible_) {
      this->renderer_->AddActor(this->arrow_glyph_actor_);
    }

    if (this->arrows_visible_ || this->showing_feature_map()) {
      this->renderer_->AddActor(this->scalar_bar_actor_);
    }
  }

  if (this->show_surface_ && this->meshes_.valid()) {
    for (int i = 0; i < this->number_of_domains_; i++) {
      this->surface_actors_[i]->GetProperty()->BackfaceCullingOff();
      this->renderer_->AddActor(this->surface_actors_[i]);
    }
  }
  this->update_opacities();
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
  if (!this->arrows_visible_ && !this->showing_feature_map()) {
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
  QList<Shape::Point> centers = object->get_exclusion_sphere_centers();
  QList<double> radii = object->get_exclusion_sphere_radii();

  int num_points = centers.size();

  if (num_points <= 0) {
    this->exclusion_sphere_points_->Reset();
  }
  else {

    vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*) (
      this->exclusion_sphere_point_set_->GetPointData()->GetScalars());
    scalars->Reset();
    scalars->SetNumberOfTuples(num_points);

    this->exclusion_sphere_glyph_->SetRange(0.0, (double) num_points + 1);
    this->exclusion_sphere_mapper_->SetScalarRange(0.0, (double) num_points + 1.0);

    this->exclusion_sphere_points_->Reset();
    this->exclusion_sphere_points_->SetNumberOfPoints(num_points);

    for (int i = 0; i < num_points; i++) {
      Shape::Point p = centers[i];
      scalars->InsertValue(i, radii[i]);
      this->exclusion_sphere_points_->InsertPoint(i, p.x, p.y, p.z);
    }
  }

  this->exclusion_sphere_points_->Modified();
}

//-----------------------------------------------------------------------------
void Viewer::update_difference_lut(float r0, float r1)
{
  float maxrange = 0;
  if (fabs(r0) > fabs(r1)) { maxrange = fabs(r0); }
  else { maxrange = fabs(r1); }

  double rd = r1 - r0;

  double range[2];
  range[0] = r0;
  range[1] = r1;

  this->surface_lut_->SetTableRange(range);
  this->surface_lut_->Build();
  for (size_t i = 0; i < this->surface_mappers_.size(); i++) {
    this->surface_mappers_[i]->SetLookupTable(this->surface_lut_);
    this->surface_mappers_[i]->SetScalarRange(range[0], range[1]);
  }

  this->arrow_glyph_mapper_->SetLookupTable(this->surface_lut_);
  this->arrow_glyph_mapper_->SetScalarRange(range);
  this->glyph_mapper_->SetScalarRange(range);
  this->scalar_bar_actor_->SetLookupTable(this->surface_lut_);
  if (rd > 100) {
    this->scalar_bar_actor_->SetLabelFormat("%.0f");
  }
  else if (rd > 1) {
    this->scalar_bar_actor_->SetLabelFormat("%.1f");
  }
  else if (rd > 0.1) {
    this->scalar_bar_actor_->SetLabelFormat("%.2f");
  }
  else {
    this->scalar_bar_actor_->SetLabelFormat("%-#6.3g");
  }
  this->scalar_bar_actor_->Modified();
}

//-----------------------------------------------------------------------------
bool Viewer::showing_feature_map()
{
  return this->visualizer_->get_feature_map() != ""
         || (this->shape_ && this->shape_->get_override_feature() != "");
}

//-----------------------------------------------------------------------------
void Viewer::update_feature_range(double* range)
{
  this->update_difference_lut(range[0], range[1]);
}

//-----------------------------------------------------------------------------
void Viewer::update_opacities()
{
  auto opacities = this->visualizer_->get_opacities();
  if (opacities.size() == this->surface_mappers_.size()) {
    for (size_t i = 0; i < opacities.size(); i++) {
      this->surface_actors_[i]->GetProperty()->SetOpacity(opacities[i]);
    }
  }
}

//-----------------------------------------------------------------------------
QSharedPointer<Shape> Viewer::get_shape()
{
  return this->shape_;
}

//-----------------------------------------------------------------------------
void Viewer::initialize_surfaces()
{
  if (this->number_of_domains_ > this->surface_mappers_.size()) {

    this->surface_mappers_.resize(this->number_of_domains_);
    this->surface_actors_.resize(this->number_of_domains_);

    for (int i = 0; i < this->number_of_domains_; i++) {
      this->surface_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      this->surface_actors_[i] = vtkSmartPointer<vtkActor>::New();
      this->surface_actors_[i]->SetMapper(this->surface_mappers_[i]);
    }
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_transform(int alignment_domain, int domain)
{
  return this->visualizer_->get_transform(this->shape_, alignment_domain, domain);
}

//-----------------------------------------------------------------------------
bool Viewer::is_reverse(vtkSmartPointer<vtkTransform> transform)
{
  bool reverse = false;
  for (int i=0;i<3;i++) {
    if (transform && transform->GetScale()[i] < 0) {
      reverse = true;
    }
  }
  return reverse;
}
}
