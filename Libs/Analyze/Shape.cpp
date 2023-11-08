#include <Image/Image.h>
#include <Logging.h>
#include <MeshGenerator.h>
#include <MeshGroup.h>
#include <Particles/ParticleSystemEvaluation.h>
#include <Project/ProjectUtils.h>
#include <Shape.h>
#include <Utils/StringUtils.h>
#include <itkImageFileReader.h>
#include <itkOrientImageFilter.h>
#include <vtkCenterOfMass.h>

#include <boost/filesystem.hpp>

// vtk
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>

#include "Libs/Optimize/Domain/VtkMeshWrapper.h"

using ReaderType = itk::ImageFileReader<ImageType>;

namespace shapeworks {

//---------------------------------------------------------------------------
Shape::Shape() {
  id_ = 0;
  corner_annotations_ = {"", "", "", ""};
}

//---------------------------------------------------------------------------
std::string Shape::get_display_name() {
  if (subject_ && subject_->get_display_name() != "") {
    return subject_->get_display_name();
  }

  return "";
}

//---------------------------------------------------------------------------
Shape::~Shape() = default;

//---------------------------------------------------------------------------
MeshGroup Shape::get_meshes(DisplayMode display_mode, bool wait) {
  if (display_mode == DisplayMode::Original) {
    return get_original_meshes(wait);
  } else if (display_mode == DisplayMode::Groomed) {
    return get_groomed_meshes(wait);
  }
  return get_reconstructed_meshes(wait);
}

//---------------------------------------------------------------------------
void Shape::set_annotations(std::vector<std::string> annotations, bool only_overwrite_blank) {
  if (only_overwrite_blank && !corner_annotations_.empty() && corner_annotations_[0] != "") {
    return;  // don't override
  }
  corner_annotations_ = annotations;
}

//---------------------------------------------------------------------------
std::vector<std::string> Shape::get_annotations() { return corner_annotations_; }

//---------------------------------------------------------------------------
void Shape::set_mesh_manager(std::shared_ptr<MeshManager> mesh_manager) { mesh_manager_ = mesh_manager; }

//---------------------------------------------------------------------------
void Shape::set_subject(std::shared_ptr<Subject> subject) {
  subject_ = subject;
  original_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  groomed_meshes_.set_number_of_meshes(subject->get_number_of_domains());
  reconstructed_meshes_.set_number_of_meshes(subject->get_number_of_domains());

  if (!subject_->get_original_filenames().empty()) {
    /// TODO: Show multiple lines of filenames for multiple domains?
    std::string filename = subject_->get_original_filenames()[0];
    corner_annotations_[0] = StringUtils::getFilename(filename);
  }

  if (subject->get_display_name() != "") {
    corner_annotations_[0] = subject->get_display_name();
  }
}

//---------------------------------------------------------------------------
bool Shape::is_subject() { return subject_ != nullptr; }

//---------------------------------------------------------------------------
std::shared_ptr<Subject> Shape::get_subject() { return subject_; }

//---------------------------------------------------------------------------
void Shape::import_original_file(const std::string& filename) {
  subject_->set_original_filenames({filename});
  corner_annotations_[0] = StringUtils::getFilename(filename);
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_original_meshes(bool wait) {
  if (!subject_) {
    std::cerr << "Error: asked for original mesh when none is present!\n";
    assert(0);
    return original_meshes_;
  }

  if (!original_meshes_.valid()) {
    generate_meshes(subject_->get_original_filenames(), original_meshes_, true, wait);
  }
  return original_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_groomed_meshes(bool wait) {
  if (!subject_) {
    std::cerr << "Error: asked for groomed meshes when none are present!\n";
    assert(0);
  }

  if (!groomed_meshes_.valid()) {
    generate_meshes(subject_->get_groomed_filenames(), groomed_meshes_, true, wait);
  }
  return groomed_meshes_;
}

//---------------------------------------------------------------------------
MeshGroup Shape::get_reconstructed_meshes(bool wait) {
  if (!reconstructed_meshes_.valid()) {
    auto locals = particles_.get_local_particles();
    reconstructed_meshes_.set_number_of_meshes(locals.size());
    for (int i = 0; i < locals.size(); i++) {
      MeshHandle mesh = mesh_manager_->get_mesh(locals[i], i, wait);
      if (mesh) {
        reconstructed_meshes_.set_mesh(i, mesh);
      }
    }
  }
  return reconstructed_meshes_;
}

//---------------------------------------------------------------------------
void Shape::reset_groomed_mesh() { groomed_meshes_ = MeshGroup(subject_->get_number_of_domains()); }

//---------------------------------------------------------------------------
void Shape::clear_reconstructed_mesh() { reconstructed_meshes_ = MeshGroup(subject_->get_number_of_domains()); }

//---------------------------------------------------------------------------
bool Shape::import_global_point_files(std::vector<std::string> filenames) {
  for (int i = 0; i < filenames.size(); i++) {
    Eigen::VectorXd points;
    if (filenames[i] != "") {
      if (!Shape::import_point_file(filenames[i], points)) {
        throw std::invalid_argument("Unable to import point file: " + filenames[i]);
      }
    }
    global_point_filenames_.push_back(filenames[i]);
    particles_.set_world_particles(i, points);
  }
  subject_->set_world_particle_filenames(global_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_local_point_files(std::vector<std::string> filenames) {
  for (int i = 0; i < filenames.size(); i++) {
    Eigen::VectorXd points;
    if (filenames[i] != "") {
      if (!Shape::import_point_file(filenames[i], points)) {
        throw std::invalid_argument("Unable to import point file: " + filenames[i]);
      }
    }
    local_point_filenames_.push_back(filenames[i]);
    particles_.set_local_particles(i, points);
  }
  subject_->set_local_particle_filenames(local_point_filenames_);
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_landmarks_files(std::vector<std::string> filenames) {
  std::vector<Eigen::VectorXd> all_points;
  int total_count = 0;
  for (int i = 0; i < filenames.size(); i++) {
    if (filenames[i] == "") {
      continue;
    }
    Eigen::VectorXd points;
    if (!Shape::import_point_file(filenames[i], points)) {
      throw std::invalid_argument("Unable to load landmarks file: " + filenames[i]);
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
  while (filenames.size() < subject_->get_original_filenames().size()) {
    std::string filename = subject_->get_original_filenames()[filenames.size()];
    filename = StringUtils::getBaseFilenameWithoutExtension(filename) + "_landmarks.particles";
    filenames.push_back(filename);
  }
  for (int i = 0; i < filenames.size(); i++) {
    if (filenames[i] == "") {
      std::string filename = subject_->get_original_filenames()[i];
      filenames[i] = StringUtils::getBaseFilenameWithoutExtension(filename) + "_landmarks.particles";
    }
  }

  if (landmarks_.rows() == 0) {
    filenames = {};
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
bool Shape::import_constraints(std::vector<std::string> filenames) {
  for (int i = 0; i < filenames.size(); i++) {
    Constraints constraints;
    try {
      if (!(filenames[i] == "")) {
        constraints.read(filenames[i]);
      }
    } catch (std::exception& e) {
      SW_ERROR(e.what());
      return false;
    }
    constraints_.push_back(constraints);
  }
  return true;
}

//---------------------------------------------------------------------------
bool Shape::store_constraints() {
  auto filenames = subject_->get_constraints_filenames();
  while (filenames.size() < subject_->get_original_filenames().size()) {
    filenames.push_back("");
  }

  if (constraints_.empty()) {
    filenames = {};
  }

  bool has_constraints = false;

  for (int i = 0; i < filenames.size(); i++) {
    if (filenames[i] == "") {
      std::string filename = subject_->get_original_filenames()[i];
      filename = StringUtils::getBaseFilenameWithoutExtension(filename) + "_constraints.json";
      filenames[i] = filename;
    }
    if (get_constraints(i).hasConstraints()) {
      has_constraints = true;
    }
  }

  if (!has_constraints) {
    filenames = {};
  }

  subject_->set_constraints_filenames(filenames);

  for (int i = 0; i < filenames.size(); i++) {
    try {
      get_constraints(i).write(filenames[i]);
    } catch (std::exception& e) {
      SW_ERROR(e.what());
      return false;
    }
  }
  return true;
}

//---------------------------------------------------------------------------
Eigen::VectorXd Shape::get_global_correspondence_points() { return particles_.get_combined_global_particles(); }

//---------------------------------------------------------------------------
Eigen::VectorXd Shape::get_local_correspondence_points() { return particles_.get_combined_local_particles(); }

//---------------------------------------------------------------------------
int Shape::get_id() { return id_; }

//---------------------------------------------------------------------------
void Shape::set_id(int id) { id_ = id; }

//---------------------------------------------------------------------------
void Shape::update_name() {
  if (subject_->get_display_name() != "") {
    corner_annotations_[0] = subject_->get_display_name();
  }
}

//---------------------------------------------------------------------------
std::string Shape::get_original_filename() {
  if (subject_->get_original_filenames().size() < 1) {
    return "";
  }

  return StringUtils::getFilename(subject_->get_original_filenames()[0]);
}

//---------------------------------------------------------------------------
std::string Shape::get_original_filename_with_path() {
  if (subject_->get_original_filenames().size() < 1) {
    return "";
  }
  return subject_->get_original_filenames()[0];
}

//---------------------------------------------------------------------------
std::vector<std::string> Shape::get_original_filenames() {
  if (subject_->get_original_filenames().size() < 1) {
    return std::vector<std::string>();
  }

  std::vector<std::string> filenames;
  for (auto&& name : subject_->get_original_filenames()) {
    filenames.push_back(StringUtils::getFilename(name));
  }

  return filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Shape::get_original_filenames_with_path() {
  if (subject_->get_original_filenames().size() < 1) {
    return std::vector<std::string>();
  }

  std::vector<std::string> filenames;
  for (auto&& name : subject_->get_original_filenames()) {
    filenames.push_back(name);
  }

  return filenames;
}

//---------------------------------------------------------------------------
std::string Shape::get_groomed_filename() {
  if (subject_->get_groomed_filenames().size() < 1) {
    return "";
  }
  return StringUtils::getFilename(subject_->get_groomed_filenames()[0]);
}

//---------------------------------------------------------------------------
std::string Shape::get_groomed_filename_with_path(int domain) {
  if (domain >= subject_->get_groomed_filenames().size()) {
    return "";
  }
  return subject_->get_groomed_filenames()[domain];
}

//---------------------------------------------------------------------------
void Shape::set_transform(vtkSmartPointer<vtkTransform> transform) { transform_ = transform; }

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_transform(int domain) {
  auto groom_transform = get_groomed_transform(domain);
  if (!groom_transform) {
    return transform_;
  }
  return groom_transform;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_inverse_transform(int domain) {
  // transform from common space to destination space
  auto inverse = vtkSmartPointer<vtkTransform>::New();
  inverse->DeepCopy(get_transform(domain));
  inverse->Inverse();
  return inverse;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_alignment(int domain) {
  auto groom_transform = get_groomed_transform(domain);
  if (!groom_transform) {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();
    return transform;
  }
  return groom_transform;
}

//---------------------------------------------------------------------------
bool Shape::has_alignment() {
  auto groom_transform = get_groomed_transform(0);
  if (groom_transform) {
    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_original_transform(int domain) { return transform_; }

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_group, bool save_transform, bool wait) {
  if (filenames.empty()) {
    return;
  }

  for (int i = 0; i < filenames.size(); i++) {
    auto filename = filenames[i];
    MeshWorkItem item;
    item.filename = filename;
    MeshHandle new_mesh = mesh_manager_->get_mesh(item, wait);
    if (new_mesh && new_mesh->get_poly_data()) {
      mesh_group.set_mesh(i, new_mesh);

      if (new_mesh->get_poly_data()->GetNumberOfPoints() < 1) {
        SW_ERROR("Generated mesh is empty, file: " + filenames[i]);
      } else {
        // generate a basic centering transform
        auto com = vtkSmartPointer<vtkCenterOfMass>::New();
        com->SetInputData(new_mesh->get_poly_data());
        com->Update();
        double center[3];
        com->GetCenter(center);

        if (save_transform && i == 0) {  // only store for first domain
          transform_->Identity();
          transform_->Translate(-center[0], -center[1], -center[2]);
        }
      }
    }
  }
}

//---------------------------------------------------------------------------
bool Shape::import_point_file(std::string filename, Eigen::VectorXd& points) {
  return ParticleSystemEvaluation::ReadParticleFile(filename, points);
}

//---------------------------------------------------------------------------
void Shape::load_feature(DisplayMode display_mode, std::string feature) {
  auto group = get_meshes(display_mode);
  if (!group.valid()) {
    // not ready yet
    return;
  }

  int num_domains = group.meshes().size();

  if (!subject_) {
    return;
  }

  for (int d = 0; d < num_domains; d++) {
    vtkSmartPointer<vtkPolyData> poly_data = group.meshes()[d]->get_poly_data();

    // first check if we already have this array
    auto scalar_array = poly_data->GetPointData()->GetArray(feature.c_str());
    if (scalar_array) {
      auto point_features = get_point_features(feature);
      if (point_features.size() == 0) {
        auto original_meshes = get_original_meshes(true);
        if (original_meshes.valid()) {
          load_feature_from_mesh(feature, original_meshes.meshes()[d]);
        }
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
          display_mode == DisplayMode::Reconstructed) {  // already loaded as particle scalars
        set_point_features(feature, point_features);
      } else {
        // next check if there is a feature filename
        auto filenames = subject_->get_feature_filenames();
        if (filenames.find(feature) == filenames.end()) {
          // no feature filename, so load it from the original mesh
          auto original_meshes = get_original_meshes(true).meshes();

          if (original_meshes.size() > d) {
            // assign scalars at points
            load_feature_from_mesh(feature, original_meshes[d]);
          }
        } else {
          // read the feature
          std::string filename = filenames[feature];
          try {
            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName(filename);
            reader->Update();
            ImageType::Pointer image = reader->GetOutput();
            group.meshes()[d]->apply_feature_map(feature, image);
            apply_feature_to_points(feature, image);
          } catch (itk::ExceptionObject& excep) {
            SW_ERROR("Unable to open file: " + filename);
          }
        }
      }
    }
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<Image> Shape::get_image_volume(std::string image_volume_name) {
  if (!subject_) {
    return nullptr;
  }
  auto filenames = subject_->get_feature_filenames();
  if (filenames.find(image_volume_name) != filenames.end()) {
    auto filename = filenames[image_volume_name];

    if (image_volume_filename_ != filename) {
      std::shared_ptr<Image> image = std::make_shared<Image>(filename);
      image_volume_ = image;
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

  Eigen::VectorXd all_locals = get_local_correspondence_points();

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

  Eigen::VectorXd all_locals = get_local_correspondence_points();

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

  set_point_features(feature, values);
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
  if (!subject_) {
    return nullptr;
  }
  auto transforms = subject_->get_groomed_transforms();
  if (domain < 0) {  // global alignment is stored at the end
    domain = transforms.size() - 1;
  }
  if (domain < transforms.size()) {
    return ProjectUtils::convert_transform(transforms[domain]);
  }
  return nullptr;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_procrustes_transform(int domain) {
  auto transforms = subject_->get_procrustes_transforms();
  if (domain < transforms.size()) {
    return ProjectUtils::convert_transform(transforms[domain]);
  }
  return nullptr;
}

//---------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkTransform>> Shape::get_procrustes_transforms() {
  auto lists = subject_->get_procrustes_transforms();
  std::vector<vtkSmartPointer<vtkTransform>> transforms;
  for (size_t i = 0; i < lists.size(); i++) {
    transforms.push_back(ProjectUtils::convert_transform(lists[i]));
  }
  return transforms;
}

//---------------------------------------------------------------------------
void Shape::set_point_features(std::string feature, Eigen::VectorXf values) {
  point_features_[feature] = values;

  auto group = get_meshes(DisplayMode::Reconstructed);

  if (group.valid()) {
    for (auto mesh : group.meshes()) {
      mesh->interpolate_scalars_to_mesh(feature, get_local_correspondence_points(), values);
    }
  }
}

//---------------------------------------------------------------------------
void Shape::set_particles(Particles particles) { particles_ = particles; }

//---------------------------------------------------------------------------
Particles Shape::get_particles() { return particles_; }

//---------------------------------------------------------------------------
void Shape::set_particle_transform(vtkSmartPointer<vtkTransform> transform) {
  particles_.set_procrustes_transforms(get_procrustes_transforms());
  particles_.set_transform(transform);
}

//---------------------------------------------------------------------------
void Shape::set_alignment_type(int alignment) { particles_.set_alignment_type(alignment); }

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> Shape::get_reconstruction_transform(int domain) {
  if (domain < reconstruction_transforms_.size()) {
    return reconstruction_transforms_[domain];
  }

  // no transforms, just return identity
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  return transform;
}

//---------------------------------------------------------------------------
std::vector<Eigen::VectorXd> Shape::get_particles_for_display() {
  std::vector<Eigen::VectorXd> particles;

  auto locals = particles_.get_local_particles();

  // int idx = 0;
  for (int i = 0; i < locals.size(); i++) {
    Eigen::VectorXd points = locals[i];
    for (int j = 0; j < locals[i].size(); j += 3) {
      int idx = j;
      double p[3];
      p[0] = locals[i][j + 0];
      p[1] = locals[i][j + 1];
      p[2] = locals[i][j + 2];
      if (reconstruction_transforms_.size() > i && !subject_) {  // only computed shapes
        double* pt = reconstruction_transforms_[i]->TransformPoint(p);
        points[idx++] = pt[0];
        points[idx++] = pt[1];
        points[idx++] = pt[2];
      } else {
        points[idx++] = p[0];
        points[idx++] = p[1];
        points[idx++] = p[2];
      }
    }
    particles.push_back(points);
  }

  return particles;
}

//---------------------------------------------------------------------------
void Shape::set_reconstruction_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms) {
  reconstruction_transforms_ = transforms;
}

//---------------------------------------------------------------------------
void Shape::load_feature_from_scalar_file(std::string filename, std::string feature_name) {
  if (!boost::filesystem::exists(filename)) {
    return;
  }

  std::ifstream in(filename);
  if (!in.good()) {
    SW_ERROR("Unable to open scalar file: " + filename);
    return;
  }

  std::vector<float> floats;
  while (in.good()) {
    float line;
    in >> line;
    floats.push_back(line);
  }

  Eigen::VectorXf values(floats.size());
  for (int i = 0; i < floats.size(); i++) {
    values[i] = floats[i];
  }

  set_point_features(feature_name, values);
}

//---------------------------------------------------------------------------
void Shape::set_override_feature(std::string feature) { override_feature_ = feature; }

//---------------------------------------------------------------------------
std::string Shape::get_override_feature() { return override_feature_; }

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
bool Shape::has_planes() {
  for (int i = 0; i < constraints_.size(); i++) {
    for (auto& plane : constraints_[i].getPlaneConstraints()) {
      if (plane.points().size() == 3) {
        return true;
      }
    }
  }
  return false;
}

//---------------------------------------------------------------------------
std::vector<std::shared_ptr<VtkMeshWrapper>> Shape::get_groomed_mesh_wrappers() {
  if (!groomed_mesh_wrappers_.empty()) {
    return groomed_mesh_wrappers_;
  }

  auto group = get_groomed_meshes(true /* wait */);
  for (auto& mesh : group.meshes()) {
    auto wrapper = std::make_shared<VtkMeshWrapper>(mesh->get_poly_data());
    groomed_mesh_wrappers_.push_back(wrapper);
  }
  return groomed_mesh_wrappers_;
}

}  // namespace shapeworks
