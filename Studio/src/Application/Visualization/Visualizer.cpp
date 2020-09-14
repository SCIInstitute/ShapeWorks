#include <vtkMath.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <Visualization/Visualizer.h>
#include <Data/Shape.h>
#include <Data/MeshManager.h>
#include <QThread>

const std::string Visualizer::MODE_ORIGINAL_C("Original");
const std::string Visualizer::MODE_GROOMED_C("Groomed");
const std::string Visualizer::MODE_RECONSTRUCTION_C("Reconstructed");

//-----------------------------------------------------------------------------
Visualizer::Visualizer(Preferences& prefs) : preferences_(prefs)
{
  this->display_mode_ = Visualizer::MODE_ORIGINAL_C;

  QObject::connect(
    &preferences_, SIGNAL(glyph_properties_changed()),
    this, SLOT(update_viewer_properties()));

  this->show_glyphs_ = true;
  this->show_surface_ = true;

  this->glyph_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  this->selected_point_one_ = -1;
  this->selected_point_two_ = -1;
}

//-----------------------------------------------------------------------------
Visualizer::~Visualizer()
{}

//-----------------------------------------------------------------------------
void Visualizer::set_lightbox(LightboxHandle lightbox)
{
  this->lightbox_ = lightbox;
  this->lightbox_->set_visualizer(this);
}

//-----------------------------------------------------------------------------
void Visualizer::set_session(SessionHandle session)
{
  this->session_ = session;
}

//-----------------------------------------------------------------------------
void Visualizer::set_display_mode(std::string mode)
{
  this->display_mode_ = mode;
}

//-----------------------------------------------------------------------------
std::string Visualizer::get_display_mode()
{
  return this->display_mode_;
}

//-----------------------------------------------------------------------------
void Visualizer::set_center(bool center)
{
  std::cerr << "set center to: " << center << "\n";
  this->center_ = center;
}

//-----------------------------------------------------------------------------
void Visualizer::display_samples()
{
  this->update_viewer_properties();
  QVector<QSharedPointer<Shape>> shapes = this->session_->get_shapes();
  this->lightbox_->set_shapes(shapes);
  this->lightbox_->redraw();
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
void Visualizer::update_samples()
{
  QVector<QSharedPointer<Shape >> shapes = this->session_->get_shapes();
    foreach(ViewerHandle viewer, this->lightbox_->get_viewers()) {
      viewer->update_points();
    }
  this->lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape(const vnl_vector<double>& points)
{
  std::vector<Point> empty_vectors;
  this->display_shape(points, empty_vectors);
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape(const vnl_vector<double>& points, const std::vector<Point>& vectors)
{
  QVector<ShapeHandle> shapes;
  shapes.push_back(this->create_display_object(points, vectors));
  this->lightbox_->set_shapes(shapes);
  this->update_viewer_properties();
  //this->reset_camera();
  this->lightbox_->redraw();
  this->current_shape_ = points;
}

//-----------------------------------------------------------------------------
void Visualizer::display_shape(ShapeHandle shape)
{
  QVector<ShapeHandle> shapes;
  shapes.push_back(shape);
  this->lightbox_->set_shapes(shapes);
  this->update_viewer_properties();
  //this->reset_camera();
  this->lightbox_->redraw();
  this->current_shape_ = shape->get_global_correspondence_points();
}

//-----------------------------------------------------------------------------
vnl_vector<double> Visualizer::getCurrentShape()
{
  return this->current_shape_;
}

//-----------------------------------------------------------------------------
void Visualizer::handle_new_mesh()
{
  this->lightbox_->handle_new_mesh();
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Visualizer::get_current_mesh()
{
  auto shapes = this->lightbox_->get_shapes();
  if (shapes.size() > 0) {
    std::cerr << "returning something nice\n";
    return shapes[0]->get_mesh(this->display_mode_)->get_poly_data();
  }
  return nullptr;
}

//-----------------------------------------------------------------------------
void Visualizer::display_sample(int i)
{
  this->update_viewer_properties();
  QVector<ShapeHandle> display_shapes;
  QVector<ShapeHandle> shapes = this->session_->get_shapes();
  if (i < shapes.size()) {
    display_shapes.push_back(shapes[i]);
  }

  this->lightbox_->set_shapes(display_shapes);
  this->lightbox_->redraw();
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
ShapeHandle Visualizer::create_display_object(const vnl_vector<double>& points,
                                              const std::vector<Point>& vectors)
{

  MeshHandle mesh = this->session_->get_mesh_manager()->get_mesh(points);

  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(this->session_->get_mesh_manager());
  shape->set_reconstructed_mesh(mesh);
  shape->set_global_particles(points);
  shape->set_vectors(vectors);

  QStringList annotations;
  //annotations for the 4 corners of the view box
  ///annotations << "computed shape";
  annotations << "";
  annotations << "";
  annotations << "";
  annotations << "";
  shape->set_annotations(annotations);

  return shape;
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_glyphs(bool show)
{
  this->show_glyphs_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::set_show_surface(bool show)
{
  this->show_surface_ = show;
}

//-----------------------------------------------------------------------------
void Visualizer::update_viewer_properties()
{
  double size = this->preferences_.get_glyph_size();
  double quality = this->preferences_.get_glyph_quality();

  if (this->lightbox_) {
      foreach(ViewerHandle viewer, this->lightbox_->get_viewers()) {
        viewer->set_glyph_size_and_quality(size, quality);
        viewer->set_show_glyphs(this->show_glyphs_);
        viewer->set_show_surface(this->show_surface_);
        viewer->set_color_scheme(this->preferences_.get_color_scheme());
      }

    this->update_lut();

    this->lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
void Visualizer::update_lut()
{
  int num_points = this->cached_mean_.size() / 3;

  /// TMP
  num_points = 512;

  this->glyph_lut_->SetNumberOfTableValues(num_points + 1);
  this->glyph_lut_->SetTableRange(0.0, (double) num_points + 1.0);

  if (this->selected_point_one_ < 0) {
    this->glyph_lut_->ForceBuild();
  }
  else {

    if (this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0) {
      // measurement mode

      for (int i = 0; i < num_points; i++) {
        this->glyph_lut_->SetTableValue(i, 0, 0, 0);

        if (this->selected_point_one_ == i) {
          this->glyph_lut_->SetTableValue(i, 1, 1, 1);
        }

        if (this->selected_point_two_ == i) {
          this->glyph_lut_->SetTableValue(i, 1, 1, 0);
        }
      }
    }
    else {
      // color by distance from the selected point

      double p1[3];
      p1[0] = this->cached_mean_[this->selected_point_one_ * 3 + 0];
      p1[1] = this->cached_mean_[this->selected_point_one_ * 3 + 1];
      p1[2] = this->cached_mean_[this->selected_point_one_ * 3 + 2];

      std::vector<double> distances;
      double max_distance = 0;
      for (int i = 0; i < num_points; i++) {

        double p2[3];
        p2[0] = this->cached_mean_[i * 3 + 0];
        p2[1] = this->cached_mean_[i * 3 + 1];
        p2[2] = this->cached_mean_[i * 3 + 2];

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

        //this->glyph_lut_->SetTableValue( i, distances[i] / max_distance, 0, 1 );
        this->glyph_lut_->SetTableValue(i, color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);

        if (this->selected_point_one_ == i) {
          this->glyph_lut_->SetTableValue(i, 1, 1, 1);
        }

        if (this->selected_point_two_ == i) {
          this->glyph_lut_->SetTableValue(i, 0, 0, 0);
        }
      }
    }
  }

  this->glyph_lut_->Modified();
  this->lightbox_->set_glyph_lut(this->glyph_lut_);
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_one(int id)
{
  this->selected_point_one_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::set_selected_point_two(int id)
{
  this->selected_point_two_ = id;
  this->update_lut();
}

//-----------------------------------------------------------------------------
void Visualizer::compute_measurements()
{
  if (this->selected_point_one_ >= 0 && this->selected_point_two_ >= 0) {}
}

//-----------------------------------------------------------------------------
void Visualizer::set_mean(const vnl_vector<double>& mean)
{
  this->cached_mean_ = mean;
}

//-----------------------------------------------------------------------------
void Visualizer::reset_camera()
{
  if (this->lightbox_) {
    auto trans = this->lightbox_->initPos();
    for (auto a : this->lightbox_->get_viewers()) {
      a->reset_camera(trans);
    }
  }
  this->update_viewer_properties();
}

//-----------------------------------------------------------------------------
const std::string& Visualizer::get_feature_map() const
{
  return feature_map_;
}

//-----------------------------------------------------------------------------
void Visualizer::set_feature_map(const std::string& feature_map)
{
  this->feature_map_ = feature_map;
}

//-----------------------------------------------------------------------------
bool Visualizer::get_center()
{
  return this->center_;
}

//-----------------------------------------------------------------------------
void Visualizer::clear_viewers()
{
  QVector<ShapeHandle> shapes;
  this->lightbox_->set_shapes(shapes);
}


