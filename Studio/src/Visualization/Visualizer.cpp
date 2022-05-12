#include <Data/MeshManager.h>
#include <Data/Shape.h>
#include <Utils/StudioUtils.h>
#include <Visualization/Visualizer.h>
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWindowToImageFilter.h>
#include <Visualization/ColorMap.h>

#include <QColor>
#include <QPixmap>
#include <QThread>

namespace shapeworks {

//-----------------------------------------------------------------------------
Visualizer::Visualizer(Preferences& prefs) : preferences_(prefs) {
  QObject::connect(&preferences_, SIGNAL(glyph_properties_changed()), this, SLOT(update_viewer_properties()));

  show_glyphs_ = true;
  show_surface_ = true;

  glyph_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  selected_point_one_ = -1;
  selected_point_two_ = -1;
}

//-----------------------------------------------------------------------------
Visualizer::~Visualizer() {}

//-----------------------------------------------------------------------------
void Visualizer::set_lightbox(LightboxHandle lightbox) {
  lightbox_ = lightbox;
  lightbox_->set_visualizer(this);
}

//-----------------------------------------------------------------------------
void Visualizer::set_session(SessionHandle session) {
  session_ = session;
  lightbox_->set_session(session);
  connect(session_.data(), &Session::feature_range_changed, this, &Visualizer::handle_feature_range_changed);
  connect(session_.data(), &Session::landmarks_changed, this, &Visualizer::update_landmarks);
  connect(session_.data(), &Session::planes_changed, this, &Visualizer::update_planes);
  connect(session_.data(), &Session::image_slice_settings_changed, this,
          &Visualizer::handle_image_slice_settings_changed);
  connect(session_.data(), &Session::ffc_paint_mode_changed, this, &Visualizer::update_ffc_mode);
  connect(session_.data(), &Session::repaint, this, &Visualizer::redraw);
}

//-----------------------------------------------------------------------------
void Visualizer::set_display_mode(std::string mode) { session_->set_display_mode(mode); }

//-----------------------------------------------------------------------------
std::string Visualizer::get_display_mode() { return session_->get_display_mode(); }

//-----------------------------------------------------------------------------
void Visualizer::set_center(bool center) { center_ = center; }

//-----------------------------------------------------------------------------
void Visualizer::display_samples() {
  update_viewer_properties();
  QVector<QSharedPointer<Shape>> shapes = session_->get_shapes();
  display_shapes(shapes);
}

//-----------------------------------------------------------------------------
void Visualizer::update_samples() {
  foreach (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_points(); }
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::update_landmarks() {
  foreach (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_landmarks(); }
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::update_planes() {
  foreach (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_planes(); }
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::update_ffc_mode() {
  foreach (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_ffc_mode(); }
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::display_shapes(QVector<QSharedPointer<Shape>> shapes) {
  lightbox_->set_shapes(shapes);
  lightbox_->redraw();
  update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape(ShapeHandle shape) {
  QVector<ShapeHandle> shapes;
  shapes.push_back(shape);
  lightbox_->set_shapes(shapes);
  update_viewer_properties();
  // reset_camera();
  lightbox_->redraw();
  current_shape_ = shape->get_particles();
}

//-----------------------------------------------------------------------------
StudioParticles Visualizer::get_current_shape() {
  auto shapes = lightbox_->get_shapes();
  if (shapes.size() > 0) {
    return shapes[0]->get_particles();
  }
  StudioParticles particles;
  return particles;
}

//-----------------------------------------------------------------------------
vtkFloatArray* Visualizer::get_current_particle_scalars() {
  auto viewers = lightbox_->get_viewers();
  if (viewers.size() > 0) {
    return viewers[0]->get_particle_scalars();
  }
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Visualizer::get_current_particle_poly_data()
{
  auto viewers = lightbox_->get_viewers();
  if (viewers.size() > 0) {
    return viewers[0]->get_particle_poly_data();
  }
  return nullptr;

}

//-----------------------------------------------------------------------------
void Visualizer::handle_new_mesh() {
  lightbox_->handle_new_mesh();
  if (needs_camera_reset_) {
    reset_camera();
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Visualizer::get_current_mesh() {
  auto meshes = get_current_meshes_transformed();
  if (meshes.empty()) {
    return nullptr;
  }
  auto append = vtkSmartPointer<vtkAppendPolyData>::New();
  for (int domain = 0; domain < meshes.size(); domain++) {
    append->AddInputData(meshes[domain]);
  }
  append->Update();
  vtkSmartPointer<vtkPolyData> combined = append->GetOutput();
  return combined;
}

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkPolyData>> Visualizer::get_current_meshes_transformed() {
  std::vector<vtkSmartPointer<vtkPolyData>> list;
  auto shapes = lightbox_->get_shapes();
  if (shapes.size() > 0) {
    if (shapes[0]->get_meshes(get_display_mode()).valid()) {
      auto meshes = shapes[0]->get_meshes(get_display_mode()).meshes();

      for (int domain = 0; domain < meshes.size(); domain++) {
        if (!meshes[domain]->get_poly_data()) {
          return list;
        }
        // we have to transform each domain to its location in order to export an appended mesh
        auto filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        filter->SetTransform(get_transform(shapes[0], get_alignment_domain(), domain));
        filter->SetInputData(meshes[domain]->get_poly_data());
        filter->Update();
        list.push_back(filter->GetOutput());
      }
    }
  }
  return list;
}

//-----------------------------------------------------------------------------
void Visualizer::display_sample(int i) {
  update_viewer_properties();
  QVector<ShapeHandle> display_shapes;
  QVector<ShapeHandle> shapes = session_->get_shapes();
  if (i < shapes.size()) {
    display_shapes.push_back(shapes[i]);
  }

  lightbox_->set_shapes(display_shapes);
  lightbox_->redraw();
  update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_glyphs(bool show) { show_glyphs_ = show; }

//-----------------------------------------------------------------------------
void Visualizer::set_show_surface(bool show) { show_surface_ = show; }

//-----------------------------------------------------------------------------
void Visualizer::update_viewer_properties() {
  double size = preferences_.get_glyph_size();
  double quality = preferences_.get_glyph_quality();
  quality = std::max<double>(quality, 3);
  if (preferences_.get_glyph_auto_size()) {
    size = session_->get_auto_glyph_size();
  }

  ColorMaps color_maps;

  if (lightbox_) {
    foreach (ViewerHandle viewer, lightbox_->get_viewers()) {
      viewer->set_glyph_size_and_quality(size, quality);
      viewer->set_color_series(color_maps.get_color_map(preferences_.get_color_map()));
      viewer->set_show_glyphs(show_glyphs_);
      viewer->set_show_surface(show_surface_);
      viewer->set_color_scheme(preferences_.get_color_scheme());
    }

    lightbox_->set_orientation_marker(preferences_.get_orientation_marker_type(),
                                      preferences_.get_orientation_marker_corner());
    update_lut();

    lightbox_->redraw();
  }
  current_glyph_size_ = size;
}

//-----------------------------------------------------------------------------
void Visualizer::handle_feature_range_changed() {
  feature_manual_range_[0] = session_->get_feature_range_min();
  feature_manual_range_[1] = std::max<double>(feature_manual_range_[0], session_->get_feature_range_max());
  lightbox_->update_feature_range();
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::handle_image_slice_settings_changed() {
  lightbox_->update_interactor_style();

  if (lightbox_) {
    Q_FOREACH (ViewerHandle v, lightbox_->get_viewers()) { v->update_image_volume(); }
  }
  lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::update_lut() {
  int num_points = session_->get_num_particles();

  glyph_lut_->SetNumberOfTableValues(num_points + 1);
  glyph_lut_->SetTableRange(0.0, (double)num_points + 1.0);

  bool analysis_mode = session_->is_analysis_mode();

  auto good_bad = session_->get_good_bad_particles();
  bool good_bad_active = session_->get_show_good_bad_particles() && good_bad.size() == num_points && analysis_mode;

  bool single_point_selected_mode = analysis_mode && selected_point_one_ >= 0 && selected_point_two_ < 0;
  bool dual_point_selected_mode = analysis_mode && selected_point_one_ >= 0 && selected_point_two_ >= 0;

  if (single_point_selected_mode) {
    setup_single_selected_point_lut();
  } else if (dual_point_selected_mode) {
    // dual particle coloring mode
    for (int i = 0; i < num_points; i++) {
      glyph_lut_->SetTableValue(i, 0, 0, 0);

      if (selected_point_one_ == i) {
        glyph_lut_->SetTableValue(i, 1, 1, 1);
      }

      if (selected_point_two_ == i) {
        glyph_lut_->SetTableValue(i, 1, 1, 0);
      }
    }
  } else if (good_bad_active) {
    for (int i = 0; i < num_points; i++) {
      if (good_bad[i]) {
        glyph_lut_->SetTableValue(i, 0, 1, 0);
      } else {
        glyph_lut_->SetTableValue(i, 1, 0, 0);
      }
    }
  } else {
    // normal particle coloring mode
    glyph_lut_->ForceBuild();
  }

  glyph_lut_->Modified();
  lightbox_->set_glyph_lut(glyph_lut_);
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_one(int id) {
  selected_point_one_ = id;
  update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_two(int id) {
  selected_point_two_ = id;
  update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::compute_measurements() {
  if (selected_point_one_ >= 0 && selected_point_two_ >= 0) {
  }
}

//-----------------------------------------------------------------------------
void Visualizer::setup_single_selected_point_lut() {
  int num_points = session_->get_num_particles();

  // color by distance from the selected point

  int check = selected_point_one_ * 3 + 2;
  if (check >= cached_mean_.size()) {
    return;
  }
  double p1[3];
  p1[0] = cached_mean_[selected_point_one_ * 3 + 0];
  p1[1] = cached_mean_[selected_point_one_ * 3 + 1];
  p1[2] = cached_mean_[selected_point_one_ * 3 + 2];

  std::vector<double> distances;
  double max_distance = 0;
  for (int i = 0; i < cached_mean_.size() / 3.0; i++) {
    double p2[3];
    p2[0] = cached_mean_[i * 3 + 0];
    p2[1] = cached_mean_[i * 3 + 1];
    p2[2] = cached_mean_[i * 3 + 2];

    double distance = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
    distances.push_back(distance);
    if (distance > max_distance) {
      max_distance = distance;
    }
  }

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetTableRange(0, max_distance);
  lut->SetHueRange(0, 1);
  lut->SetSaturationRange(1, 1);
  lut->SetValueRange(1, 1);
  lut->Build();

  for (int i = 0; i < num_points; i++) {
    const unsigned char* color = lut->MapValue(max_distance - distances[i]);

    // glyph_lut_->SetTableValue( i, distances[i] / max_distance, 0, 1 );
    glyph_lut_->SetTableValue(i, color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);

    if (selected_point_one_ == i) {
      glyph_lut_->SetTableValue(i, 1, 1, 1);
    }

    if (selected_point_two_ == i) {
      glyph_lut_->SetTableValue(i, 0, 0, 0);
    }
  }
}

//-----------------------------------------------------------------------------
void Visualizer::set_mean(const Eigen::VectorXd& mean) { cached_mean_ = mean; }

//-----------------------------------------------------------------------------
void Visualizer::reset_camera() {
  needs_camera_reset_ = false;
  if (lightbox_) {
    auto trans = lightbox_->initPos();
    Q_FOREACH (ViewerHandle v, lightbox_->get_viewers()) {
      if (!v->is_viewer_ready()) {
        needs_camera_reset_ = true;
      }
      v->reset_camera(trans);
    }
  }

  update_viewer_properties();
}

//-----------------------------------------------------------------------------
const std::string& Visualizer::get_feature_map() const { return feature_map_; }

//-----------------------------------------------------------------------------
void Visualizer::set_feature_map(const std::string& feature_map) {
  feature_map_ = feature_map;
  reset_feature_range();
}

//-----------------------------------------------------------------------------
bool Visualizer::get_center() { return center_; }

//-----------------------------------------------------------------------------
void Visualizer::set_alignment_domain(int domain) { alignment_domain_ = domain; }

//-----------------------------------------------------------------------------
int Visualizer::get_alignment_domain() { return alignment_domain_; }

//-----------------------------------------------------------------------------
void Visualizer::clear_viewers() {
  QVector<ShapeHandle> shapes;
  lightbox_->set_shapes(shapes);
}

//-----------------------------------------------------------------------------
void Visualizer::reset_feature_range() { feature_range_valid_ = false; }

//-----------------------------------------------------------------------------
double* Visualizer::get_feature_range() {
  if (session_->get_feature_auto_scale()) {
    return feature_range_;
  } else {
    return feature_manual_range_;
  }
}

//-----------------------------------------------------------------------------
double* Visualizer::get_feature_raw_range() { return feature_range_; }

//-----------------------------------------------------------------------------
bool Visualizer::get_feature_range_valid() { return feature_range_valid_; }

//-----------------------------------------------------------------------------
void Visualizer::update_feature_range(double* range) { update_feature_range(range[0], range[1]); }

//-----------------------------------------------------------------------------
void Visualizer::update_feature_range(double min, double max) {
  if (!feature_range_valid_) {
    feature_range_[0] = min;
    feature_range_[1] = max;
  }
  feature_range_[0] = std::min(feature_range_[0], min);
  feature_range_[1] = std::max(feature_range_[1], max);
  feature_range_valid_ = true;
  lightbox_->update_feature_range();
}

//-----------------------------------------------------------------------------
void Visualizer::set_uniform_feature_range(bool value) { feature_range_uniform_ = value; }

//-----------------------------------------------------------------------------
bool Visualizer::get_uniform_feature_range(void) { return feature_range_uniform_; }

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Visualizer::get_transform(QSharedPointer<Shape> shape, int alignment_domain, int domain) {
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

  if (get_display_mode() == Session::MODE_ORIGINAL_C) {
    if (get_center()) {
      transform = shape->get_transform(alignment_domain);
    }
  } else if (get_display_mode() == Session::MODE_GROOMED_C) {
    if (get_center()) {
      transform = shape->get_alignment(alignment_domain);
    }
  } else {
    transform = shape->get_reconstruction_transform(domain);
  }

  return transform;
}

//-----------------------------------------------------------------------------
void Visualizer::set_opacities(std::vector<float> opacities) {
  opacities_ = opacities;
  if (lightbox_) {
    foreach (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_opacities(); }
    lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
std::vector<float> Visualizer::get_opacities() { return opacities_; }

//-----------------------------------------------------------------------------
double Visualizer::get_current_glyph_size() { return current_glyph_size_; }

//-----------------------------------------------------------------------------
void Visualizer::handle_ctrl_click(PickResult result) { session_->handle_ctrl_click(result); }

//-----------------------------------------------------------------------------
void Visualizer::redraw() { lightbox_->redraw(); }

//-----------------------------------------------------------------------------
QPixmap Visualizer::export_to_pixmap(QSize size, bool transparent_background, bool show_orientation_marker,
                                     bool show_color_scale, bool& ready) {
  auto render_window = lightbox_->get_render_window();

  ready = true;
  Q_FOREACH (ViewerHandle viewer, lightbox_->get_viewers()) {
    if (viewer->get_shape() && !viewer->is_ready()) {
      ready = false;
    }
  };

  auto window_to_image_filter = vtkSmartPointer<vtkWindowToImageFilter>::New();

  int original_size[2];
  original_size[0] = render_window->GetSize()[0];
  original_size[1] = render_window->GetSize()[1];
  auto off_render_window = vtkSmartPointer<vtkRenderWindow>::New();
  off_render_window->OffScreenRenderingOn();
  off_render_window->SetNumberOfLayers(render_window->GetNumberOfLayers());
  off_render_window->SwapBuffersOff();

  window_to_image_filter->SetInput(off_render_window);
  window_to_image_filter->ReadFrontBufferOff();  // read from the back buffer

  if (transparent_background) {
    // If the background color is transparent then add alpha channel to output image.
    off_render_window->SetAlphaBitPlanes(1);  // Enable usage of alpha channel
    window_to_image_filter->SetInputBufferTypeToRGBA();
  }

  if (!show_orientation_marker) {
    lightbox_->set_orientation_marker(Preferences::OrientationMarkerType::none,
                                      Preferences::OrientationMarkerCorner::upper_right);
  }

  if (!show_color_scale) {
    Q_FOREACH (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->remove_scalar_bar(); }
  }

  vtkRendererCollection* collection = render_window->GetRenderers();
  collection->InitTraversal();
  vtkRenderer* renderer = collection->GetNextItem();

  while (renderer) {
    off_render_window->AddRenderer(renderer);
    renderer->SetRenderWindow(off_render_window);
    renderer->Modified();
    renderer = collection->GetNextItem();
  }

  off_render_window->SetSize(size.width(), size.height());
  off_render_window->Modified();
  off_render_window->Render();

  window_to_image_filter->Update();
  auto qimage = StudioUtils::vtk_image_to_qimage(window_to_image_filter->GetOutput());

  // set back to the original render window
  collection->InitTraversal();
  renderer = collection->GetNextItem();

  while (renderer) {
    off_render_window->RemoveRenderer(renderer);
    renderer->SetRenderWindow(render_window);
    renderer = collection->GetNextItem();
  }

  // restore changes
  update_viewer_properties();
  Q_FOREACH (ViewerHandle viewer, lightbox_->get_viewers()) { viewer->update_actors(); }

  return QPixmap::fromImage(qimage);
}

//-----------------------------------------------------------------------------
QSize Visualizer::get_render_size() {
  auto render_window = lightbox_->get_render_window();
  int* size = render_window->GetSize();
  return QSize(size[0], size[1]);
}
//-----------------------------------------------------------------------------

}  // namespace shapeworks
