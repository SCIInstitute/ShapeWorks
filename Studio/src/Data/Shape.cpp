#include <Data/MeshGenerator.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Libs/Image/Image.h>
#include <Libs/Project/ProjectUtils.h>
#include <Libs/Utils/StringUtils.h>
#include <Visualization/Visualizer.h>
#include <itkOrientImageFilter.h>
#include <vtkCenterOfMass.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

using ReaderType = itk::ImageFileReader<ImageType>;

namespace shapeworks {

//---------------------------------------------------------------------------
Shape::Shape() {
  this->id_ = 0;

  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
}

//---------------------------------------------------------------------------
QString Shape::get_display_name() {
  if (subject_ && subject_->get_display_name() != "") {
    return QString::fromStdString(subject_->get_display_name());
  }

  return "";
}

//---------------------------------------------------------------------------
Shape::~Shape() = default;

//---------------------------------------------------------------------------
MeshGroup Shape::get_meshes(const string& display_mode) {
  if (display_mode == Visualizer::MODE_ORIGINAL_C) {
    return this->get_original_meshes();
  } else if (display_mode == Visualizer::MODE_GROOMED_C) {
    return this->get_groomed_meshes();
  }
  return this->get_reconstructed_meshes();
}

//---------------------------------------------------------------------------
void Shape::set_annotations(QStringList annotations, bool only_overwrite_blank) {
  if (only_overwrite_blank && !this->corner_annotations_.empty() && this->corner_annotations_[0] != "") {
    return;  // don't override
  }
  this->corner_annotations_ = annotations;
}

//---------------------------------------------------------------------------
QStringList Shape::get_annotations() { return this->corner_annotations_; }

//---------------------------------------------------------------------------
void Shape::set_mesh_manager(QSharedPointer<MeshManager> mesh_manager) { this->mesh_manager_ = mesh_manager; }

//---------------------------------------------------------------------------
void Shape::set_subject(std::shared_ptr<Subject> subject) {
  this->subject_ = subject;
  this->original_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  this->groomed_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  this->reconstructed_meshes_.set_number_of_meshes(subject->get_number_of_domains());

  if (!this->subject_->get_segmentation_filenames().empty()) {
    /// TODO: Show multiple lines of filenames for multiple domains?
    std::string filename = this->subject_->get_segmentation_filenames()[0];
    this->corner_annotations_[0] = QFileInfo(QString::fromStdString(filename)).fileName();
  }

  if (subject->get_display_name() != "") {
    this->corner_annotations_[0] = QString::fromStdString(subject->get_display_name());
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<Subject> Shape::get_subject() { return this->subject_; }

//---------------------------------------------------------------------------
void Shape::import_original_image(const std::string& filename) {
  this->subject_->set_segmentation_filenames(std::vector<std::string>{filename});
  this->corner_annotations_[0] = QFileInfo(QString::fromStdString(filename)).fileName();
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_original_meshes(bool wait) {
  if (!this->subject_) {
    std::cerr << "Error: asked for original mesh when none is present!\n";
    assert(0);
  }

  if (!this->original_meshes_.valid()) {
    this->generate_meshes(this->subject_->get_segmentation_filenames(), this->original_meshes_, true, wait);
  }
  return this->original_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_groomed_meshes(bool wait) {
  if (!this->subject_) {
    std::cerr << "Error: asked for groomed meshes when none are present!\n";
    assert(0);
  }

  if (!this->groomed_meshes_.valid()) {
    this->generate_meshes(this->subject_->get_groomed_filenames(), this->groomed_meshes_, true, wait);
  }
  return this->groomed_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_reconstructed_meshes(bool wait) {
  if (!this->reconstructed_meshes_.valid()) {
    auto worlds = this->particles_.get_world_particles();
    this->reconstructed_meshes_.set_number_of_meshes(worlds.size());
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
void Shape::reset_groomed_mesh() { this->groomed_meshes_ = MeshGroup(this->subject_->get_number_of_domains()); }

//---------------------------------------------------------------------------
void Shape::clear_reconstructed_mesh() {
  this->reconstructed_meshes_ = MeshGroup(this->subject_->get_number_of_domains());
}

//---------------------------------------------------------------------------
bool Shape::import_global_point_files(QStringList filenames) {
  for (int i = 0; i < filenames.size(); i++) {
    Eigen::VectorXd points;
    if (!Shape::import_point_file(filenames[i], points)) {
      throw std::invalid_argument("Unable to import point file: " + filenames[i].toStdString());
    }
    this->global_point_filenames_.push_back(filenames[i].toStdString());
    this->particles_.set_world_particles(i, points);
  }
  this->subject_->set_world_particle_filenames(this->global_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_local_point_files(QStringList filenames) {
  for (int i = 0; i < filenames.size(); i++) {
    Eigen::VectorXd points;
    if (!Shape::import_point_file(filenames[i], points)) {
      throw std::invalid_argument("Unable to import point file: " + filenames[i].toStdString());
    }
    this->local_point_filenames_.push_back(filenames[i].toStdString());
    this->particles_.set_local_particles(i, points);
  }
  this->subject_->set_local_particle_filenames(this->local_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_landmarks_files(QStringList filenames) {
  std::vector<Eigen::VectorXd> all_points;
  int total_count = 0;
  for (int i = 0; i < filenames.size(); i++) {
    Eigen::VectorXd points;
    if (!Shape::import_point_file(filenames[i], points)) {
      throw std::invalid_argument("Unable to load file: " + filenames[i].toStdString());
    }
    total_count += points.size() / 3;

    all_points.push_back(points);
  }

  landmarks_.resize(total_count, 5);
  int row = 0;
  for (int i = 0; i < all_points.size(); i++) {
    Eigen::VectorXd points = all_points[i];

    int point_id = 0;
    for (int j = 0; j < static_cast<int>(points.size()) - 2; j += 3) {
      landmarks_(row, 0) = i;  // domain
      landmarks_(row, 1) = point_id++;
      landmarks_(row, 2) = points[j];
      landmarks_(row, 3) = points[j + 1];
      landmarks_(row, 4) = points[j + 2];
      row++;
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool Shape::store_landmarks() {
  auto filenames = subject_->get_landmarks_filenames();
  while (filenames.size() < subject_->get_segmentation_filenames().size()) {
    std::string filename = subject_->get_segmentation_filenames()[filenames.size()];
    filename = StringUtils::getFileNameWithoutExtension(filename) + "_landmarks.particles";
    filenames.push_back(filename);
  }

  subject_->set_landmarks_filenames(filenames);

  for (int i = 0; i < filenames.size(); i++) {
    std::ofstream out(filenames[i]);
    if (out.bad()) {
      return false;
    }
    for (int row = 0; row < landmarks_.rows(); row++) {
      if (landmarks_(row, 0) == i) {
        out << landmarks_(row, 2) << " ";
        out << landmarks_(row, 3) << " ";
        out << landmarks_(row, 4) << "\n";
      }
    }
    out.close();
  }

  return true;
}

//---------------------------------------------------------------------------
Eigen::VectorXd Shape::get_global_correspondence_points() { return this->particles_.get_combined_global_particles(); }

//---------------------------------------------------------------------------
Eigen::VectorXd Shape::get_local_correspondence_points() { return this->particles_.get_combined_local_particles(); }

//---------------------------------------------------------------------------
int Shape::get_id() { return this->id_; }

//---------------------------------------------------------------------------
void Shape::set_id(int id) { this->id_ = id; }

//---------------------------------------------------------------------------
QString Shape::get_original_filename() {
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  auto string = QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
  QFileInfo info(string);
  return info.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename_with_path() {
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  return QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
}

//---------------------------------------------------------------------------
std::vector<QString> Shape::get_original_filenames() {
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
std::vector<QString> Shape::get_original_filenames_with_path() {
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
QString Shape::get_groomed_filename() {
  if (this->subject_->get_groomed_filenames().size() < 1) {
    return "";
  }
  auto string = QString::fromStdString(this->subject_->get_groomed_filenames()[0]);
  QFileInfo info(string);
  return info.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename_with_path(int domain) {
  if (domain >= this->subject_->get_groomed_filenames().size()) {
    return "";
  }
  return QString::fromStdString(this->subject_->get_groomed_filenames()[domain]);
}

//---------------------------------------------------------------------------
QList<Shape::Point> Shape::get_exclusion_sphere_centers() { return this->exclusion_sphere_centers_; }

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_centers(QList<Shape::Point> centers) { this->exclusion_sphere_centers_ = centers; }

//---------------------------------------------------------------------------
QList<double> Shape::get_exclusion_sphere_radii() { return this->exclusion_sphere_radii_; }

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_radii(QList<double> radii) { this->exclusion_sphere_radii_ = radii; }

//---------------------------------------------------------------------------
int Shape::get_group_id() { return this->group_id_; }

//---------------------------------------------------------------------------
void Shape::set_group_id(int id) {
  if (this->subject_) {
    this->subject_->set_group_values({{"group", std::to_string(id)}});
  }
  this->group_id_ = id;
}

//---------------------------------------------------------------------------
std::vector<Shape::Point> Shape::get_vectors() { return this->vectors_; }

//---------------------------------------------------------------------------
void Shape::set_vectors(std::vector<Shape::Point> vectors) { this->vectors_ = vectors; }

//---------------------------------------------------------------------------
void Shape::set_transform(vtkSmartPointer<vtkTransform> transform) { this->transform_ = transform; }

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_transform(int domain) {
  auto groom_transform = this->get_groomed_transform(domain);
  if (!groom_transform) {
    return this->transform_;
  }
  return groom_transform;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_alignment(int domain) {
  auto groom_transform = this->get_groomed_transform(domain);
  if (!groom_transform) {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();
    return transform;
  }
  return groom_transform;
}

//---------------------------------------------------------------------------
bool Shape::has_alignment() {
  auto groom_transform = this->get_groomed_transform(0);
  if (groom_transform) {
    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_original_transform(int domain) { return this->transform_; }

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_group, bool save_transform, bool wait) {
  if (filenames.empty()) {
    return;
  }

  for (int i = 0; i < filenames.size(); i++) {
    auto filename = filenames[i];
    MeshWorkItem item;
    item.filename = filename;
    MeshHandle new_mesh = this->mesh_manager_->get_mesh(item, wait);
    if (new_mesh && new_mesh->get_poly_data()) {
      mesh_group.set_mesh(i, new_mesh);

      if (new_mesh->get_poly_data()->GetNumberOfPoints() < 1) {
        STUDIO_SHOW_ERROR("Error: Mesh contained no points: " + QString::fromStdString(filenames[i]));
      } else {
        // generate a basic centering transform
        auto com = vtkSmartPointer<vtkCenterOfMass>::New();
        com->SetInputData(new_mesh->get_poly_data());
        com->Update();
        double center[3];
        com->GetCenter(center);

        if (save_transform && i == 0) {  // only store for first domain
          this->transform_->Identity();
          this->transform_->Translate(-center[0], -center[1], -center[2]);
        }
      }
    }
  }
}

//---------------------------------------------------------------------------
bool Shape::import_point_file(QString filename, Eigen::VectorXd& points) {
  std::ifstream in(filename.toStdString().c_str());
  if (!in.good()) {
    return false;
  }
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  int num_points = 0;
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) {
      break;
    }
    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  in.close();
  points.setZero();
  points.resize(num_points * 3);

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
void Shape::load_feature(std::string display_mode, std::string feature) {
  auto group = get_meshes(display_mode);
  if (!group.valid()) {
    // not ready yet
    return;
  }

  int num_domains = group.meshes().size();

  for (int d = 0; d < num_domains; d++) {
    vtkSmartPointer<vtkPolyData> poly_data = group.meshes()[d]->get_poly_data();

    // first check if we already have this array
    auto scalar_array = poly_data->GetPointData()->GetArray(feature.c_str());
    if (scalar_array) {
      auto point_features = get_point_features(feature);
      if (point_features.size() == 0) {
        auto original_meshes = this->get_original_meshes(true).meshes();
        this->load_feature_from_mesh(feature, original_meshes[d]);
      }
      return;
    }
    if (!scalar_array) {
      if (!subject_) {
        return;
      }

      // first check if we have particle scalars for this feature
      auto point_features = get_point_features(feature);
      if (point_features.size() > 0 &&
          display_mode == Visualizer::MODE_RECONSTRUCTION_C) {  // already loaded as particle scalars
        set_point_features(feature, point_features);
      } else {
        // next check if there is a feature filename
        auto filenames = this->subject_->get_feature_filenames();
        if (filenames.find(feature) == filenames.end()) {
          // no feature filename, so load it from the original mesh
          auto original_meshes = this->get_original_meshes(true).meshes();

          if (original_meshes.size() > d) {
            // assign scalars at points
            this->load_feature_from_mesh(feature, original_meshes[d]);
          }
        } else {
          // read the feature
          QString filename = QString::fromStdString(filenames[feature]);
          try {
            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName(filename.toStdString());
            reader->Update();
            ImageType::Pointer image = reader->GetOutput();
            group.meshes()[d]->apply_feature_map(feature, image);
            apply_feature_to_points(feature, image);
          } catch (itk::ExceptionObject& excep) {
            QMessageBox::warning(0, "Unable to open file", "Error opening file: \"" + filename + "\"");
          }
        }
      }
    }
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> Shape::get_image_volume(std::string image_volume_name) {
  if (!subject_) {
    return nullptr;
  }
  auto filenames = this->subject_->get_feature_filenames();
  if (filenames.find(image_volume_name) != filenames.end()) {
    auto filename = filenames[image_volume_name];

    if (image_volume_filename_ != filename) {
      Image image(filename);
      image_volume_ = vtkSmartPointer<vtkImageData>::New();
      image_volume_->DeepCopy(image.getVTKImage());
      image_volume_filename_ = filename;
    }

    return image_volume_;
  }
  return nullptr;
}

//---------------------------------------------------------------------------
void Shape::apply_feature_to_points(std::string feature, ImageType::Pointer image) {
  using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);

  auto region = image->GetLargestPossibleRegion();

  Eigen::VectorXd all_locals = this->get_local_correspondence_points();

  int num_points = all_locals.size() / 3;

  Eigen::VectorXf values(num_points);

  int idx = 0;
  for (int i = 0; i < num_points; ++i) {
    double p[3];
    p[0] = all_locals[idx++];
    p[1] = all_locals[idx++];
    p[2] = all_locals[idx++];

    double* pt = p;

    ImageType::PointType pitk;
    pitk[0] = pt[0];
    pitk[1] = pt[1];
    pitk[2] = pt[2];

    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    auto pixel = 0;
    if (region.IsInside(index)) {
      pixel = interpolator->EvaluateAtContinuousIndex(index);
    }

    values[i] = pixel;
  }

  set_point_features(feature, values);
}

//---------------------------------------------------------------------------
void Shape::load_feature_from_mesh(std::string feature, MeshHandle mesh) {
  vtkSmartPointer<vtkPolyData> from_mesh = mesh->get_poly_data();

  // Create the tree
  vtkSmartPointer<vtkKdTreePointLocator> kd_tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kd_tree->SetDataSet(from_mesh);
  kd_tree->BuildLocator();

  Eigen::VectorXd all_locals = this->get_local_correspondence_points();

  int num_points = all_locals.size() / 3;

  Eigen::VectorXf values(num_points);

  vtkDataArray* from_array = from_mesh->GetPointData()->GetArray(feature.c_str());
  if (!from_array) {
    return;
  }

  int idx = 0;
  for (int i = 0; i < num_points; ++i) {
    double p[3];
    p[0] = all_locals[idx++];
    p[1] = all_locals[idx++];
    p[2] = all_locals[idx++];

    vtkIdType id = kd_tree->FindClosestPoint(p);
    vtkVariant var = from_array->GetVariantValue(id);

    values[i] = var.ToDouble();
  }

  this->set_point_features(feature, values);
}

//---------------------------------------------------------------------------
Eigen::VectorXf Shape::get_point_features(std::string feature) {
  auto it = point_features_.find(feature);
  if (it == point_features_.end()) {
    return Eigen::VectorXf();
  }

  return it->second;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_groomed_transform(int domain) {
  auto transforms = this->subject_->get_groomed_transforms();
  if (domain < 0) {  // global alignment is stored at the end
    domain = transforms.size() - 1;
  }
  if (domain < transforms.size()) {
    return ProjectUtils::convert_transform(transforms[domain]);
  }
  return nullptr;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_procrustest_transform(int domain) {
  auto transforms = this->subject_->get_procrustes_transforms();
  if (domain < transforms.size()) {
    return ProjectUtils::convert_transform(transforms[domain]);
  }
  return nullptr;
}

//---------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkTransform>> Shape::get_procrustest_transforms() {
  auto lists = this->subject_->get_procrustes_transforms();
  std::vector<vtkSmartPointer<vtkTransform>> transforms;
  for (size_t i = 0; i < lists.size(); i++) {
    transforms.push_back(ProjectUtils::convert_transform(lists[i]));
  }
  return transforms;
}

//---------------------------------------------------------------------------
void Shape::set_point_features(std::string feature, Eigen::VectorXf values) {
  this->point_features_[feature] = values;

  auto group = this->get_meshes(Visualizer::MODE_RECONSTRUCTION_C);

  if (group.valid()) {
    for (auto mesh : group.meshes()) {
      mesh->interpolate_scalars_to_mesh(feature, this->get_global_correspondence_points(), values);
    }
  }
}

//---------------------------------------------------------------------------
void Shape::set_particles(StudioParticles particles) { this->particles_ = particles; }

//---------------------------------------------------------------------------
StudioParticles Shape::get_particles() { return this->particles_; }

//---------------------------------------------------------------------------
void Shape::set_particle_transform(vtkSmartPointer<vtkTransform> transform) {
  this->particles_.set_procrustes_transforms(this->get_procrustest_transforms());
  this->particles_.set_transform(transform);
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_reconstruction_transform(int domain) {
  if (domain < this->reconstruction_transforms_.size()) {
    return this->reconstruction_transforms_[domain];
  }

  // no transforms, just return identity
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  return transform;
}

//---------------------------------------------------------------------------
Eigen::VectorXd Shape::get_global_correspondence_points_for_display() {
  auto worlds = this->particles_.get_world_particles();
  int size = 0;
  for (int i = 0; i < worlds.size(); i++) {
    size += worlds[i].size();
  }
  Eigen::VectorXd points;
  points.resize(size);

  int idx = 0;
  for (int i = 0; i < worlds.size(); i++) {
    for (int j = 0; j < worlds[i].size(); j += 3) {
      double p[3];
      p[0] = worlds[i][j + 0];
      p[1] = worlds[i][j + 1];
      p[2] = worlds[i][j + 2];
      if (this->reconstruction_transforms_.size() > i) {
        double* pt = this->reconstruction_transforms_[i]->TransformPoint(p);
        points[idx++] = pt[0];
        points[idx++] = pt[1];
        points[idx++] = pt[2];
      } else {
        points[idx++] = p[0];
        points[idx++] = p[1];
        points[idx++] = p[2];
      }
    }
  }

  return points;
}

//---------------------------------------------------------------------------
void Shape::set_reconstruction_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms) {
  this->reconstruction_transforms_ = transforms;
}

//---------------------------------------------------------------------------
void Shape::load_feature_from_scalar_file(std::string filename, std::string feature_name) {
  QString qfilename = QString::fromStdString(filename);

  if (!QFile(qfilename).exists()) {
    return;
  }

  QFile file(qfilename);
  if (!file.open(QIODevice::ReadOnly)) {
    STUDIO_LOG_ERROR("Unable to open scalar file: " + qfilename);
    return;
  }

  auto data = QString(file.readAll()).trimmed();
  auto lines = data.split('\n');
  file.close();

  Eigen::VectorXf values(lines.size());
  for (int i = 0; i < lines.size(); i++) {
    float value = QString(lines[i]).toFloat();
    values[i] = value;
  }

  this->set_point_features(feature_name, values);
}

//---------------------------------------------------------------------------
void Shape::set_override_feature(string feature) { this->override_feature_ = feature; }

//---------------------------------------------------------------------------
std::string Shape::get_override_feature() { return this->override_feature_; }

//---------------------------------------------------------------------------
Eigen::MatrixXd& Shape::landmarks() { return landmarks_; }

//---------------------------------------------------------------------------
std::vector<Constraints>& Shape::constraints() { return constraints_; }

//---------------------------------------------------------------------------
Constraints& Shape::get_constraints(int domain_id) {
  while (domain_id >= constraints_.size()) {
    constraints_.push_back(Constraints{});
  }
  return constraints_[domain_id];
}

//---------------------------------------------------------------------------
bool Shape::has_planes()
{
  return true;
  for (int i=0;i<constraints_.size();i++) {
    if (!constraints_[i].getPlaneConstraints().empty()) {
      return true;
    }
  }
  return false;
}
}  // namespace shapeworks
