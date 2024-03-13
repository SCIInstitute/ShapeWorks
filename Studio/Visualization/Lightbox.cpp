#include <Logging.h>
#include <Shape.h>
#include <Visualization/Lightbox.h>
#include <Visualization/SliceView.h>
#include <Visualization/StudioInteractorStyle.h>
#include <Visualization/StudioSliceInteractorStyle.h>
#include <Visualization/Visualizer.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkBoundingBox.h>
#include <vtkImageData.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkProperty.h>
#include <vtkQImageToImageSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <QPixmap>

namespace shapeworks {

//-----------------------------------------------------------------------------
Lightbox::Lightbox() {
  renderer_ = vtkSmartPointer<vtkRenderer>::New();
  camera_ = renderer_->GetActiveCamera();

  style_ = vtkSmartPointer<StudioInteractorStyle>::New();
  slice_style_ = vtkSmartPointer<StudioSliceInteractorStyle>::New();
  orientation_marker_widget_ = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

  style_->AutoAdjustCameraClippingRangeOn();
  style_->set_lightbox(this);
  slice_style_->set_lightbox(this);

  // prepare the loading spinner
  QPixmap pixmap;
  pixmap.load(QString(":/Studio/Images/spinner.png"));
  for (int i = 0; i < 19; i++) {
    const int size = 126;
    QPixmap tile = pixmap.copy(0, i * size, 126, 126);
    QImage qimage = tile.toImage();

    vtkSmartPointer<vtkQImageToImageSource> qimage_to_image_source = vtkSmartPointer<vtkQImageToImageSource>::New();
    qimage_to_image_source->SetQImage(&qimage);
    qimage_to_image_source->Update();
    vtkImageData* image = qimage_to_image_source->GetOutput();

    spinner_images_.push_back(image);
  }

  QObject::connect(&loading_timer_, SIGNAL(timeout()), this, SLOT(handle_timer_callback()));

  loading_timer_.setInterval(50);
}

//-----------------------------------------------------------------------------
Lightbox::~Lightbox() {}

//-----------------------------------------------------------------------------
void Lightbox::set_interactor(vtkRenderWindowInteractor* interactor) { interactor_ = interactor; }

//-----------------------------------------------------------------------------
void Lightbox::insert_shape_into_viewer(std::shared_ptr<Shape> shape, int position) {
  if (position >= viewers_.size()) {
    return;
  }

  QSharedPointer<Viewer> viewer = viewers_[position];

  viewer->display_shape(shape);

  check_for_first_draw();
}

//-----------------------------------------------------------------------------
int Lightbox::get_start_shape() { return start_row_ * tile_layout_width_; }

//-----------------------------------------------------------------------------
std::array<double, 3> Lightbox::initPos() { return initPos_; }

//-----------------------------------------------------------------------------
void Lightbox::handle_new_mesh() {
  for (int i = 0; i < viewers_.size(); i++) {
    viewers_[i]->handle_new_mesh();
  }
  redraw();
  check_for_first_draw();
}

//-----------------------------------------------------------------------------
void Lightbox::reset_camera() {
  if (!viewers_.empty()) {
    viewers_[0]->reset_camera();
  }
  redraw();
}

//-----------------------------------------------------------------------------
void Lightbox::reset_camera_clipping_range() {
  vtkBoundingBox bbox;
  for (int i = 0; i < viewers_.size(); i++) {
    bbox.AddBox(viewers_[i]->get_renderer()->ComputeVisiblePropBounds());
  }
  double bounds[6];
  bbox.GetBounds(bounds);
  renderer_->ResetCameraClippingRange(bounds);
}

//-----------------------------------------------------------------------------
void Lightbox::clear_renderers() {
  for (int i = 0; i < viewers_.size(); i++) {
    viewers_[i]->clear_viewer();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::display_shapes() {
  for (int i = 0; i < viewers_.size(); i++) {
    viewers_[i]->clear_viewer();
  }

  // skip based on scrollbar
  int start_object = get_start_shape();

  int end_object = std::min<int>(start_object + viewers_.size(), shapes_.size());

  int position = 0;

  // bool need_loading_screen = false;
  for (int i = start_object; i < end_object; i++) {
    // std::cerr << "insert shape into viewer\n";
    insert_shape_into_viewer(shapes_[i], position);

    position++;
  }

  update_feature_range();
}

//-----------------------------------------------------------------------------
void Lightbox::set_render_window(vtkRenderWindow* renderWindow) {
  render_window_ = renderWindow;
  render_window_->AddRenderer(renderer_);
  render_window_->GetInteractor()->SetInteractorStyle(style_);

  auto interactor = renderer_->GetRenderWindow()->GetInteractor();
  orientation_marker_widget_->SetInteractor(interactor);
  if (orientation_marker_widget_->GetEnabled()) {
    orientation_marker_widget_->InteractiveOff();
  }

  setup_renderers();
}

//-----------------------------------------------------------------------------
void Lightbox::setup_renderers() {
  clear_renderers();
  for (int i = 0; i < viewers_.size(); i++) {
    render_window_->RemoveRenderer(viewers_[i]->get_renderer());
  }

  viewers_.clear();

  int width = tile_layout_width_;
  int height = tile_layout_height_;
  int total = width * height;
  float margin = 0.005;

  float tile_height = (1.0f - (margin * (height + 1))) / height;
  float tile_width = (1.0f - (margin * (width + 1))) / width;

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;

  set_orientation_marker_viewport();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width) + x;
      vtkSmartPointer<vtkRenderer> renderer;
      // create/get a renderer for the viewer
      if (i >= viewers_.size()) {
        renderer = vtkSmartPointer<vtkRenderer>::New();
        render_window_->AddRenderer(renderer);
      } else {
        renderer = viewers_[i]->get_renderer();
      }

      renderer->SetActiveCamera(camera_);

      // create/ set the viewer
      QSharedPointer<Viewer> viewer;
      if (i >= viewers_.size()) {
        viewer = QSharedPointer<Viewer>(new Viewer());
        viewer->set_visualizer(visualizer_);
        viewer->set_renderer(renderer);
        viewer->set_session(session_);
        viewers_.push_back(viewer);
      } else {
        viewer = viewers_[i];
      }
      viewer->set_loading_screen(spinner_images_[timer_callback_count_]);

      double viewport[4] = {0.0, 0.0, 0.0, 0.0};

      // horizontal
      viewport[0] = margin + (x * step_x);
      viewport[2] = viewport[0] + tile_width;

      // vertical
      viewport[1] = margin + (((height - 1) - y) * step_y);
      viewport[3] = viewport[1] + tile_height;

      renderer->SetViewport(viewport);
      renderer->DrawOn();

      double color = 0.2f;

      renderer->SetBackground(color, color, color);
    }
  }
  for (size_t i = total; i < viewers_.size(); i++) {
    viewers_[i]->get_renderer()->DrawOff();
  }

  render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_tile_layout(int width, int height) {
  if (width == tile_layout_width_ && height == tile_layout_height_) {
    return;
  }
  tile_layout_width_ = width;
  tile_layout_height_ = height;

  setup_renderers();
  display_shapes();
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows() { return std::ceil((float)shapes_.size() / (float)tile_layout_width_); }

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows_visible() { return tile_layout_height_; }

//-----------------------------------------------------------------------------
void Lightbox::set_start_row(int row) {
  start_row_ = row;
  display_shapes();
  render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_shapes(ShapeList shapes) {
  shapes_ = shapes;
  display_shapes();
}

//-----------------------------------------------------------------------------
ShapeList Lightbox::get_shapes() { return shapes_; }

//-----------------------------------------------------------------------------
void Lightbox::redraw() {
  if (render_window_) {
    // reset clipping range
    reset_camera_clipping_range();
    render_window_->Render();
  }
}

//-----------------------------------------------------------------------------
ViewerList Lightbox::get_viewers() { return viewers_; }

//-----------------------------------------------------------------------------
void Lightbox::handle_pick(int* click_pos, bool one, bool ctrl) {
  if (ctrl) {
    for (int i = 0; i < viewers_.size(); i++) {
      auto result = viewers_[i]->handle_ctrl_click(click_pos);
      if (result.domain_ != -1) {
        result.subject_ = i + get_start_shape();
        visualizer_->handle_ctrl_click(result);
        return;
      }
    }
  } else {
    int id = -1;
    Q_FOREACH (ViewerHandle viewer, viewers_) {
      int vid = viewer->handle_pick(click_pos);
      if (vid != -1) {
        id = vid;
      }
    }

    if (one) {
      visualizer_->set_selected_point_one(id);
    } else {
      visualizer_->set_selected_point_two(id);
    }
  }
}

//-----------------------------------------------------------------------------
void Lightbox::handle_hover(int* click_pos) {}

//-----------------------------------------------------------------------------
void Lightbox::handle_key(int* click_pos, std::string key) {
  if (key == "Up" || key == "Down") {
    SliceView::SliceChange change = SliceView::SliceChange::Up;
    if (key == "Down") {
      change = SliceView::SliceChange::Down;
    }

    if (session_->get_image_sync_slice()) {
      viewers_[0]->slice_view().change_slice(change);
      Point point = viewers_[0]->slice_view().get_slice_position();

      // transform from source shape to common space
      auto base_transform = viewers_[0]->get_shape()->get_transform();

      Point common;
      base_transform->TransformPoint(point.GetDataPointer(), common.GetDataPointer());

      for (int i = 1; i < viewers_.size(); i++) {
        if (!viewers_[i]->get_shape()) {
          continue;
        }

        // transform from common space to destination space
        auto inverse = viewers_[i]->get_shape()->get_inverse_transform();

        Point local;
        inverse->TransformPoint(common.GetDataPointer(), local.GetDataPointer());
        viewers_[i]->slice_view().set_slice_position(local);
      }
    } else {
      for (int i = 0; i < viewers_.size(); i++) {
        viewers_[i]->slice_view().change_slice(change);
      }
    }
    redraw();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::handle_right_click(int* click_pos, vtkRenderer* renderer) {
  for (int i = 0; i < viewers_.size(); i++) {
    if (viewers_[i]->get_renderer() == renderer) {
      if (viewers_[i]->get_shape()) {
        Q_EMIT right_click(i + get_start_shape());
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_glyph_lut(vtkSmartPointer<vtkLookupTable> lut) {
  Q_FOREACH (ViewerHandle viewer, viewers_) {
    viewer->set_glyph_lut(lut);
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_session(QSharedPointer<Session> session) {
  session_ = session;
  Q_FOREACH (ViewerHandle viewer, viewers_) {
    viewer->set_session(session);
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_visualizer(Visualizer* visualizer) { visualizer_ = visualizer; }

//-----------------------------------------------------------------------------
void Lightbox::handle_timer_callback() {
  timer_callback_count_ = (timer_callback_count_ + 1) % 19;
  Q_FOREACH (ViewerHandle viewer, get_viewers()) {
    viewer->set_loading_screen(spinner_images_[timer_callback_count_]);
  }
  renderer_->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::check_for_first_draw() {
  if (first_draw_) {
    int ready_viewer = -1;
    for (int i = 0; i < viewers_.size(); i++) {
      if (viewers_[i]->is_viewer_ready()) {
        ready_viewer = i;
      }
    }
    if (ready_viewer >= 0) {
      // std::cerr << "First draw!\n";
      first_draw_ = false;
      redraw();
      viewers_[ready_viewer]->get_renderer()->Render();
      viewers_[ready_viewer]->get_renderer()->ResetCamera();
      viewers_[ready_viewer]->get_renderer()->Render();
      viewers_[ready_viewer]->get_renderer()->ResetCamera();
      auto pos = viewers_[ready_viewer]->get_renderer()->GetActiveCamera()->GetPosition();
      initPos_ = {{pos[0], pos[1], pos[2]}};
    }
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_orientation_marker(Preferences::OrientationMarkerType type,
                                      Preferences::OrientationMarkerCorner corner) {
  if (!render_window_) {
    return;
  }
  if (type == current_orientation_marker_type_ && corner == current_orientation_marker_corner_) {
    return;
  }

  current_orientation_marker_type_ = type;
  current_orientation_marker_corner_ = corner;

  orientation_marker_widget_->EnabledOff();

  if (type == Preferences::OrientationMarkerType::none) {
    return;
  }

  if (type == Preferences::OrientationMarkerType::medical) {
    // create the orientation cube
    vtkSmartPointer<vtkAnnotatedCubeActor> cube_actor = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
    cube_actor->SetXPlusFaceText("L");
    cube_actor->SetXMinusFaceText("R");
    cube_actor->SetYPlusFaceText("P");
    cube_actor->SetYMinusFaceText("A");
    cube_actor->SetZPlusFaceText("S");
    cube_actor->SetZMinusFaceText("I");
    cube_actor->SetFaceTextScale(0.67);
    cube_actor->GetTextEdgesProperty()->SetColor(0.5, 0.5, 0.5);
    cube_actor->SetTextEdgesVisibility(1);
    cube_actor->SetCubeVisibility(1);
    cube_actor->SetFaceTextVisibility(1);
    vtkProperty* prop = cube_actor->GetXPlusFaceProperty();
    prop->SetColor(0, 0, 1);
    prop = cube_actor->GetXMinusFaceProperty();
    prop->SetColor(0, 0, 1);
    prop = cube_actor->GetYPlusFaceProperty();
    prop->SetColor(0, 1, 0);
    prop = cube_actor->GetYMinusFaceProperty();
    prop->SetColor(0, 1, 0);
    prop = cube_actor->GetZPlusFaceProperty();
    prop->SetColor(1, 0, 0);
    prop = cube_actor->GetZMinusFaceProperty();
    prop->SetColor(1, 0, 0);
    orientation_marker_widget_->SetOrientationMarker(cube_actor);
  } else if (type == Preferences::OrientationMarkerType::triad) {
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetShaftTypeToCylinder();
    axes->GetZAxisShaftProperty()->SetColor(0, 1, 0);
    axes->GetZAxisTipProperty()->SetColor(0, 1, 0);
    axes->GetYAxisShaftProperty()->SetColor(1, 1, 0);
    axes->GetYAxisTipProperty()->SetColor(1, 1, 0);
    orientation_marker_widget_->SetOrientationMarker(axes);
  }

  set_orientation_marker_viewport();

  orientation_marker_widget_->EnabledOn();
}

//-----------------------------------------------------------------------------
void Lightbox::set_orientation_marker_viewport() {
  if (!orientation_marker_widget_) {
    return;
  }

  int width = tile_layout_width_;
  int height = tile_layout_height_;
  float margin = 0.005;

  float tile_height = (1.0f - (margin * (height + 1))) / height;
  float tile_width = (1.0f - (margin * (width + 1))) / width;

  double cube_viewport[4];

  double view_height = tile_height * 0.2;
  double view_width = tile_width * 0.2;

  if (current_orientation_marker_corner_ == Preferences::OrientationMarkerCorner::lower_right) {
    cube_viewport[0] = 1.0 - view_width;
    cube_viewport[1] = 0;
    cube_viewport[2] = 1.0;
    cube_viewport[3] = view_height;
  } else if (current_orientation_marker_corner_ == Preferences::OrientationMarkerCorner::upper_left) {
    cube_viewport[0] = 0;
    cube_viewport[1] = 1.0 - view_height;
    cube_viewport[2] = view_width;
    cube_viewport[3] = 1.0;
  } else if (current_orientation_marker_corner_ == Preferences::OrientationMarkerCorner::lower_left) {
    cube_viewport[0] = 0;
    cube_viewport[1] = 0;
    cube_viewport[2] = view_width;
    cube_viewport[3] = view_height;
  } else {
    cube_viewport[0] = 1.0 - view_width;
    cube_viewport[1] = 1.0 - view_height;
    cube_viewport[2] = 1.0;
    cube_viewport[3] = 1.0;
  }
  orientation_marker_widget_->SetCurrentRenderer(renderer_);
  orientation_marker_widget_->SetViewport(cube_viewport);
}

//-----------------------------------------------------------------------------
void Lightbox::update_feature_range() {
  if (visualizer_->get_feature_map() != "" && visualizer_->get_uniform_feature_range()) {
    for (int i = 0; i < viewers_.size(); i++) {
      viewers_[i]->update_feature_range(visualizer_->get_feature_range());
    }
  }
}

//-----------------------------------------------------------------------------
void Lightbox::update_interactor_style() {
  if (!render_window_) {
    return;
  }

  auto current_style = render_window_->GetInteractor()->GetInteractorStyle();
  vtkInteractorObserver* new_style = nullptr;

  if (session_->get_image_name() == "-none-" || session_->get_image_3d_mode()) {
    new_style = style_;
  } else {
    new_style = slice_style_;
  }
  if (current_style != new_style) {
    render_window_->GetInteractor()->SetInteractorStyle(new_style);
    reset_camera();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_shared_brightness_and_contrast(double brightness, double contrast) {
  if (!session_->get_image_share_brightness_contrast()) {
    return;
  }
  for (int i = 0; i < viewers_.size(); i++) {
    viewers_[i]->slice_view().set_brightness_and_contrast(brightness, contrast);
  }
}

//-----------------------------------------------------------------------------
vtkRenderWindow* Lightbox::get_render_window() { return render_window_; }
//-----------------------------------------------------------------------------

}  // namespace shapeworks
