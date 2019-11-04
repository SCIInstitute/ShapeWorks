#include <QPixmap>

#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Sparse>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageData.h>

#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>
#include <Visualization/StudioInteractorStyle.h>
#include <Visualization/Visualizer.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

//-----------------------------------------------------------------------------
Lightbox::Lightbox()
{
  this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->camera_ = this->renderer_->GetActiveCamera();
  this->render_window_ = NULL;

  this->tile_layout_width_ = 4;
  this->tile_layout_height_ = 4;
  this->start_row_ = 0;

  this->first_draw_ = true;

  this->style_ = vtkSmartPointer<StudioInteractorStyle>::New();
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
  this->timer_callback_count_ = 0;
  this->loading_timer_.start();
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
void Lightbox::insert_object_into_viewer(QSharedPointer<DisplayObject> object, int position)
{
  if (position >= this->viewers_.size()) {
    return;
  }

  QSharedPointer<Viewer> viewer = this->viewers_[position];

  viewer->display_object(object);

  if (this->first_draw_) {
    this->first_draw_ = false;
    viewer->get_renderer()->ResetCamera();
    auto pos = viewer->get_renderer()->GetActiveCamera()->GetPosition();
    this->initPos_ = { { pos[0], pos[1], pos[2] } };
  }
}

//-----------------------------------------------------------------------------
std::array<double, 3> Lightbox::initPos()
{
  return this->initPos_;
}

//-----------------------------------------------------------------------------
void Lightbox::clear_renderers()
{
  for (int i = 0; i < this->viewers_.size(); i++) {
    this->viewers_[i]->get_renderer()->RemoveAllViewProps();
  }
}

//-----------------------------------------------------------------------------
void Lightbox::display_objects()
{
  //this->clear_renderers();

  for (int i = 0; i < this->viewers_.size(); i++) {
    this->viewers_[i]->clear_viewer();
  }

  // skip based on scrollbar
  int start_object = this->start_row_ * this->tile_layout_width_;

  int position = 0;
  for (int i = start_object; i < this->objects_.size(); i++) {
    this->insert_object_into_viewer(this->objects_[i], position);
    position++;
  }
}

//-----------------------------------------------------------------------------
void Lightbox::set_render_window(vtkRenderWindow* renderWindow)
{
  this->render_window_ = renderWindow;
  this->render_window_->AddRenderer(this->renderer_);

  this->render_window_->GetInteractor()->SetInteractorStyle(this->style_);

  this->setup_renderers();
}

//-----------------------------------------------------------------------------
void Lightbox::setup_renderers()
{
  /*for ( int i = 0; i < this->viewers_.size(); i++ )
     {
     this->render_window_->RemoveRenderer( this->viewers_[i]->get_renderer() );
     }*/

  //this->viewers_.clear();

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  int total = width * height;

  float margin = 0.005;

  float tile_height = (1.0f - (margin * (height + 1))) / height;
  float tile_width = (1.0f - (margin * (width + 1))) / width;

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;

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
      renderer->SetActiveCamera(this->camera_);

      // create/ set the viewer
      QSharedPointer<Viewer> viewer;
      if (i >= this->viewers_.size()) {
        viewer = QSharedPointer<Viewer>(new Viewer());
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
  this->display_objects();
}

//-----------------------------------------------------------------------------
int Lightbox::get_num_rows()
{
  return std::ceil((float)this->objects_.size() / (float)this->tile_layout_width_);
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
  this->display_objects();
  this->render_window_->Render();
}

//-----------------------------------------------------------------------------
void Lightbox::set_display_objects(QVector < QSharedPointer < DisplayObject >> objects)
{
  this->objects_ = objects;
  this->display_objects();
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

  foreach(ViewerHandle viewer, this->get_viewers()) {
    viewer->set_loading_screen(this->spinner_images_[this->timer_callback_count_]);
  }
  this->renderer_->ResetCameraClippingRange();
  this->renderer_->GetRenderWindow()->Render();
}
