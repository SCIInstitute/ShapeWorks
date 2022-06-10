// vtk
#include <vtkArrowSource.h>
#include <vtkCell.h>
#include <vtkCellPicker.h>
#include <vtkColorSeries.h>
#include <vtkCornerAnnotation.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkHandleWidget.h>
#include <vtkImageData.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLookupTable.h>
#include <vtkPickingManager.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataPointPlacer.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkScalarBarActor.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnsignedLongArray.h>

// shapeworks
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Utils/StudioUtils.h>
#include <Visualization/LandmarkWidget.h>
#include <Visualization/Lightbox.h>
#include <Visualization/PaintWidget.h>
#include <Visualization/PlaneWidget.h>
#include <Visualization/Viewer.h>
#include <Visualization/Visualizer.h>

namespace shapeworks {

const double EXCLUDED_COLOR[4] = {0.45, 0.45, 0.45, 1.0};

//-----------------------------------------------------------------------------
Viewer::Viewer() {
  cell_picker_ = vtkSmartPointer<vtkCellPicker>::New();
  cell_picker_->SetPickFromList(1);
  prop_picker_ = vtkSmartPointer<vtkPropPicker>::New();
  prop_picker_->SetPickFromList(1);
  point_placer_ = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  point_placer_->SetDistanceOffset(0);

  landmark_widget_ = std::make_shared<LandmarkWidget>(this);
  plane_widget_ = std::make_shared<PlaneWidget>(this);
  paint_widget_ = vtkSmartPointer<PaintWidget>::New();
  paint_widget_->set_viewer(this);

  sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();
  reverse_sphere_ = vtkSmartPointer<vtkReverseSense>::New();
  reverse_sphere_->SetInputConnection(sphere_source_->GetOutputPort());
  reverse_sphere_->ReverseNormalsOff();
  reverse_sphere_->ReverseCellsOn();

  surface_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  surface_lut_->SetTableRange(0, 1);
  surface_lut_->SetHueRange(0.667, 0.0);
  surface_lut_->SetSaturationRange(1, 1);
  surface_lut_->SetValueRange(1, 1);
  surface_lut_->SetIndexedLookup(false);
  surface_lut_->Build();

  glyph_points_ = vtkSmartPointer<vtkPoints>::New();
  glyph_points_->SetDataTypeToDouble();
  glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  glyph_point_set_->SetPoints(glyph_points_);
  glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkFloatArray>::New());

  glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  glyphs_->SetInputData(glyph_point_set_);
  glyphs_->ScalingOn();
  glyphs_->ClampingOff();
  glyphs_->SetScaleModeToDataScalingOff();
  glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
  glyphs_->GeneratePointIdsOn();

  glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  glyph_mapper_->SetInputConnection(glyphs_->GetOutputPort());

  glyph_actor_ = vtkSmartPointer<vtkActor>::New();
  glyph_actor_->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  glyph_actor_->GetProperty()->SetDiffuse(0.8);
  glyph_actor_->GetProperty()->SetSpecular(0.3);
  glyph_actor_->GetProperty()->SetSpecularPower(10.0);
  glyph_actor_->SetMapper(glyph_mapper_);

  arrow_flip_filter_ = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  arrow_glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  arrow_glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  arrow_glyph_actor_ = vtkSmartPointer<vtkActor>::New();

  // Arrow glyphs
  arrow_source_ = vtkSmartPointer<vtkArrowSource>::New();
  arrow_source_->SetTipResolution(6);
  arrow_source_->SetShaftResolution(6);

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
  transform_180_ = vtkSmartPointer<vtkTransform>::New();
  transform_180_->Concatenate(t1);

  arrow_flip_filter_->SetTransform(transform_180_);
  arrow_flip_filter_->SetInputConnection(arrow_source_->GetOutputPort());

  arrow_glyphs_->SetSourceConnection(arrow_flip_filter_->GetOutputPort());
  arrow_glyphs_->SetInputData(glyph_point_set_);
  arrow_glyphs_->ScalingOn();
  arrow_glyphs_->ClampingOff();

  arrow_glyphs_->SetVectorModeToUseVector();
  arrow_glyphs_->SetScaleModeToScaleByVector();

  arrow_glyph_mapper_->SetInputConnection(arrow_glyphs_->GetOutputPort());

  arrow_glyph_actor_->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  arrow_glyph_actor_->GetProperty()->SetDiffuse(0.8);
  arrow_glyph_actor_->GetProperty()->SetSpecular(0.3);
  arrow_glyph_actor_->GetProperty()->SetSpecularPower(10.0);
  arrow_glyph_actor_->SetMapper(arrow_glyph_mapper_);

  arrow_glyphs_->SetColorModeToColorByScalar();
  arrow_glyph_mapper_->SetColorModeToMapScalars();
  arrow_glyph_mapper_->ScalarVisibilityOn();

  visible_ = false;
  scheme_ = 0;

  update_glyph_properties();

  scalar_bar_actor_ = vtkSmartPointer<vtkScalarBarActor>::New();
  scalar_bar_actor_->SetTitle("");
  scalar_bar_actor_->SetLookupTable(surface_lut_);
  // scalar_bar_actor_->SetOrientationToHorizontal();
  scalar_bar_actor_->SetOrientationToVertical();
  scalar_bar_actor_->SetMaximumNumberOfColors(1000);
  scalar_bar_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  scalar_bar_actor_->GetPositionCoordinate()->SetValue(.3, .2);
  scalar_bar_actor_->SetWidth(0.10);
  scalar_bar_actor_->SetHeight(0.7);
  scalar_bar_actor_->SetPosition(0.9, 0.1);
  scalar_bar_actor_->SetLabelFormat("%.1f");
  scalar_bar_actor_->GetTitleTextProperty()->SetFontFamilyToArial();
  scalar_bar_actor_->GetTitleTextProperty()->SetFontSize(4);
  scalar_bar_actor_->GetLabelTextProperty()->SetFontFamilyToArial();
  scalar_bar_actor_->GetLabelTextProperty()->SetFontSize(4);
  scalar_bar_actor_->GetLabelTextProperty()->SetJustificationToCentered();
  scalar_bar_actor_->GetLabelTextProperty()->SetColor(1, 1, 1);

  corner_annotation_ = vtkSmartPointer<vtkCornerAnnotation>::New();
  corner_annotation_->SetLinearFontScaleFactor(2);
  corner_annotation_->SetNonlinearFontScaleFactor(1);
  corner_annotation_->SetMaximumFontSize(32);
  corner_annotation_->SetMaximumLineHeight(0.03);
}

//-----------------------------------------------------------------------------
void Viewer::set_color_scheme(int scheme) {
  scheme_ = scheme;

  for (size_t i = 0; i < surface_actors_.size(); i++) {
    int scheme = (scheme_ + i) % color_schemes_.size();

    double rgba[4] = {color_schemes_[scheme].foreground.r, color_schemes_[scheme].foreground.g,
                      color_schemes_[scheme].foreground.b, 1.0};
    surface_actors_[i]->GetProperty()->SetDiffuseColor(rgba);

    ffc_luts_[i]->SetTableValue(0, EXCLUDED_COLOR);
    ffc_luts_[i]->SetTableValue(1, rgba);
  }

  renderer_->SetBackground(color_schemes_[scheme].background.r, color_schemes_[scheme].background.g,
                           color_schemes_[scheme].background.b);

  double color = color_schemes_[scheme].get_text_intensity();
  scalar_bar_actor_->GetLabelTextProperty()->SetColor(color, color, color);
}

//-----------------------------------------------------------------------------
void Viewer::handle_new_mesh() {
  if (!mesh_ready_ && shape_ && shape_->get_meshes(session_->get_display_mode()).valid()) {
    display_shape(shape_);
  }
}

//-----------------------------------------------------------------------------
bool Viewer::is_viewer_ready() { return viewer_ready_; }

//-----------------------------------------------------------------------------
void Viewer::set_visualizer(Visualizer* visualizer) { visualizer_ = visualizer; }

//-----------------------------------------------------------------------------
void Viewer::display_vector_field() {
  Eigen::VectorXd vecs;
  if (session_->should_difference_vectors_show()) {
    vecs = shape_->get_particles().get_difference_vectors(session_->get_difference_particles());
  }

  if (vecs.size() == 0) {
    // restore things to normal
    glyphs_->ScalingOn();
    glyphs_->ClampingOff();
    glyphs_->SetScaleModeToDataScalingOff();
    glyph_mapper_->SetLookupTable(lut_);

    glyph_points_->SetDataTypeToDouble();
    glyph_point_set_->SetPoints(glyph_points_);
    glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkFloatArray>::New());

    glyphs_->SetInputData(glyph_point_set_);
    glyphs_->ScalingOn();
    glyphs_->ClampingOff();
    glyphs_->SetScaleModeToDataScalingOff();
    glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
    glyphs_->GeneratePointIdsOn();

    update_points();
    arrows_visible_ = false;
    return;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Step 1. Assign values at each correspondence point based on the image gradient
  /////////////////////////////////////////////////////////////////////////////////

  // Dot product difference vectors with the surface normals.
  vtkSmartPointer<vtkFloatArray> magnitudes = vtkSmartPointer<vtkFloatArray>::New();
  magnitudes->SetName("magnitudes");
  magnitudes->SetNumberOfComponents(1);

  vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
  vectors->SetName("vectors");
  vectors->SetNumberOfComponents(3);

  update_points();
  compute_point_differences(vecs, magnitudes, vectors);

  /////////////////////////////////////////////////////////////////////////////////
  // Step 2. Assign values at each mesh point based on the closest correspondence points
  /////////////////////////////////////////////////////////////////////////////////
  compute_surface_differences(magnitudes, vectors);

  /////////////////////////////////////////////////////////////////////////////////
  // Step 3. Assign the vectors and magnitudes to the glyphs and surface
  /////////////////////////////////////////////////////////////////////////////////

  // assign glyph vectors and magnitudes
  glyph_point_set_->GetPointData()->SetVectors(vectors);
  glyph_point_set_->GetPointData()->SetScalars(magnitudes);

  glyphs_->SetSourceConnection(arrow_source_->GetOutputPort());
  glyphs_->SetScaleModeToScaleByVector();
  glyphs_->SetScaleFactor(1.0);

  // update glyph rendering
  glyph_mapper_->SetLookupTable(surface_lut_);
  arrow_glyph_mapper_->SetLookupTable(surface_lut_);

  // update surface rendering
  for (size_t i = 0; i < surface_mappers_.size(); i++) {
    surface_mappers_[i]->SetLookupTable(surface_lut_);
    surface_mappers_[i]->InterpolateScalarsBeforeMappingOn();
    surface_mappers_[i]->SetColorModeToMapScalars();
    surface_mappers_[i]->ScalarVisibilityOn();
  }

  // Set the color modes
  arrow_glyphs_->SetColorModeToColorByScalar();
  glyphs_->SetColorModeToColorByScalar();
  glyph_mapper_->SetColorModeToMapScalars();
  glyph_mapper_->ScalarVisibilityOn();
  arrow_glyph_mapper_->SetColorModeToMapScalars();
  arrow_glyph_mapper_->ScalarVisibilityOn();

  arrows_visible_ = true;
}

//-----------------------------------------------------------------------------
void Viewer::compute_point_differences(const Eigen::VectorXd& vecs, vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors) {
  auto mesh_group = shape_->get_meshes(session_->get_display_mode());
  if (!mesh_group.valid() || vecs.size() == 0) {
    return;
  }

  vtkSmartPointer<vtkPolyData> point_set = glyph_point_set_;
  std::vector<vtkSmartPointer<vtkPolyData>> polys;
  std::vector<vtkSmartPointer<vtkKdTreePointLocator>> locators;

  for (size_t i = 0; i < mesh_group.meshes().size(); i++) {
    auto poly_data = mesh_group.meshes()[i]->get_poly_data();

    if (!poly_data || poly_data->GetNumberOfPoints() == 0) {
      locators.push_back(nullptr);
      continue;
    }

    auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputData(poly_data);
    normals->Update();
    normals->SetSplitting(false);  // must be sure not to split normals
    poly_data = normals->GetOutput();
    polys.push_back(poly_data);

    auto locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    locator->SetDataSet(poly_data);
    locator->BuildLocator();
    locators.push_back(locator);
  }

  double minmag = std::numeric_limits<double>::max();
  double maxmag = std::numeric_limits<double>::min();

  bool any_valid = false;
  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling happens properly.
  for (vtkIdType i = 0; i < point_set->GetNumberOfPoints(); i++) {
    int domain = shape_->get_particles().get_domain_for_combined_id(i);

    if (!locators[domain]) {
      vectors->InsertNextTuple3(0, 0, 0);
      magnitudes->InsertNextTuple1(0);
      continue;
    }

    auto id = locators[domain]->FindClosestPoint(point_set->GetPoint(i));
    double* normal = polys[domain]->GetPointData()->GetNormals()->GetTuple(id);

    float xd = vecs(i * 3 + 0);
    float yd = vecs(i * 3 + 1);
    float zd = vecs(i * 3 + 2);

    float mag = xd * normal[0] + yd * normal[1] + zd * normal[2];
    if (std::isnan(mag)) {
      mag = 0;
    }
    minmag = std::min<float>(minmag, mag);
    maxmag = std::max<float>(maxmag, mag);
    any_valid = true;
    vectors->InsertNextTuple3(normal[0] * mag, normal[1] * mag, normal[2] * mag);
    magnitudes->InsertNextTuple1(mag);
  }

  if (!session_->get_feature_auto_scale()) {
    minmag = session_->get_feature_range_min();
    maxmag = session_->get_feature_range_max();
  }

  if (any_valid) {
    update_difference_lut(minmag, maxmag);
    visualizer_->update_feature_range(minmag, maxmag);
  }
}

//-----------------------------------------------------------------------------
void Viewer::compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                         vtkSmartPointer<vtkFloatArray> vectors) {
  auto mesh_group = shape_->get_meshes(session_->get_display_mode());
  if (!mesh_group.valid()) {
    return;
  }

  for (size_t i = 0; i < mesh_group.meshes().size(); i++) {
    auto poly_data = mesh_group.meshes()[i]->get_poly_data();
    if (!poly_data || poly_data->GetNumberOfPoints() == 0) {
      return;
    }

    vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
    point_data->SetPoints(glyph_points_);

    auto point_locator = vtkSmartPointer<vtkPointLocator>::New();
    point_locator->SetDataSet(point_data);
    point_locator->SetDivisions(100, 100, 100);
    point_locator->BuildLocator();

    auto surface_magnitudes = vtkSmartPointer<vtkFloatArray>::New();
    surface_magnitudes->SetName("surface_difference");
    surface_magnitudes->SetNumberOfComponents(1);
    surface_magnitudes->SetNumberOfTuples(poly_data->GetPoints()->GetNumberOfPoints());

    auto surface_vectors = vtkSmartPointer<vtkFloatArray>::New();
    surface_vectors->SetNumberOfComponents(3);
    surface_vectors->SetName("surface_vectors");
    surface_vectors->SetNumberOfTuples(poly_data->GetPoints()->GetNumberOfPoints());

    for (vtkIdType i = 0; i < surface_magnitudes->GetNumberOfTuples(); i++) {
      // find the 8 closest correspondence points the to current vertex
      auto closest_points = vtkSmartPointer<vtkIdList>::New();
      point_locator->FindClosestNPoints(8, poly_data->GetPoint(i), closest_points);
      // assign scalar value based on a weighted scheme
      float weighted_scalar = 0.0f;
      float distance_sum = 0.0f;
      float distance[8];

      bool exactly_on_point = false;
      float exact_scalar = 0.0f;

      for (vtkIdType p = 0; p < closest_points->GetNumberOfIds(); p++) {
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
        } else {
          distance[p] = 1.0f / (x * x + y * y + z * z);
        }

        // multiply scalar value by weight and add to running sum
        distance_sum += distance[p];
      }

      float vecX = 0.0f;
      float vecY = 0.0f;
      float vecZ = 0.0f;

      for (vtkIdType p = 0; p < closest_points->GetNumberOfIds(); p++) {
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
std::string Viewer::get_displayed_feature_map() {
  auto feature_map = visualizer_->get_feature_map();

  if (shape_->get_override_feature() != "") {
    feature_map = shape_->get_override_feature();
  }
  return feature_map;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPlane> Viewer::transform_plane(vtkSmartPointer<vtkPlane> plane,
                                                  vtkSmartPointer<vtkTransform> transform) {
  vtkSmartPointer<vtkPlane> new_plane = vtkSmartPointer<vtkPlane>::New();
  double new_origin[3];
  double new_normal[3];
  double normal[3];
  plane->GetNormal(normal);
  normal[0] = normal[0] + plane->GetOrigin()[0];
  normal[1] = normal[1] + plane->GetOrigin()[1];
  normal[2] = normal[2] + plane->GetOrigin()[2];
  transform->TransformPoint(normal, new_normal);
  transform->TransformPoint(plane->GetOrigin(), new_origin);
  new_normal[0] = new_normal[0] - new_origin[0];
  new_normal[1] = new_normal[1] - new_origin[1];
  new_normal[2] = new_normal[2] - new_origin[2];
  new_plane->SetNormal(new_normal);
  new_plane->SetOrigin(new_origin);
  return new_plane;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPoints> Viewer::get_glyph_points() { return glyph_points_; }

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_alignment_transform() {
  int alignment_domain = visualizer_->get_alignment_domain();

  auto transform = shape_->get_alignment(alignment_domain);
  return transform;
}

//-----------------------------------------------------------------------------
void Viewer::update_clipping_planes() {
  for (int i = 0; i < surface_mappers_.size(); i++) {
    auto mapper = surface_mappers_[i];
    mapper->RemoveAllClippingPlanes();
    if (shape_ && session_->get_display_mode() != DisplayMode::Reconstructed) {
      auto& constraints = shape_->get_constraints(i);
      for (auto& plane : constraints.getPlaneConstraints()) {
        if (plane.points().size() == 3) {
          auto vtk_plane = plane.getVTKPlane();
          auto transform = get_transform(visualizer_->get_alignment_domain(), i);
          vtk_plane = transform_plane(vtk_plane, transform);
          mapper->AddClippingPlane(vtk_plane);
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolygonalSurfacePointPlacer> Viewer::get_point_placer() { return point_placer_; }

//-----------------------------------------------------------------------------
void Viewer::handle_ffc_paint(double display_pos[], double world_pos[]) {
  if (!meshes_.valid()) {
    return;
  }

  prop_picker_->Pick(display_pos[0], display_pos[1], 0, renderer_);

  int domain = 0;
  for (int i = 0; i < unclipped_surface_actors_.size(); i++) {
    if (prop_picker_->GetActor() == unclipped_surface_actors_[i]) {
      domain = i;
    }
  }

  auto mesh = meshes_.meshes()[domain];

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->DeepCopy(get_landmark_transform(domain));
  transform->Inverse();
  double xyzt[3];
  transform->TransformPoint(world_pos, xyzt);

  mesh->paint_ffc(xyzt, paint_widget_->get_brush_size(), session_->get_ffc_paint_mode_inclusive());

  auto& ffc = shape_->get_constraints(domain).getFreeformConstraint();
  ffc.setDefinition(mesh->get_poly_data());
  if (!ffc.isSet()) {
    ffc.setPainted(true);
    session_->trigger_ffc_changed();
  }
}

//-----------------------------------------------------------------------------
void Viewer::update_landmarks() { landmark_widget_->update_landmarks(); }

//-----------------------------------------------------------------------------
void Viewer::update_planes() {
  update_actors();
  update_clipping_planes();
  plane_widget_->update();
}

//-----------------------------------------------------------------------------
void Viewer::update_ffc_mode() {
  paint_widget_->SetDefaultRenderer(renderer_);
  paint_widget_->SetRenderer(renderer_);
  paint_widget_->SetInteractor(renderer_->GetRenderWindow()->GetInteractor());
  paint_widget_->SetEnabled(0);
  paint_widget_->SetEnabled(session_->get_ffc_paint_active());

  double paint_size = session_->get_ffc_paint_size() * 0.10;

  // scale based on dimension of data
  if (meshes_.valid()) {
    paint_size = paint_size / 100.0 * meshes_.meshes()[0]->get_largest_dimension_size();
  }

  paint_widget_->set_brush_size(paint_size);
  paint_widget_->SetPointPlacer(point_placer_);
}

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkActor>> Viewer::get_surface_actors() { return surface_actors_; }

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkActor>> Viewer::get_unclipped_surface_actors() { return unclipped_surface_actors_; }

//-----------------------------------------------------------------------------
MeshGroup Viewer::get_meshes() { return meshes_; }

//-----------------------------------------------------------------------------
void Viewer::display_shape(QSharedPointer<Shape> shape) {
  visible_ = true;

  shape_ = shape;

  meshes_ = shape->get_meshes(session_->get_display_mode());

  auto compare_settings = session_->get_compare_settings();

  bool compare_ready = true;
  if (compare_settings.compare_enabled_) {
    compare_meshes_ = shape->get_meshes(compare_settings.get_display_mode());
    compare_ready = compare_meshes_.valid();
  }

  if ((!meshes_.valid() && loading_displayed_) || !compare_ready) {
    // no need to proceed
    mesh_ready_ = false;
    return;
  }
  mesh_ready_ = true;

  QStringList annotations = shape->get_annotations();
  corner_annotation_->SetText(0, (annotations[0]).toStdString().c_str());
  corner_annotation_->SetText(1, (annotations[1]).toStdString().c_str());
  corner_annotation_->SetText(2, (annotations[2]).toStdString().c_str());
  corner_annotation_->SetText(3, (annotations[3]).toStdString().c_str());
  corner_annotation_->GetTextProperty()->SetColor(0.50, 0.5, 0.5);

  renderer_->RemoveAllViewProps();

  number_of_domains_ = session_->get_domains_per_shape();
  initialize_surfaces();

  if (!meshes_.valid()) {
    mesh_ready_ = false;
    viewer_ready_ = false;
    loading_displayed_ = true;
    update_points();
  } else {
    loading_displayed_ = false;
    viewer_ready_ = true;

    for (size_t i = 0; i < meshes_.meshes().size(); i++) {
      MeshHandle mesh = meshes_.meshes()[i];

      vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

      auto feature_map = get_displayed_feature_map();

      if (session_->should_difference_vectors_show()) {
        feature_map = "";
      }

      if (feature_map != "" && poly_data) {
        auto scalar_array = poly_data->GetPointData()->GetArray(feature_map.c_str());
        if (!scalar_array) {
          shape_->load_feature(session_->get_display_mode(), feature_map);
        }
      }

      auto mapper = surface_mappers_[i];
      auto actor = surface_actors_[i];

      auto transform = get_transform(visualizer_->get_alignment_domain(), i);
      if (Viewer::is_reverse(transform)) {  // if it's been reflected we need to reverse
        poly_data = StudioUtils::reverse_poly_data(poly_data);
      }
      actor->SetUserTransform(transform);
      unclipped_surface_actors_[i]->SetUserTransform(transform);
      mapper->SetInputData(poly_data);
      unclipped_surface_mappers_[i]->SetInputData(poly_data);

      int domain_scheme = (scheme_ + i) % color_schemes_.size();

      actor->SetMapper(mapper);
      actor->GetProperty()->SetDiffuseColor(color_schemes_[domain_scheme].foreground.r,
                                            color_schemes_[domain_scheme].foreground.g,
                                            color_schemes_[domain_scheme].foreground.b);
      actor->GetProperty()->SetSpecular(0.2);
      actor->GetProperty()->SetSpecularPower(15);
      actor->GetProperty()->SetOpacity(1.0);

      if (compare_settings.compare_enabled_ && compare_settings.surface_distance_mode_) {
        // compute surface to surface distance
        Mesh m(poly_data);
        auto compare_poly_data = compare_meshes_.meshes()[i]->get_poly_data();
        Mesh m2(compare_poly_data);
        auto field = m.distance(m2)[0];
        m.setField("distance", field, Mesh::Point);
      }

      if (feature_map != "" && poly_data) {
        set_scalar_visibility(poly_data, mapper, feature_map.c_str());
      } else {
        if (session_->get_display_mode() != DisplayMode::Reconstructed) {
          try {
            auto& ffc = shape_->get_constraints(i).getFreeformConstraint();
            if (ffc.getDefinition() != poly_data) {
              ffc.computeBoundaries();
              ffc.applyToPolyData(poly_data);
              ffc.setDefinition(poly_data);
            }
          } catch (std::exception& e) {
            STUDIO_SHOW_ERROR(QString("Unable to apply free form constraints: ") + e.what());
          }
        }

        mapper->ScalarVisibilityOn();
        mapper->SetScalarModeToUsePointData();
        mapper->SetScalarRange(0, 1);
        mapper->SetLookupTable(ffc_luts_[i]);
        mesh->get_or_create_array(StudioMesh::FFC_PAINT, 1.0);
        poly_data->GetPointData()->SetActiveScalars(StudioMesh::FFC_PAINT);
      }
    }
    update_points();

    display_vector_field();
  }

  insert_compare_meshes();

  update_image_volume();

  update_clipping_planes();
  update_actors();
  update_glyph_properties();
  landmark_widget_->clear_landmark_handles();
  plane_widget_->clear_planes();
  update_landmarks();
  update_planes();
  update_ffc_mode();
  renderer_->AddViewProp(corner_annotation_);
}

//-----------------------------------------------------------------------------
void Viewer::clear_viewer() {
  renderer_->RemoveAllViewProps();
  shape_ = nullptr;
  visible_ = false;
  viewer_ready_ = false;
  mesh_ready_ = false;
  loading_displayed_ = false;
  update_landmarks();
}

//-----------------------------------------------------------------------------
void Viewer::reset_camera(std::array<double, 3> c) { renderer_->ResetCamera(); }

//-----------------------------------------------------------------------------
void Viewer::reset_camera() {
  slice_view_.update_camera();
  renderer_->ResetCameraClippingRange();
  renderer_->ResetCamera();
  renderer_->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------
void Viewer::set_color_series(ColorMap color_series) {
  color_series_ = color_series;
  color_series.construct_lookup_table(surface_lut_);
}

//-----------------------------------------------------------------------------
void Viewer::set_renderer(vtkSmartPointer<vtkRenderer> renderer) { renderer_ = renderer; }

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkRenderer> Viewer::get_renderer() { return renderer_; }

//-----------------------------------------------------------------------------
void Viewer::set_glyph_size_and_quality(double size, double quality) {
  glyph_size_ = size;
  glyph_quality_ = quality;
  update_glyph_properties();
}

//-----------------------------------------------------------------------------
double Viewer::get_glyph_size() { return glyph_size_; }

//-----------------------------------------------------------------------------
double Viewer::get_glyph_quality() { return glyph_quality_; }

//-----------------------------------------------------------------------------
void Viewer::set_session(QSharedPointer<Session> session) { session_ = session; }

//-----------------------------------------------------------------------------
QSharedPointer<Session> Viewer::get_session() { return session_; }

//-----------------------------------------------------------------------------
void Viewer::update_glyph_properties() {
  if (renderer_) {
    double bounds[6];
    renderer_->ComputeVisiblePropBounds(bounds);
    double sum_range = bounds[1] - bounds[0];
    sum_range += bounds[3] - bounds[2];
    sum_range += bounds[5] - bounds[4];
    double average_range = sum_range / 3.0;
    // sanity clamp
    glyph_size_ = std::max<double>(glyph_size_, average_range * 0.01);
    glyph_size_ = std::min<double>(glyph_size_, average_range * 0.25);
  }

  if (session_ && session_->should_difference_vectors_show()) {
    glyphs_->SetScaleFactor(1.0);
    arrow_glyphs_->SetScaleFactor(1.0);
  } else {
    glyphs_->SetScaleFactor(glyph_size_);
    arrow_glyphs_->SetScaleFactor(glyph_size_);
  }

  sphere_source_->SetThetaResolution(glyph_quality_);
  sphere_source_->SetPhiResolution(glyph_quality_);

  arrow_source_->SetTipResolution(glyph_quality_);
  arrow_source_->SetShaftResolution(glyph_quality_);

  glyphs_->Update();
  arrow_glyphs_->Update();
  landmark_widget_->update_glyph_properties();
  plane_widget_->update_glyph_properties();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_glyphs(bool show) {
  show_glyphs_ = show;
  update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::set_show_surface(bool show) {
  show_surface_ = show;
  update_actors();
}

//-----------------------------------------------------------------------------
void Viewer::update_points() {
  if (!shape_) {
    return;
  }

  Eigen::VectorXd correspondence_points;
  if (session_->get_display_mode() == DisplayMode::Reconstructed) {
    correspondence_points = shape_->get_correspondence_points_for_display();
  } else {
    correspondence_points = shape_->get_local_correspondence_points();
  }

  int num_points = correspondence_points.size() / 3;

  vtkFloatArray* scalars = (vtkFloatArray*)(glyph_point_set_->GetPointData()->GetScalars());

  Eigen::VectorXf scalar_values;
  if (showing_feature_map() && !session_->should_difference_vectors_show()) {
    auto feature_map = get_displayed_feature_map();
    shape_->load_feature(session_->get_display_mode(), feature_map);
    scalar_values = shape_->get_point_features(feature_map);
  }

  if (num_points > 0) {
    viewer_ready_ = true;
    glyphs_->SetRange(0.0, (double)num_points + 1);
    glyph_mapper_->SetScalarRange(0.0, (double)num_points + 1.0);

    glyph_points_->Reset();
    scalars->Reset();

    unsigned int idx = 0;
    for (int i = 0; i < num_points; i++) {
      double x = correspondence_points[idx++];
      double y = correspondence_points[idx++];
      double z = correspondence_points[idx++];

      if (slice_view_.should_point_show(x, y, z)) {
        if (scalar_values.size() > i) {
          scalars->InsertNextValue(scalar_values[i]);
        } else {
          scalars->InsertNextValue(i);
        }
        glyph_points_->InsertNextPoint(x, y, z);
      }
    }
  } else {
    glyph_points_->Reset();
    scalars->Reset();
  }

  if (showing_feature_map()) {
    glyph_mapper_->SetScalarRange(surface_lut_->GetRange());
    glyph_point_set_->GetPointData()->SetScalars(scalars);
    glyphs_->SetColorModeToColorByScalar();
    glyphs_->SetScaleModeToDataScalingOff();
    glyph_mapper_->SetColorModeToMapScalars();
    glyph_mapper_->ScalarVisibilityOn();
    glyph_mapper_->SetLookupTable(surface_lut_);
  }

  int alignment_domain = visualizer_->get_alignment_domain();

  // reset
  glyph_actor_->SetUserTransform(vtkSmartPointer<vtkTransform>::New());
  arrow_glyph_actor_->SetUserTransform(vtkSmartPointer<vtkTransform>::New());

  bool reverse = false;
  if (visualizer_->get_center()) {
    auto transform = shape_->get_alignment(alignment_domain);
    reverse = Viewer::is_reverse(transform);
    glyph_actor_->SetUserTransform(transform);
    arrow_glyph_actor_->SetUserTransform(transform);
  }

  if (session_->should_difference_vectors_show()) {
    /// TODO:  probably need reverse arrows?
  } else {
    if (reverse) {
      glyphs_->SetSourceConnection(reverse_sphere_->GetOutputPort());
    } else {
      glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
    }
  }
  glyph_points_->Modified();
}

//-----------------------------------------------------------------------------
void Viewer::update_actors() {
  if (!visible_) {
    return;
  }

  renderer_->RemoveActor(glyph_actor_);
  renderer_->RemoveActor(arrow_glyph_actor_);
  renderer_->RemoveActor(scalar_bar_actor_);

  cell_picker_->InitializePickList();
  prop_picker_->InitializePickList();
  point_placer_->RemoveAllProps();
  point_placer_->GetPolys()->RemoveAllItems();
  for (size_t i = 0; i < surface_actors_.size(); i++) {
    renderer_->RemoveActor(surface_actors_[i]);
  }
  for (size_t i = 0; i < unclipped_surface_actors_.size(); i++) {
    renderer_->RemoveActor(unclipped_surface_actors_[i]);
  }

  for (size_t i = 0; i < compare_actors_.size(); i++) {
    renderer_->RemoveActor(compare_actors_[i]);
  }

  if (show_glyphs_) {
    renderer_->AddActor(glyph_actor_);

    if (arrows_visible_) {
      renderer_->AddActor(arrow_glyph_actor_);
    }
  }

  if ((show_glyphs_ && arrows_visible_) || showing_feature_map()) {
    renderer_->AddActor(scalar_bar_actor_);
  }

  if (show_surface_ && meshes_.valid()) {
    for (int i = 0; i < number_of_domains_; i++) {
      renderer_->AddActor(unclipped_surface_actors_[i]);
      renderer_->AddActor(surface_actors_[i]);
      if (session_->get_compare_settings().compare_enabled_ &&
          !session_->get_compare_settings().surface_distance_mode_) {
        renderer_->AddActor(compare_actors_[i]);
      }

      surface_actors_[i]->GetProperty()->BackfaceCullingOff();
      unclipped_surface_actors_[i]->GetProperty()->BackfaceCullingOff();
      cell_picker_->AddPickList(unclipped_surface_actors_[i]);
      prop_picker_->AddPickList(unclipped_surface_actors_[i]);
      point_placer_->AddProp(unclipped_surface_actors_[i]);
      point_placer_->GetPolys()->AddItem(meshes_.meshes()[i]->get_poly_data());
    }
  }

  slice_view_.update_renderer();

  update_opacities();
}

//-----------------------------------------------------------------------------
void Viewer::remove_scalar_bar() { renderer_->RemoveActor(scalar_bar_actor_); }

//-----------------------------------------------------------------------------
vtkFloatArray* Viewer::get_particle_scalars() {
  vtkFloatArray* scalars = (vtkFloatArray*)(glyph_point_set_->GetPointData()->GetScalars());
  return scalars;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Viewer::get_particle_poly_data() { return glyph_point_set_; }

//-----------------------------------------------------------------------------
void Viewer::insert_compare_meshes() {
  auto settings = session_->get_compare_settings();

  if (!settings.compare_enabled_ || !compare_meshes_.valid() || settings.surface_distance_mode_) {
    return;
  }

  for (size_t i = 0; i < compare_meshes_.meshes().size(); i++) {
    MeshHandle mesh = compare_meshes_.meshes()[i];
    vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
    auto mapper = compare_mappers_[i];
    auto actor = compare_actors_[i];

    auto transform =
        visualizer_->get_transform(shape_, settings.get_display_mode(), visualizer_->get_alignment_domain(), i);
    if (Viewer::is_reverse(transform)) {  // if it's been reflected we need to reverse
      poly_data = StudioUtils::reverse_poly_data(poly_data);
    }
    /*
        if (session_->get_display_mode() == DisplayMode::Reconstructed) {
          auto procrustes_transform = shape_->get_procrustest_transform(i);

          if (procrustes_transform) {
            std::cerr << "compose transform!\n";
            vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
            vtkMatrix4x4::Multiply4x4(procrustes_transform->GetMatrix(), transform->GetMatrix(), matrix);

            transform = vtkSmartPointer<vtkTransform>::New();
            transform->SetMatrix(matrix);
          }
        }
        */

    actor->SetUserTransform(transform);
    mapper->SetInputData(poly_data);

    int domain_scheme = (scheme_ + i + 1) % color_schemes_.size();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(color_schemes_[domain_scheme].foreground.r,
                                          color_schemes_[domain_scheme].foreground.g,
                                          color_schemes_[domain_scheme].foreground.b);
    actor->GetProperty()->SetSpecular(0.2);
    actor->GetProperty()->SetSpecularPower(15);
    actor->GetProperty()->SetOpacity(settings.opacity_);
  }
}

//-----------------------------------------------------------------------------
void Viewer::set_scalar_visibility(vtkSmartPointer<vtkPolyData> poly_data, vtkSmartPointer<vtkPolyDataMapper> mapper,
                                   std::string scalar) {
  poly_data->GetPointData()->SetActiveScalars(scalar.c_str());
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUsePointData();
  auto scalars = poly_data->GetPointData()->GetScalars(scalar.c_str());
  if (scalars) {
    double range[2];
    scalars->GetRange(range);
    update_difference_lut(range[0], range[1]);
    mapper->SetScalarRange(range[0], range[1]);
    visualizer_->update_feature_range(range);
  }
}

//-----------------------------------------------------------------------------
void Viewer::update_image_volume() {
  if (!session_ || !shape_) {
    return;
  }
  slice_view_.set_orientation(session_->get_image_axis());

  if (meshes_.valid()) {
    slice_view_.set_mesh(meshes_.meshes()[0]->get_poly_data());
  }

  slice_view_.update_particles();

  auto image_volume_name = session_->get_image_name();
  if (image_volume_name == current_image_name_) {
    return;
  }
  current_image_name_ = image_volume_name;
  if (image_volume_name != "-none-") {
    auto volume = shape_->get_image_volume(image_volume_name);
    slice_view_.set_volume(volume);
  } else {
    slice_view_.set_volume(nullptr);
  }
  slice_view_.update_renderer();
  slice_view_.update_camera();
  update_points();
}

//-----------------------------------------------------------------------------
int Viewer::handle_pick(int* click_pos) {
  // First determine what was picked
  // we use a new prop picker here since the member one only uses the surface actors
  auto prop_picker = vtkSmartPointer<vtkPropPicker>::New();

  // check that the glyph actor was picked
  prop_picker->Pick(click_pos[0], click_pos[1], 0, renderer_);
  if (prop_picker->GetActor() != glyph_actor_) {
    return -1;
  }

  // now determine which point was picked
  // we use a new cell picker here since the member only only uses the surface actors
  auto cell_picker = vtkSmartPointer<vtkCellPicker>::New();
  cell_picker->Pick(click_pos[0], click_pos[1], 0, renderer_);
  vtkDataArray* input_ids = glyphs_->GetOutput()->GetPointData()->GetArray("InputPointIds");

  if (input_ids) {
    vtkCell* cell = glyphs_->GetOutput()->GetCell(cell_picker->GetCellId());

    if (cell && cell->GetNumberOfPoints() > 0) {
      // get first PointId from picked cell
      vtkIdType input_id = cell->GetPointId(0);

      // get matching Id from "InputPointIds" array
      vtkIdType glyph_id = input_ids->GetTuple1(input_id);

      if (glyph_id >= 0) {
        STUDIO_LOG_MESSAGE("picked correspondence point :" + QString::number(glyph_id));
        return glyph_id;
      }
    }
  }

  return -1;
}

//-----------------------------------------------------------------------------
PickResult Viewer::handle_ctrl_click(int* click_pos) {
  // First determine what was picked
  cell_picker_->Pick(click_pos[0], click_pos[1], 0, renderer_);
  PickResult result;

  for (int i = 0; i < unclipped_surface_actors_.size(); i++) {
    if (cell_picker_->GetActor() == unclipped_surface_actors_[i]) {
      double* pos = cell_picker_->GetPickPosition();

      auto transform = vtkSmartPointer<vtkTransform>::New();
      transform->DeepCopy(get_landmark_transform(i));
      transform->Inverse();
      double xyzt[3];
      transform->TransformPoint(pos, xyzt);
      result.pos_ = Shape::Point(xyzt[0], xyzt[1], xyzt[2]);
      result.domain_ = i;
      return result;
    }
  }
  return result;
}

//-----------------------------------------------------------------------------
void Viewer::set_lut(vtkSmartPointer<vtkLookupTable> lut) {
  lut_ = lut;
  if (!arrows_visible_ && !showing_feature_map()) {
    glyph_mapper_->SetLookupTable(lut_);
  }
}

//-----------------------------------------------------------------------------
void Viewer::set_loading_screen(vtkSmartPointer<vtkImageData> loading_screen) {
  // image_actor_->SetInputData(loading_screen);
}

//-----------------------------------------------------------------------------
void Viewer::update_difference_lut(float r0, float r1) {
  double rd = r1 - r0;

  double range[2];
  range[0] = r0;
  range[1] = r1;

  surface_lut_->SetTableRange(range);
  surface_lut_->Build();
  for (size_t i = 0; i < surface_mappers_.size(); i++) {
    surface_mappers_[i]->SetLookupTable(surface_lut_);
    surface_mappers_[i]->SetScalarRange(range[0], range[1]);
  }

  arrow_glyph_mapper_->SetLookupTable(surface_lut_);
  arrow_glyph_mapper_->SetScalarRange(range);
  glyph_mapper_->SetScalarRange(range);
  scalar_bar_actor_->SetLookupTable(surface_lut_);
  if (rd > 100) {
    scalar_bar_actor_->SetLabelFormat("%.0f");
  } else if (rd > 1) {
    scalar_bar_actor_->SetLabelFormat("%.1f");
  } else if (rd > 0.1) {
    scalar_bar_actor_->SetLabelFormat("%.2f");
  } else {
    scalar_bar_actor_->SetLabelFormat("%-#6.3g");
  }
  scalar_bar_actor_->Modified();
}

//-----------------------------------------------------------------------------
bool Viewer::showing_feature_map() {
  return visualizer_->get_feature_map() != "" || (shape_ && shape_->get_override_feature() != "");
}

//-----------------------------------------------------------------------------
void Viewer::update_feature_range(double* range) { update_difference_lut(range[0], range[1]); }

//-----------------------------------------------------------------------------
void Viewer::update_opacities() {
  auto opacities = visualizer_->get_opacities();
  if (opacities.size() == surface_mappers_.size()) {
    for (size_t i = 0; i < opacities.size(); i++) {
      unclipped_surface_actors_[i]->GetProperty()->SetOpacity(opacities[i]);
      surface_actors_[i]->GetProperty()->SetOpacity(opacities[i]);
    }
  }
}

//-----------------------------------------------------------------------------
QSharedPointer<Shape> Viewer::get_shape() { return shape_; }

//-----------------------------------------------------------------------------
void Viewer::initialize_surfaces() {
  if (number_of_domains_ > surface_mappers_.size()) {
    surface_mappers_.resize(number_of_domains_);
    surface_actors_.resize(number_of_domains_);
    unclipped_surface_mappers_.resize(number_of_domains_);
    unclipped_surface_actors_.resize(number_of_domains_);
    compare_mappers_.resize(number_of_domains_);
    compare_actors_.resize(number_of_domains_);
    ffc_luts_.resize(number_of_domains_);

    for (int i = 0; i < number_of_domains_; i++) {
      surface_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      surface_actors_[i] = vtkSmartPointer<vtkActor>::New();
      surface_actors_[i]->SetMapper(surface_mappers_[i]);
      unclipped_surface_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      unclipped_surface_mappers_[i]->ScalarVisibilityOff();

      unclipped_surface_actors_[i] = vtkSmartPointer<vtkActor>::New();
      unclipped_surface_actors_[i]->GetProperty()->SetColor(EXCLUDED_COLOR[0], EXCLUDED_COLOR[1], EXCLUDED_COLOR[2]);
      unclipped_surface_actors_[i]->SetMapper(unclipped_surface_mappers_[i]);

      compare_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      compare_actors_[i] = vtkSmartPointer<vtkActor>::New();

      ffc_luts_[i] = vtkSmartPointer<vtkLookupTable>::New();
      ffc_luts_[i]->SetHueRange(.667, 0.0);
      ffc_luts_[i]->SetNumberOfTableValues(2);
      ffc_luts_[i]->SetRange(0, 1);
      ffc_luts_[i]->SetValueRange(0, 1);
      ffc_luts_[i]->SetTableRange(0, 1);
      ffc_luts_[i]->Build();
    }
    set_color_scheme(scheme_);
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_transform(int alignment_domain, int domain) {
  return visualizer_->get_transform(shape_, alignment_domain, domain);
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_landmark_transform(int domain) {
  return visualizer_->get_transform(shape_, visualizer_->get_alignment_domain(), domain);
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_inverse_landmark_transform(int domain) {
  auto transform = get_landmark_transform(domain);
  auto inverse = vtkSmartPointer<vtkTransform>::New();
  inverse->DeepCopy(transform);
  inverse->Inverse();
  return inverse;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Viewer::get_image_transform() {
  return visualizer_->get_transform(shape_, visualizer_->get_alignment_domain(), 0);
}

//-----------------------------------------------------------------------------
SliceView& Viewer::slice_view() { return slice_view_; }

//-----------------------------------------------------------------------------
bool Viewer::is_reverse(vtkSmartPointer<vtkTransform> transform) {
  bool reverse = false;
  for (int i = 0; i < 3; i++) {
    if (transform && transform->GetScale()[i] < 0) {
      reverse = true;
    }
  }
  return reverse;
}
}  // namespace shapeworks
