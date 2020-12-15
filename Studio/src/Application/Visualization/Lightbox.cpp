#include <QPixmap>

#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Sparse>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageData.h>
#include <vtkBoundingBox.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkProperty.h>

#include <Visualization/Lightbox.h>
#include <Visualization/StudioInteractorStyle.h>
#include <Visualization/Visualizer.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
Lightbox::Lightbox()
{
  this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->camera_ = this->renderer_->GetActiveCamera();

  this->style_ = vtkSmartPointer<StudioInteractorStyle>::New();
  this->orientation_marker_widget_ =
    vtkSmartPointer<vtkOrientationMarkerWidget>::New();

  this->style_->AutoAdjustCameraClippingRangeOn();
  this->style_->set_lightbox(this);

  // prepare the loading spinner
  QPixmap pixmap;
  pixmap.load(QString(":/Studio/Images/spinner.png"));
  for (int i = 0; i < 19; i++) {
    const int size = 126;
    QPixmap tile = pixmap.copy(0, i * size, 126, 126);
    QImage qimage = tile.toImage();

    vtkSmartPointer<vtkQImageToImageSource> qimage_to_image_source =
      vtkSmartPointer<vtkQImageToImageSource>::New();
    qimage_to_image_source->SetQImage(&qimage);
    qimage_to_image_source->Update();
    vtkImageData* image = qimage_to_image_source->GetOutput();

    this->spinner_images_.push_back(image);
  }

  QObject::connect(
    &this->loading_timer_, SIGNAL(timeout()),
    this, SLOT(handle_timer_callback()));

  this->loading_timer_.setInterval(50);
}

//-----------------------------------------------------------------------------
Lightbox::~Lightbox()
{}

//-----------------------------------------------------------------------------
void Lightbox::set_interactor(vtkRenderWindowInteractor* interactor)
{
  this->interactor_ = interactor;
}

//-----------------------------------------------------------------------------
void Lightbox::insert_shape_into_viewer(QSharedPointer<Shape> shape, int position)
{
  if (position >= this->viewers_.size()) {
    return;
  }

  QSharedPointer<Viewer> viewer = this->viewers_[position];

  viewer->display_shape(shape);

  this->check_for_first_draw();
}

//-----------------------------------------------------------------------------
std::array<double, 3> Lightbox::initPos()
{
  return this->initPos_;
}

//-----------------------------------------------------------------------------
void Lightbox::handle_new_mesh()
{
  for (int i = 0; i < this->viewers_.size(); i++) {
    this->viewers_[i]->handle_new_mesh();
  }
  this->redraw();
  this->check_for_first_draw();
}

//-----------------------------------------------------------------------------
void Lightbox::reset_camera()
{

  this->viewers_[0]->get_renderer()->ResetCameraClippingRange();
  this->viewers_[0]->get_renderer()->ResetCamera();

  //this->renderer_->ResetCamera();
//  this->get_re
//  for (int i = 0; i < this->viewers_.size(); i++) {
//    this->viewers_[i]->get_renderer()->RemoveAllViewProps();
//  }
}

//-----------------------------------------------------------------------------
void Lightbox::reset_camera_clipping_range()
{
  vtkBoundingBox bbox;
  for (int i = 0; i < this->viewers_.size(); i++) {
    bbox.AddBox(this->viewers_[i]->get_renderer()->ComputeVisiblePropBounds());
  }
  double bounds[6];
  bbox.GetBounds(bounds);
  this->renderer_->ResetCameraClippingRange(bounds);
}

//-----------------------------------------------------------------------------
void Lightbox::clear_renderers()
{
  for (int i = 0; i < this->viewers_.size(); i++) {
    this->viewers_[i]->get_renderer()->RemoveAllViewProps();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::display_shapes()
{
  //this->clear_renderers();

  for (int i = 0; i < this->viewers_.size(); i++) {
    this->viewers_[i]->clear_viewer();
  }

  // skip based on scrollbar
  int start_object = this->start_row_ * this->tile_layout_width_;

  int end_object = std::min<int>(start_object + this->viewers_.size(), this->shapes_.size());

  int position = 0;

  //bool need_loading_screen = false;
  for (int i = start_object; i < end_object; i++) {
    this->insert_shape_into_viewer(this->shapes_[i], position);
    //if (!this->viewers_[position]->is_viewer_ready()) {
    //need_loading_screen = true;
    //}
    position++;
  }

  if (this->visualizer_->get_feature_map() != "" &&
      this->visualizer_->get_uniform_feature_range()) {
    for (int i = 0; i < this->viewers_.size(); i++) {
      auto shape = this->viewers_[i]->get_shape();
      if (shape && shape->get_vectors().empty()) {
        this->viewers_[i]->update_feature_range(this->visualizer_->get_feature_range());
      }
    }
  }

  /*
     /// disabled for now
     ///
     if (need_loading_screen) {
     this->loading_timer_.start();
     }
     else {
     this->loading_timer_.stop();
     }
   */
}

//-----------------------------------------------------------------------------
void Lightbox::set_render_window(vtkRenderWindow* renderWindow)
{
  this->render_window_ = renderWindow;
  this->render_window_->AddRenderer(this->renderer_);

  this->render_window_->GetInteractor()->SetInteractorStyle(this->style_);

  this->orientation_marker_widget_->SetInteractor(
    this->renderer_->GetRenderWindow()->GetInteractor());
  this->orientation_marker_widget_->InteractiveOff();

  this->setup_renderers();
}

//-----------------------------------------------------------------------------
void Lightbox::setup_renderers()
{
  for (int i = 0; i < this->viewers_.size(); i++) {
    this->render_window_->RemoveRenderer(this->viewers_[i]->get_renderer());
  }

  this->viewers_.clear();

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  int total = width * height;
  float margin = 0.005;

  float tile_height = (1.0f - (margin * (height + 1))) / height;
  float tile_width = (1.0f - (margin * (width + 1))) / width;

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;

  this->set_orientation_marker_viewport();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width) + x;
      vtkSmartPointer<vtkRenderer> renderer;
      // create/get a renderer for the viewer
      if (i >= this->viewers_.size()) {
        renderer = vtkSmartPointer<vtkRenderer>::New();
        this->render_window_->AddRenderer(renderer);
      }
      else {
        renderer = this->viewers_[i]->get_renderer();
      }

      renderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->style_);

      renderer->SetActiveCamera(this->camera_);

      // create/ set the viewer
      QSharedPointer<Viewer> viewer;
      if (i >= this->viewers_.size()) {
        viewer = QSharedPointer<Viewer>(new Viewer());
        viewer->set_visualizer(this->visualizer_);
        viewer->set_renderer(renderer);
        this->viewers_.push_back(viewer);
      }
      else {
        viewer = this->viewers_[i];
      }
      viewer->set_loading_screen(this->spinner_images_[this->timer_callback_count_]);

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
  for (size_t i = total; i < this->viewers_.size(); i++) {
    this->viewers_[i]->get_renderer()->DrawOff();
  }

  if (this->render_window_->IsDrawable()) {
    this->render_window_->Render();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_tile_layout(int width, int height)
{
  this->tile_layout_width_ = width;
  this->tile_layout_height_ = height;

  this->setup_renderers();
  this->display_shapes();
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows()
{
  return std::ceil((float) this->shapes_.size() / (float) this->tile_layout_width_);
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows_visible()
{
  return this->tile_layout_height_;
}

//-----------------------------------------------------------------------------
void Lightbox::set_start_row(int row)
{
  this->start_row_ = row;
  this->display_shapes();
  this->render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_shapes(QVector<QSharedPointer<Shape>> shapes)
{
  this->shapes_ = shapes;
  this->display_shapes();
}

//-----------------------------------------------------------------------------
QVector<ShapeHandle> Lightbox::get_shapes()
{
  return this->shapes_;
}

//-----------------------------------------------------------------------------
void Lightbox::redraw()
{
  if (this->render_window_) {
    this->render_window_->Render();
  }
}

//-----------------------------------------------------------------------------
ViewerList Lightbox::get_viewers()
{
  return this->viewers_;
}

//-----------------------------------------------------------------------------
void Lightbox::handle_pick(int* click_pos, bool one)
{
  int id = -1;
    foreach(ViewerHandle viewer, this->viewers_) {
      int vid = viewer->handle_pick(click_pos);
      if (vid != -1) {
        id = vid;
      }
    }

  if (one) {
    this->visualizer_->set_selected_point_one(id);
  }
  else {
    this->visualizer_->set_selected_point_two(id);
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_glyph_lut(vtkSmartPointer<vtkLookupTable> lut)
{
    foreach(ViewerHandle viewer, this->viewers_) {
      viewer->set_lut(lut);
    }
}

//-----------------------------------------------------------------------------
void Lightbox::set_visualizer(Visualizer* visualizer)
{
  this->visualizer_ = visualizer;
}

//-----------------------------------------------------------------------------
void Lightbox::handle_timer_callback()
{
  this->timer_callback_count_ = (this->timer_callback_count_ + 1) % 19;

  //std::cerr << "timer!\n";
    foreach(ViewerHandle viewer, this->get_viewers()) {
      viewer->set_loading_screen(this->spinner_images_[this->timer_callback_count_]);
    }
  //this->renderer_->ResetCameraClippingRange();
  this->renderer_->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::check_for_first_draw()
{
  if (this->first_draw_) {
    int ready_viewer = -1;
    for (int i = 0; i < this->viewers_.size(); i++) {
      if (this->viewers_[i]->is_viewer_ready()) {
        ready_viewer = i;
      }
    }
    if (ready_viewer >= 0) {
      //std::cerr << "First draw!\n";
      this->first_draw_ = false;
      this->redraw();
      this->viewers_[ready_viewer]->get_renderer()->Render();
      this->viewers_[ready_viewer]->get_renderer()->ResetCamera();
      this->viewers_[ready_viewer]->get_renderer()->Render();
      this->viewers_[ready_viewer]->get_renderer()->ResetCamera();
      //this->viewers_[ready_viewer]->get_renderer()->ResetCamera();
      auto pos = this->viewers_[ready_viewer]->get_renderer()->GetActiveCamera()->GetPosition();
      this->initPos_ = {{pos[0], pos[1], pos[2]}};
/*
      for (int i = 0; i < this->viewers_.size(); i++) {

        this->viewers_[i]->get_renderer()->SetActiveCamera(this->camera_);
      }
 */
    }
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_orientation_marker(Preferences::OrientationMarkerType type,
                                      Preferences::OrientationMarkerCorner corner)
{
  if (!this->render_window_) {
    return;
  }
  if (type == this->current_orientation_marker_type_ &&
      corner == this->current_orientation_marker_corner_) {
    return;
  }

  this->current_orientation_marker_type_ = type;
  this->current_orientation_marker_corner_ = corner;

  this->orientation_marker_widget_->EnabledOff();

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
    this->orientation_marker_widget_->SetOrientationMarker(cube_actor);
  }
  else if (type == Preferences::OrientationMarkerType::triad) {
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetShaftTypeToCylinder();
    axes->GetZAxisShaftProperty()->SetColor(0, 1, 0);
    axes->GetZAxisTipProperty()->SetColor(0, 1, 0);
    axes->GetYAxisShaftProperty()->SetColor(1, 1, 0);
    axes->GetYAxisTipProperty()->SetColor(1, 1, 0);
    this->orientation_marker_widget_->SetOrientationMarker(axes);
  }

  this->set_orientation_marker_viewport();

  this->orientation_marker_widget_->EnabledOn();

}

//-----------------------------------------------------------------------------
void Lightbox::set_orientation_marker_viewport()
{
  if (!this->orientation_marker_widget_) {
    return;
  }

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  float margin = 0.005;

  float tile_height = (1.0f - (margin * (height + 1))) / height;
  float tile_width = (1.0f - (margin * (width + 1))) / width;

  double cube_viewport[4];

  double view_height = tile_height * 0.2;
  double view_width = tile_width * 0.2;

  if (this->current_orientation_marker_corner_ ==
      Preferences::OrientationMarkerCorner::lower_right) {
    cube_viewport[0] = 1.0 - view_width;
    cube_viewport[1] = 0;
    cube_viewport[2] = 1.0;
    cube_viewport[3] = view_height;
  }
  else if (this->current_orientation_marker_corner_ ==
           Preferences::OrientationMarkerCorner::upper_left) {
    cube_viewport[0] = 0;
    cube_viewport[1] = 1.0 - view_height;
    cube_viewport[2] = view_width;
    cube_viewport[3] = 1.0;
  }
  else if (this->current_orientation_marker_corner_ ==
           Preferences::OrientationMarkerCorner::lower_left) {
    cube_viewport[0] = 0;
    cube_viewport[1] = 0;
    cube_viewport[2] = view_width;
    cube_viewport[3] = view_height;
  }
  else {
    cube_viewport[0] = 1.0 - view_width;
    cube_viewport[1] = 1.0 - view_height;
    cube_viewport[2] = 1.0;
    cube_viewport[3] = 1.0;
  }
  this->orientation_marker_widget_->SetCurrentRenderer(this->renderer_);
  this->orientation_marker_widget_->SetViewport(cube_viewport);
}
}