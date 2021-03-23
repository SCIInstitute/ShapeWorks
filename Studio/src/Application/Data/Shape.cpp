#include <Data/Shape.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <itkOrientImageFilter.h>

#include <vtkCenterOfMass.h>

#include <Data/MeshGenerator.h>
#include <Data/StudioLog.h>
#include <Visualization/Visualizer.h>

using ReaderType = itk::ImageFileReader<ImageType>;

namespace shapeworks {

//---------------------------------------------------------------------------
Shape::Shape()
{
  this->id_ = 0;

  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
}

//---------------------------------------------------------------------------
Shape::~Shape() = default;

//---------------------------------------------------------------------------
MeshGroup Shape::get_meshes(const string& display_mode)
{
  if (display_mode == Visualizer::MODE_ORIGINAL_C) {
    return this->get_original_meshes();
  }
  else if (display_mode == Visualizer::MODE_GROOMED_C) {
    return this->get_groomed_meshes();
  }
  return this->get_reconstructed_meshes();
}

//---------------------------------------------------------------------------
void Shape::set_annotations(QStringList annotations, bool only_overwrite_blank)
{
  if (only_overwrite_blank && !this->corner_annotations_.empty() &&
      this->corner_annotations_[0] != "") {
    return; // don't override
  }
  this->corner_annotations_ = annotations;
}

//---------------------------------------------------------------------------
QStringList Shape::get_annotations()
{
  return this->corner_annotations_;
}

//---------------------------------------------------------------------------
void Shape::set_mesh_manager(QSharedPointer<MeshManager> mesh_manager)
{
  this->mesh_manager_ = mesh_manager;
}

//---------------------------------------------------------------------------
void Shape::set_subject(std::shared_ptr<Subject> subject)
{
  this->subject_ = subject;
  this->original_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  this->groomed_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  this->reconstructed_meshes_.set_number_of_meshes(subject->get_number_of_domains());

  if (!this->subject_->get_segmentation_filenames().empty()) {

    /// TODO: Show multiple lines of filenames for multiple domains?
    std::string filename = this->subject_->get_segmentation_filenames()[0];
    this->corner_annotations_[0] = QFileInfo(QString::fromStdString(filename)).fileName();
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<Subject> Shape::get_subject()
{
  return this->subject_;
}

//---------------------------------------------------------------------------
void Shape::import_original_image(const std::string& filename)
{
  this->subject_->set_segmentation_filenames(std::vector<std::string>{filename});
  this->corner_annotations_[0] = QFileInfo(QString::fromStdString(filename)).fileName();
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_original_meshes(bool wait)
{
  if (!this->subject_) {
    std::cerr << "Error: asked for original mesh when none is present!\n";
    assert(0);
  }

  if (!this->original_meshes_.valid()) {
    this->generate_meshes(this->subject_->get_segmentation_filenames(), this->original_meshes_,
                          true, wait);
  }
  return this->original_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_groomed_meshes(bool wait)
{
  if (!this->subject_) {
    std::cerr << "Error: asked for groomed meshes when none are present!\n";
    assert(0);
  }

  if (!this->groomed_meshes_.valid()) {
    this->generate_meshes(this->subject_->get_groomed_filenames(), this->groomed_meshes_,
                          true, wait);
  }
  return this->groomed_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_reconstructed_meshes(bool wait)
{
  if (!this->reconstructed_meshes_.valid()) {
    auto worlds = this->particles_.get_world_particles();
    for (int i = 0; i < worlds.size(); i++) {
      MeshHandle mesh = this->mesh_manager_->get_mesh(worlds[i], i);
      if (mesh) {
        this->reconstructed_meshes_.set_mesh(i, mesh);
      }
    }
  }
  return this->reconstructed_meshes_;
}

//---------------------------------------------------------------------------
void Shape::reset_groomed_mesh()
{
  this->groomed_mesh_.reset();
  this->groomed_meshes_ = MeshGroup(this->subject_->get_number_of_domains());
}

//---------------------------------------------------------------------------
void Shape::set_reconstructed_mesh(MeshHandle mesh)
{
  this->reconstructed_mesh_ = mesh;
}

//---------------------------------------------------------------------------
void Shape::clear_reconstructed_mesh()
{
  this->reconstructed_mesh_ = nullptr;
}

//---------------------------------------------------------------------------
bool Shape::import_global_point_files(std::vector<std::string> filenames)
{
  for (int i = 0; i < filenames.size(); i++) {
    vnl_vector<double> points;
    if (!Shape::import_point_file(QString::fromStdString(filenames[i]), points)) {
      return false;
    }
    this->global_point_filenames_.push_back(filenames[i]);
    this->particles_.set_world_particles(i, points);
  }
  this->subject_->set_world_particle_filenames(this->global_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_local_point_files(std::vector<std::string> filenames)
{
  for (int i = 0; i < filenames.size(); i++) {
    vnl_vector<double> points;
    if (!Shape::import_point_file(QString::fromStdString(filenames[i]), points)) {
      return false;
    }
    this->local_point_filenames_.push_back(filenames[i]);
    this->particles_.set_local_particles(i, points);
  }
  this->subject_->set_local_particle_filenames(this->local_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_global_correspondence_points()
{
  return this->particles_.get_combined_global_particles();
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_local_correspondence_points()
{
  return this->particles_.get_combined_local_particles();
}

//---------------------------------------------------------------------------
int Shape::get_id()
{
  return this->id_;
}

//---------------------------------------------------------------------------
void Shape::set_id(int id)
{
  this->id_ = id;
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  auto string = QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
  QFileInfo info(string);
  return info.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename_with_path()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  return QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
}

//---------------------------------------------------------------------------
std::vector<QString> Shape::get_original_filenames()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return std::vector<QString>();
  }

  std::vector<QString> filenames;
  for (auto&& name : this->subject_->get_segmentation_filenames()) {
    filenames.push_back(QFileInfo(QString::fromStdString(name)).fileName());
  }

  return filenames;
}

//---------------------------------------------------------------------------
std::vector<QString> Shape::get_original_filenames_with_path()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return std::vector<QString>();
  }

  std::vector<QString> filenames;
  for (auto&& name : this->subject_->get_segmentation_filenames()) {
    filenames.push_back(QString::fromStdString(name));
  }

  return filenames;
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename()
{
  if (this->subject_->get_groomed_filenames().size() < 1) {
    return "";
  }
  auto string = QString::fromStdString(this->subject_->get_groomed_filenames()[0]);
  QFileInfo info(string);
  return info.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename_with_path(int domain)
{
  if (domain >= this->subject_->get_groomed_filenames().size()) {
    return "";
  }
  return QString::fromStdString(this->subject_->get_groomed_filenames()[domain]);
}

/*
//---------------------------------------------------------------------------
QString Shape::get_global_point_filename()
{
  return QFileInfo(this->global_point_filename_).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_global_point_filename_with_path()
{
  return this->global_point_filename_;
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename()
{
  return QFileInfo(this->local_point_filename_).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename_with_path()
{
  return this->local_point_filename_;
}
*/

//---------------------------------------------------------------------------
QList<Shape::Point> Shape::get_exclusion_sphere_centers()
{
  return this->exclusion_sphere_centers_;
}

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_centers(QList<Shape::Point> centers)
{
  this->exclusion_sphere_centers_ = centers;
}

//---------------------------------------------------------------------------
QList<double> Shape::get_exclusion_sphere_radii()
{
  return this->exclusion_sphere_radii_;
}

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_radii(QList<double> radii)
{
  this->exclusion_sphere_radii_ = radii;
}

//---------------------------------------------------------------------------
int Shape::get_group_id()
{
  return this->group_id_;
}

//---------------------------------------------------------------------------
void Shape::set_group_id(int id)
{
  if (this->subject_) {
    this->subject_->set_group_values({{"group", std::to_string(id)}});
  }
  this->group_id_ = id;
}

//---------------------------------------------------------------------------
std::vector<Shape::Point> Shape::get_vectors()
{
  return this->vectors_;
}

//---------------------------------------------------------------------------
void Shape::set_vectors(std::vector<Shape::Point> vectors)
{
  this->vectors_ = vectors;
}

//---------------------------------------------------------------------------
void Shape::set_transform(const vnl_vector<double>& transform)
{
  this->transform_ = transform;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_transform(int domain)
{
  auto groom_transform = this->get_groomed_transform(domain);
  if (groom_transform.size() != 12) {
    return this->transform_;
  }

  this->transform_.set_size(12);
  for (unsigned int i = 0; i < 12; i++) {
    this->transform_[i] = groom_transform[i];
  }

  return this->transform_;
}

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<string> filenames, MeshHandle& mesh,
                            bool save_transform, bool wait)
{
  if (filenames.empty()) {
    return;
  }

  // single domain supported right now
  std::string filename = filenames[0];

  MeshWorkItem item;
  item.filename = filename;
  MeshHandle new_mesh = this->mesh_manager_->get_mesh(item, wait);
  if (new_mesh) {
    mesh = new_mesh;

    /// Temporarily calculate the COM here
    auto com = vtkSmartPointer<vtkCenterOfMass>::New();
    com->SetInputData(mesh->get_poly_data());
    com->Update();
    double center[3];
    com->GetCenter(center);

    if (save_transform) {
      this->transform_.set_size(12);
      for (unsigned int i = 0; i < 3; i++) {
        this->transform_[9 + i] = center[i];
      }
    }
  }
}

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_group,
                            bool save_transform, bool wait)
{
  if (filenames.empty()) {
    return;
  }

  for (int i = 0; i < filenames.size(); i++) {
    auto filename = filenames[i];
    MeshWorkItem item;
    item.filename = filename;
    MeshHandle new_mesh = this->mesh_manager_->get_mesh(item, wait);
    if (new_mesh) {
      mesh_group.set_mesh(i, new_mesh);

      // generate a basic centering transform
      auto com = vtkSmartPointer<vtkCenterOfMass>::New();
      com->SetInputData(new_mesh->get_poly_data());
      com->Update();
      double center[3];
      com->GetCenter(center);

      if (save_transform && i == 0) { // only store for first domain
        this->transform_.set_size(12);
        for (unsigned int i = 0; i < 3; i++) {
          this->transform_[9 + i] = -center[i];
        }
      }
    }
  }
}

//---------------------------------------------------------------------------
bool Shape::import_point_file(QString filename, vnl_vector<double>& points)
{
  std::ifstream in(filename.toStdString().c_str());
  if (!in.good()) {
    QMessageBox::warning(0, "Unable to open file", "Error opening file: " + filename);
    return false;
  }
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  auto test = filename.toStdString();
  int num_points = 0;
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) { break; }
    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  in.close();
  points.clear();
  points.set_size(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }
  return true;
}

//---------------------------------------------------------------------------
void Shape::load_feature(std::string display_mode, std::string feature)
{
  auto group = this->get_meshes(display_mode);
  if (!group.valid()) {
    // not ready yet
    return;
  }

  /// TODO: multiple domain support
  vtkSmartPointer<vtkPolyData> poly_data = group.meshes()[0]->get_poly_data();

  auto scalar_array = poly_data->GetPointData()->GetArray(feature.c_str());
  if (!scalar_array) {

    if (!this->subject_) {
      return;
    }

    auto filenames = this->subject_->get_feature_filenames();

    vnl_vector<double> transform;
    if (display_mode != Visualizer::MODE_ORIGINAL_C) {
      transform = this->get_groomed_transform();
    }

    if (this->subject_->get_domain_types().size() > 0 &&
        this->subject_->get_domain_types()[0] == DomainType::Image) {

      // read the feature
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(filenames[feature]);
      reader->Update();
      ImageType::Pointer image = reader->GetOutput();

      group.meshes()[0]->apply_feature_map(feature, image, transform);
      this->apply_feature_to_points(feature, image);

    }
    else if (this->subject_->get_domain_types().size() > 0 &&
             this->subject_->get_domain_types()[0] == DomainType::Mesh) {

      auto original_meshes = this->get_original_meshes(true).meshes();

      for (int i = 0; i < original_meshes.size(); i++) {
        auto original_mesh = original_meshes[i];
        original_mesh->apply_scalars(original_mesh, transform);
        this->apply_feature_to_points(feature, original_mesh);
      }

    }

  }
}

//---------------------------------------------------------------------------
void Shape::apply_feature_to_points(std::string feature, ImageType::Pointer image)
{
  using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);

  auto region = image->GetLargestPossibleRegion();

  vnl_vector<double> transform = this->get_groomed_transform();

  vnl_vector<double> all_locals = this->get_local_correspondence_points();

  int num_points = all_locals.size() / 3;

  Eigen::VectorXf values(num_points);

  int idx = 0;
  for (int i = 0; i < num_points; ++i) {

    ImageType::PointType pitk;
    pitk[0] = all_locals[idx++];
    pitk[1] = all_locals[idx++];
    pitk[2] = all_locals[idx++];

    if (transform.size() == 12) {
      pitk[0] = pitk[0] + transform[9];
      pitk[1] = pitk[1] + transform[10];
      pitk[2] = pitk[2] + transform[11];
    }

    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    auto pixel = 0;
    if (region.IsInside(index)) {
      pixel = interpolator->EvaluateAtContinuousIndex(index);
    }

    values[i] = pixel;
  }
  this->point_features_[feature] = values;

}

//---------------------------------------------------------------------------
void Shape::apply_feature_to_points(std::string feature, MeshHandle mesh)
{
  vtkSmartPointer<vtkPolyData> from_mesh = mesh->get_poly_data();

  // Create the tree
  vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kDTree->SetDataSet(from_mesh);
  kDTree->BuildLocator();

  vnl_vector<double> transform = this->get_groomed_transform();

  vnl_vector<double> all_locals = this->get_local_correspondence_points();

  int num_points = all_locals.size() / 3;

  Eigen::VectorXf values(num_points);

  vtkDataArray* from_array = from_mesh->GetPointData()->GetArray(feature.c_str());

  int idx = 0;
  for (int i = 0; i < num_points; ++i) {

    double pt[3];
    pt[0] = all_locals[idx++];
    pt[1] = all_locals[idx++];
    pt[2] = all_locals[idx++];

    if (transform.size() == 12) {
      pt[0] = pt[0] + transform[9];
      pt[1] = pt[1] + transform[10];
      pt[2] = pt[2] + transform[11];
    }

    vtkIdType id = kDTree->FindClosestPoint(pt);
    vtkVariant var = from_array->GetVariantValue(id);

    values[i] = var.ToDouble();
  }
  this->point_features_[feature] = values;

}


//---------------------------------------------------------------------------
Eigen::VectorXf Shape::get_point_features(std::string feature)
{
  auto it = this->point_features_.find(feature);
  if (it == this->point_features_.end()) {
    return Eigen::VectorXf();
  }

  return it->second;
}

//---------------------------------------------------------------------------
TransformType Shape::get_groomed_transform(int domain)
{
  auto transforms = this->subject_->get_groomed_transforms();
  if (domain < transforms.size()) {
    this->groomed_transform_.set_size(transforms[domain].size());
    for (int i = 0; i < transforms[domain].size(); i++) {
      this->groomed_transform_[i] = transforms[domain][i];
    }
  }
  return this->groomed_transform_;
}

//---------------------------------------------------------------------------
void Shape::set_point_features(std::string feature, Eigen::VectorXf values)
{
  this->point_features_[feature] = values;

  auto group = this->get_meshes(Visualizer::MODE_RECONSTRUCTION_C);

  if (group.valid()) {
    for (auto mesh : group.meshes()) {
      mesh->interpolate_scalars_to_mesh(feature,
                                        this->get_global_correspondence_points(), values);
    }
  }
}

//---------------------------------------------------------------------------
void Shape::set_particles(StudioParticles particles)
{
  this->particles_ = particles;
}

//---------------------------------------------------------------------------
StudioParticles Shape::get_particles()
{
  return this->particles_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_reconstruction_transform(int domain)
{
  assert(domain < this->reconstruction_transforms_.size());
  return this->reconstruction_transforms_[domain];
}


//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_global_correspondence_points_for_display()
{
  auto worlds = this->particles_.get_world_particles();
  int size = 0;
  for (int i = 0; i < worlds.size(); i++) {
    size += worlds[i].size();
  }
  vnl_vector<double> points;
  points.set_size(size);

  int idx = 0;
  for (int i = 0; i < worlds.size(); i++) {
    for (int j = 0; j < worlds[i].size(); j+=3) {
      points[idx++] = worlds[i][j+0] + this->reconstruction_transforms_[i][9];
      points[idx++] = worlds[i][j+1] + this->reconstruction_transforms_[i][10];
      points[idx++] = worlds[i][j+2] + this->reconstruction_transforms_[i][11];
    }
  }

  return points;
}

//---------------------------------------------------------------------------
void Shape::set_reconstruction_transforms(std::vector<vnl_vector<double>> transforms)
{
  this->reconstruction_transforms_ = transforms;
}

}