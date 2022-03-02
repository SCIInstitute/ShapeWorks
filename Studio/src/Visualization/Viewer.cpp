#include <Data/Preferences.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Visualization/LandmarkWidget.h>
#include <Visualization/Lightbox.h>
#include <Visualization/PlaneWidget.h>
#include <Visualization/Viewer.h>
#include <Visualization/Visualizer.h>
#include <vtkArrowSource.h>
#include <vtkCell.h>
#include <vtkCellPicker.h>
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

namespace shapeworks {

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

  // exclusion spheres
  exclusion_sphere_points_ = vtkSmartPointer<vtkPoints>::New();
  exclusion_sphere_points_->SetDataTypeToDouble();
  exclusion_sphere_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  exclusion_sphere_point_set_->SetPoints(exclusion_sphere_points_);
  exclusion_sphere_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkUnsignedLongArray>::New());

  exclusion_sphere_glyph_ = vtkSmartPointer<vtkGlyph3D>::New();
  exclusion_sphere_glyph_->SetInputData(exclusion_sphere_point_set_);
  exclusion_sphere_glyph_->ScalingOn();
  exclusion_sphere_glyph_->ClampingOff();
  exclusion_sphere_glyph_->SetScaleModeToScaleByScalar();
  exclusion_sphere_glyph_->SetSourceConnection(sphere_source_->GetOutputPort());
  // exclusion_sphere_glyph_->SetColorModeToColorByScale();

  exclusion_sphere_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  exclusion_sphere_mapper_->SetInputConnection(exclusion_sphere_glyph_->GetOutputPort());
  exclusion_sphere_mapper_->SetScalarVisibility(0);

  exclusion_sphere_actor_ = vtkSmartPointer<vtkActor>::New();
  // exclusion_sphere_actor_->GetProperty()->SetSpecularColor( 1.0, 1.0, 0.0 );
  // exclusion_sphere_actor_->GetProperty()->SetDiffuse( 0.8 );
  // exclusion_sphere_actor_->GetProperty()->SetSpecular( 0.3 );
  // exclusion_sphere_actor_->GetProperty()->SetSpecularPower( 10.0 );
  exclusion_sphere_actor_->GetProperty()->SetOpacity(0.5);
  exclusion_sphere_actor_->GetProperty()->SetColor(0, 1, 0);
  exclusion_sphere_actor_->SetMapper(exclusion_sphere_mapper_);

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

    surface_actors_[i]->GetProperty()->SetDiffuseColor(
        color_schemes_[scheme].foreground.r, color_schemes_[scheme].foreground.g, color_schemes_[scheme].foreground.b);
  }

  renderer_->SetBackground(color_schemes_[scheme].background.r, color_schemes_[scheme].background.g,
                           color_schemes_[scheme].background.b);

  double average = (color_schemes_[scheme].background.r + color_schemes_[scheme].background.g +
                    color_schemes_[scheme].background.b) /
                   3.0;

  double color = 1;
  if (average > 0.5) {
    color = 0;
  }

  scalar_bar_actor_->GetLabelTextProperty()->SetColor(color, color, color);
}

//-----------------------------------------------------------------------------
void Viewer::handle_new_mesh() {
  if (!mesh_ready_ && shape_ && shape_->get_meshes(visualizer_->get_display_mode()).valid()) {
    display_shape(shape_);
  }
}

//-----------------------------------------------------------------------------
bool Viewer::is_viewer_ready() { return viewer_ready_; }

//-----------------------------------------------------------------------------
void Viewer::set_visualizer(Visualizer* visualizer) { visualizer_ = visualizer; }

//-----------------------------------------------------------------------------
void Viewer::display_vector_field() {
  std::vector<Shape::Point> vecs = shape_->get_vectors();
  if (vecs.empty()) {
    // restore things to normal
    glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
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
  magnitudes->SetNumberOfComponents(1);

  vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
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
void Viewer::compute_point_differences(const std::vector<Shape::Point>& points,
                                       vtkSmartPointer<vtkFloatArray> magnitudes,
                                       vtkSmartPointer<vtkFloatArray> vectors) {
  auto mesh_group = shape_->get_meshes(visualizer_->get_display_mode());
  if (!mesh_group.valid() || points.empty()) {
    return;
  }

  vtkSmartPointer<vtkPolyData> point_set = glyph_point_set_;
  std::vector<vtkSmartPointer<vtkPolyData>> polys;
  std::vector<vtkSmartPointer<vtkKdTreePointLocator>> locators;

  for (size_t i = 0; i < mesh_group.meshes().size(); i++) {
    vtkSmartPointer<vtkPolyData> poly_data = mesh_group.meshes()[i]->get_poly_data();

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

  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling
  // happens properly.
  for (unsigned int i = 0; i < point_set->GetNumberOfPoints(); i++) {
    int domain = shape_->get_particles().get_domain_for_combined_id(i);

    auto id = locators[domain]->FindClosestPoint(point_set->GetPoint(i));
    double* normal = polys[domain]->GetPointData()->GetNormals()->GetTuple(id);

    float xd = points[i].x;
    float yd = points[i].y;
    float zd = points[i].z;

    float mag = xd * normal[0] + yd * normal[1] + zd * normal[2];
    if (std::isnan(mag)) {
      mag = 0;
    }
    if (mag < minmag) {
      minmag = mag;
    }
    if (mag > maxmag) {
      maxmag = mag;
    }

    vectors->InsertNextTuple3(normal[0] * mag, normal[1] * mag, normal[2] * mag);
    magnitudes->InsertNextTuple1(mag);
  }
  update_difference_lut(minmag, maxmag);
}

//-----------------------------------------------------------------------------
void Viewer::compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                         vtkSmartPointer<vtkFloatArray> vectors) {
  if (!mesh_ready_) {
    return;
  }

  for (size_t i = 0; i < surface_mappers_.size(); i++) {
    vtkPolyData* poly_data = surface_mappers_[i]->GetInput();
    if (!poly_data || poly_data->GetNumberOfPoints() < 0) {
      return;
    }

    vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
    point_data->SetPoints(glyph_points_);

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
        } else {
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
    auto clipped_mapper = clipped_surface_mappers_[i];
    mapper->RemoveAllClippingPlanes();
    clipped_mapper->RemoveAllClippingPlanes();
    auto& constraints = shape_->get_constraints(i);
    for (auto& plane : constraints.getPlaneConstraints()) {
      if (plane.points().size() == 3) {
        auto vtk_plane = plane.getVTKPlane();
        auto transform = get_transform(visualizer_->get_alignment_domain(), i);

        vtk_plane = transform_plane(vtk_plane, transform);
        mapper->AddClippingPlane(vtk_plane);
        /*
                auto opposite_plane = vtkSmartPointer<vtkPlane>::New();
                opposite_plane->SetOrigin(vtk_plane->GetOrigin());
                double normal[3];
                vtk_plane->GetNormal(normal);
                normal[0] = -normal[0];
                normal[1] = -normal[1];
                normal[2] = -normal[2];
                opposite_plane->SetNormal(normal);
                // clipped_mapper->AddClippingPlane(opposite_plane);
        */
      }
    }
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolygonalSurfacePointPlacer> Viewer::get_point_placer() { return point_placer_; }

//-----------------------------------------------------------------------------
void Viewer::update_landmarks() { landmark_widget_->update_landmarks(); }

//-----------------------------------------------------------------------------
void Viewer::update_planes() {
  update_actors();
  update_clipping_planes();
  plane_widget_->update();
}

//-----------------------------------------------------------------------------
void Viewer::update_ffc_mode() {}

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkActor>> Viewer::get_surface_actors() { return surface_actors_; }

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkActor>> Viewer::get_clipped_surface_actors() { return clipped_surface_actors_; }

//-----------------------------------------------------------------------------
MeshGroup Viewer::get_meshes() { return meshes_; }

//-----------------------------------------------------------------------------
void Viewer::display_shape(QSharedPointer<Shape> shape) {
  visible_ = true;

  shape_ = shape;

  meshes_ = shape->get_meshes(visualizer_->get_display_mode());

  if (!meshes_.valid() && loading_displayed_) {
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

  vtkSmartPointer<vtkRenderer> ren = renderer_;

  ren->RemoveAllViewProps();

  if (!meshes_.valid()) {
    mesh_ready_ = false;
    viewer_ready_ = false;
    loading_displayed_ = true;
    update_points();
  } else {
    loading_displayed_ = false;
    viewer_ready_ = true;

    number_of_domains_ = meshes_.meshes().size();
    initialize_surfaces();

    for (size_t i = 0; i < meshes_.meshes().size(); i++) {
      MeshHandle mesh = meshes_.meshes()[i];

      vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

      auto feature_map = get_displayed_feature_map();

      std::vector<Shape::Point> vecs = shape_->get_vectors();
      if (!vecs.empty()) {
        feature_map = "";
      }

      if (feature_map != "" && poly_data) {
        auto scalar_array = poly_data->GetPointData()->GetArray(feature_map.c_str());
        if (!scalar_array) {
          shape_->load_feature(visualizer_->get_display_mode(), feature_map);
        }
      }

      auto mapper = surface_mappers_[i];
      auto actor = surface_actors_[i];

      draw_exclusion_spheres(shape);

      auto transform = get_transform(visualizer_->get_alignment_domain(), i);
      if (Viewer::is_reverse(transform)) {  // if it's been reflected we need to reverse
        vtkSmartPointer<vtkReverseSense> reverse_filter = vtkSmartPointer<vtkReverseSense>::New();
        reverse_filter->SetInputData(poly_data);
        reverse_filter->ReverseNormalsOff();
        reverse_filter->ReverseCellsOn();
        reverse_filter->Update();
        poly_data = reverse_filter->GetOutput();
      }
      actor->SetUserTransform(transform);
      clipped_surface_actors_[i]->SetUserTransform(transform);
      mapper->SetInputData(poly_data);
      clipped_surface_mappers_[i]->SetInputData(poly_data);

      int domain_scheme = (scheme_ + i) % color_schemes_.size();

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
          update_difference_lut(range[0], range[1]);
          mapper->SetScalarRange(range[0], range[1]);
          visualizer_->update_feature_range(range);
        }
      } else {
        mapper->ScalarVisibilityOff();
      }
    }
    update_points();

    display_vector_field();
  }

  update_image_volume();

  update_clipping_planes();
  update_actors();
  update_glyph_properties();
  landmark_widget_->clear_landmarks();
  plane_widget_->clear_planes();
  update_landmarks();
  update_planes();
  ren->AddViewProp(corner_annotation_);
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
    sum_range += bounds[5] - bounds[3];
    double average_range = sum_range / 3.0;
    // sanity clamp
    glyph_size_ = std::max<double>(glyph_size_, average_range * 0.03);
    glyph_size_ = std::min<double>(glyph_size_, average_range * 0.25);
  }

  glyphs_->SetScaleFactor(glyph_size_);
  arrow_glyphs_->SetScaleFactor(glyph_size_);

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
void Viewer::set_show_landmarks(bool show) {
  show_landmarks_ = show;
  update_landmarks();
}

//-----------------------------------------------------------------------------
bool Viewer::get_show_landmarks() { return show_landmarks_; }

//-----------------------------------------------------------------------------
void Viewer::update_points() {
  if (!shape_) {
    return;
  }

  Eigen::VectorXd correspondence_points;
  if (visualizer_->get_display_mode() == Visualizer::MODE_RECONSTRUCTION_C) {
    correspondence_points = shape_->get_global_correspondence_points_for_display();
  } else {
    correspondence_points = shape_->get_local_correspondence_points();
  }

  int num_points = correspondence_points.size() / 3;

  vtkFloatArray* scalars = (vtkFloatArray*)(glyph_point_set_->GetPointData()->GetScalars());

  Eigen::VectorXf scalar_values;
  if (showing_feature_map()) {
    auto feature_map = get_displayed_feature_map();
    shape_->load_feature(visualizer_->get_display_mode(), feature_map);
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

  // glyph_actor_->SetUserTransform(get_transform(alignment_domain));

  bool reverse = false;
  if (visualizer_->get_display_mode() == Visualizer::MODE_ORIGINAL_C ||
      visualizer_->get_display_mode() == Visualizer::MODE_GROOMED_C) {
    if (visualizer_->get_center()) {
      auto transform = shape_->get_alignment(alignment_domain);
      reverse = Viewer::is_reverse(transform);
      glyph_actor_->SetUserTransform(transform);
    } else {
      if (!shape_->has_alignment()) {
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->DeepCopy(shape_->get_original_transform());
        transform->Inverse();
        reverse = Viewer::is_reverse(transform);
        glyph_actor_->SetUserTransform(transform);
      }
    }
  }

  if (reverse) {
    glyphs_->SetSourceConnection(reverse_sphere_->GetOutputPort());
  } else {
    glyphs_->SetSourceConnection(sphere_source_->GetOutputPort());
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
  point_placer_->GetPolys()->RemoveAllItems();
  for (size_t i = 0; i < surface_actors_.size(); i++) {
    renderer_->RemoveActor(surface_actors_[i]);
  }
  for (size_t i = 0; i < clipped_surface_actors_.size(); i++) {
    renderer_->RemoveActor(clipped_surface_actors_[i]);
  }

  if (show_glyphs_) {
    renderer_->AddActor(glyph_actor_);

    renderer_->AddActor(exclusion_sphere_actor_);
    if (arrows_visible_) {
      renderer_->AddActor(arrow_glyph_actor_);
    }

    if (arrows_visible_ || showing_feature_map()) {
      renderer_->AddActor(scalar_bar_actor_);
    }
  }

  if (show_surface_ && meshes_.valid()) {
    for (int i = 0; i < number_of_domains_; i++) {
      /*
      if (shape_->has_planes()) {
        renderer_->AddActor(surface_actors_[i]);
        renderer_->AddActor(clipped_surface_actors_[i]);
      } else {
        renderer_->AddActor(clipped_surface_actors_[i]);
        renderer_->AddActor(surface_actors_[i]);
      }
*/
      renderer_->AddActor(clipped_surface_actors_[i]);
      renderer_->AddActor(surface_actors_[i]);

      surface_actors_[i]->GetProperty()->BackfaceCullingOff();
      clipped_surface_actors_[i]->GetProperty()->BackfaceCullingOff();
      // cell_picker_->AddPickList(surface_actors_[i]);
      // prop_picker_->AddPickList(surface_actors_[i]);
      cell_picker_->AddPickList(clipped_surface_actors_[i]);
      prop_picker_->AddPickList(clipped_surface_actors_[i]);
      // point_placer_->AddProp(surface_actors_[i]);
      point_placer_->AddProp(clipped_surface_actors_[i]);
      point_placer_->GetPolys()->AddItem(meshes_.meshes()[i]->get_poly_data());
    }
  }

  slice_view_.update_renderer();

  update_opacities();
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

  for (int i = 0; i < clipped_surface_actors_.size(); i++) {
    if (cell_picker_->GetActor() == clipped_surface_actors_[i]) {
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
void Viewer::draw_exclusion_spheres(QSharedPointer<Shape> object) {
  QList<Shape::Point> centers = object->get_exclusion_sphere_centers();
  QList<double> radii = object->get_exclusion_sphere_radii();

  int num_points = centers.size();

  if (num_points <= 0) {
    exclusion_sphere_points_->Reset();
  } else {
    vtkUnsignedLongArray* scalars = (vtkUnsignedLongArray*)(exclusion_sphere_point_set_->GetPointData()->GetScalars());
    scalars->Reset();
    scalars->SetNumberOfTuples(num_points);

    exclusion_sphere_glyph_->SetRange(0.0, (double)num_points + 1);
    exclusion_sphere_mapper_->SetScalarRange(0.0, (double)num_points + 1.0);

    exclusion_sphere_points_->Reset();
    exclusion_sphere_points_->SetNumberOfPoints(num_points);

    for (int i = 0; i < num_points; i++) {
      Shape::Point p = centers[i];
      scalars->InsertValue(i, radii[i]);
      exclusion_sphere_points_->InsertPoint(i, p.x, p.y, p.z);
    }
  }

  exclusion_sphere_points_->Modified();
}

//-----------------------------------------------------------------------------
void Viewer::update_difference_lut(float r0, float r1) {
  float maxrange = 0;
  if (fabs(r0) > fabs(r1)) {
    maxrange = fabs(r0);
  } else {
    maxrange = fabs(r1);
  }

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
    clipped_surface_mappers_.resize(number_of_domains_);
    clipped_surface_actors_.resize(number_of_domains_);

    for (int i = 0; i < number_of_domains_; i++) {
      surface_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      surface_actors_[i] = vtkSmartPointer<vtkActor>::New();
      surface_actors_[i]->SetMapper(surface_mappers_[i]);
      clipped_surface_mappers_[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
      clipped_surface_mappers_[i]->ScalarVisibilityOff();

      clipped_surface_actors_[i] = vtkSmartPointer<vtkActor>::New();
      // clipped_surface_actors_[i]->GetProperty()->SetOpacity(0.5);
      clipped_surface_actors_[i]->GetProperty()->SetColor(0.45, 0.45, 0.45);
      clipped_surface_actors_[i]->SetMapper(clipped_surface_mappers_[i]);
    }
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
void Viewer::handle_key(int* click_pos, std::string key) { slice_view_.handle_key(key); }

//-----------------------------------------------------------------------------
void Viewer::set_window_and_level(double window, double level) { slice_view_.set_window_and_level(window, level); }

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
