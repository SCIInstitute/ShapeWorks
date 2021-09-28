#include <vtkMath.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkAppendPolyData.h>
#include <vtkTransformPolyDataFilter.h>

#include <Visualization/Visualizer.h>
#include <Data/Shape.h>
#include <Data/MeshManager.h>
#include <QThread>

namespace shapeworks {

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
  connect(this->session_.data(), &Session::feature_range_changed, this, &Visualizer::handle_feature_range_changed);
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
  this->center_ = center;
}

//-----------------------------------------------------------------------------
void Visualizer::display_samples()
{
  this->update_viewer_properties();
  QVector<QSharedPointer<Shape>> shapes = this->session_->get_shapes();
  this->display_shapes(shapes);
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
void Visualizer::display_shapes(QVector<QSharedPointer<Shape>> shapes)
{
  this->lightbox_->set_shapes(shapes);
  this->lightbox_->redraw();
  this->update_viewer_properties();
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
  this->current_shape_ = shape->get_particles();
}

//-----------------------------------------------------------------------------
StudioParticles Visualizer::get_current_shape()
{
  auto shapes = this->lightbox_->get_shapes();
  if (shapes.size() > 0) {
    return shapes[0]->get_particles();
  }
  StudioParticles particles;
  return particles;
}

//-----------------------------------------------------------------------------
void Visualizer::handle_new_mesh()
{
  this->lightbox_->handle_new_mesh();
  if (this->needs_camera_reset_) {
    this->reset_camera();
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Visualizer::get_current_mesh()
{
  auto meshes = this->get_current_meshes_transformed();
  if (meshes.empty()) {
    return nullptr;
  }
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for (int domain = 0; domain < meshes.size(); domain++) {
    append->AddInputData(meshes[domain]);
  }
  append->Update();
  vtkSmartPointer<vtkPolyData> combined = append->GetOutput();
  return combined;
}

//-----------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkPolyData>> Visualizer::get_current_meshes_transformed()
{
  std::vector<vtkSmartPointer<vtkPolyData>> list;
  auto shapes = this->lightbox_->get_shapes();
  if (shapes.size() > 0) {
    if (shapes[0]->get_meshes(this->display_mode_).valid()) {
      auto meshes = shapes[0]->get_meshes(this->display_mode_).meshes();

      for (int domain = 0; domain < meshes.size(); domain++) {
        if (!meshes[domain]->get_poly_data()) {
          return list;
        }
        // we have to transform each domain to its location in order to export an appended mesh
        auto filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        filter->SetTransform(this->get_transform(shapes[0], this->get_alignment_domain(), domain));
        filter->SetInputData(meshes[domain]->get_poly_data());
        filter->Update();
        list.push_back(filter->GetOutput());
      }
    }
  }
  return list;
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
ShapeHandle Visualizer::create_display_object(const StudioParticles& points,
                                              const std::vector<Shape::Point>& vectors)
{
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(this->session_->get_mesh_manager());
  shape->set_particles(points);
  shape->set_vectors(vectors);
  shape->get_reconstructed_meshes();

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
  quality = std::max<double>(quality, 3);
  if (this->preferences_.get_glyph_auto_size()) {
    size = this->session_->get_auto_glyph_size();
  }

  if (this->lightbox_) {
    foreach(ViewerHandle viewer, this->lightbox_->get_viewers()) {
      viewer->set_glyph_size_and_quality(size, quality);
      viewer->set_show_glyphs(this->show_glyphs_);
      viewer->set_show_surface(this->show_surface_);
      viewer->set_color_scheme(this->preferences_.get_color_scheme());
    }

    this->lightbox_->set_orientation_marker(this->preferences_.get_orientation_marker_type(),
                                            this->preferences_.get_orientation_marker_corner());
    this->update_lut();

    this->lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
void Visualizer::handle_feature_range_changed()
{
  feature_manual_range_[0] = session_->get_feature_range_min();
  feature_manual_range_[1] = std::max<double>(feature_manual_range_[0], session_->get_feature_range_max());
  this->lightbox_->update_feature_range();
  this->lightbox_->redraw();
}

//-----------------------------------------------------------------------------
void Visualizer::update_lut()
{
  int num_points = this->cached_mean_.size() / 3;

  if (num_points < 1) {
    num_points = 512;
  }

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

      int check = this->selected_point_one_ * 3 + 2;
      if (check >= this->cached_mean_.size()) {
        return;
      }
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
  this->needs_camera_reset_ = false;
  if (this->lightbox_) {
    auto trans = this->lightbox_->initPos();
    for (auto a : this->lightbox_->get_viewers()) {
      if (!a->is_viewer_ready()) {
        this->needs_camera_reset_ = true;
      }
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
  this->reset_feature_range();
}

//-----------------------------------------------------------------------------
bool Visualizer::get_center()
{
  return this->center_;
}

//-----------------------------------------------------------------------------
void Visualizer::set_alignment_domain(int domain)
{
  this->alignment_domain_ = domain;
}

//-----------------------------------------------------------------------------
int Visualizer::get_alignment_domain()
{
  return this->alignment_domain_;
}

//-----------------------------------------------------------------------------
void Visualizer::clear_viewers()
{
  QVector<ShapeHandle> shapes;
  this->lightbox_->set_shapes(shapes);
}

//-----------------------------------------------------------------------------
void Visualizer::reset_feature_range()
{
  this->feature_range_valid_ = false;
}

//-----------------------------------------------------------------------------
double* Visualizer::get_feature_range()
{
  if (session_->get_feature_auto_scale()) {
    return this->feature_range_;
  }
  else {
    return this->feature_manual_range_;
  }
}

//-----------------------------------------------------------------------------
double* Visualizer::get_feature_raw_range()
{
  return this->feature_range_;
}

//-----------------------------------------------------------------------------
bool Visualizer::get_feature_range_valid()
{
  return this->feature_range_valid_;
}

//-----------------------------------------------------------------------------
void Visualizer::update_feature_range(double* range)
{
  if (!this->feature_range_valid_) {
    this->feature_range_[0] = range[0];
    this->feature_range_[1] = range[1];
  }
  this->feature_range_[0] = std::min(this->feature_range_[0], range[0]);
  this->feature_range_[1] = std::max(this->feature_range_[1], range[1]);
  this->feature_range_valid_ = true;
  this->lightbox_->update_feature_range();
}

//-----------------------------------------------------------------------------
void Visualizer::set_uniform_feature_range(bool value)
{
  this->feature_range_uniform_ = value;
}

//-----------------------------------------------------------------------------
bool Visualizer::get_uniform_feature_range(void)
{
  return feature_range_uniform_;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Visualizer::get_transform(QSharedPointer<Shape> shape, int alignment_domain, int domain)
{
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

  if (this->get_display_mode() == Visualizer::MODE_ORIGINAL_C) {
    if (this->get_center()) {
      transform = shape->get_transform(alignment_domain);
    }
  }
  else if (this->get_display_mode() == Visualizer::MODE_GROOMED_C) {
    if (this->get_center()) {
      transform = shape->get_alignment(alignment_domain);
    }
  }
  else {
    transform = shape->get_reconstruction_transform(domain);
  }

  return transform;
}

//-----------------------------------------------------------------------------
void Visualizer::set_opacities(std::vector<float> opacities)
{
  this->opacities_ = opacities;
  if (this->lightbox_) {
    foreach(ViewerHandle viewer, this->lightbox_->get_viewers()) {
      viewer->update_opacities();
    }
    this->lightbox_->redraw();
  }
}

//-----------------------------------------------------------------------------
std::vector<float> Visualizer::get_opacities()
{
  return this->opacities_;
}
}
