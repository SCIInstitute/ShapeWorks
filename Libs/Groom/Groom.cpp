#include <Groom.h>
#include <GroomParameters.h>
#include <Libs/Image/Image.h>
#include <Libs/Mesh/Mesh.h>
#include <Libs/Mesh/MeshUtils.h>
#include <Libs/Project/ProjectUtils.h>
#include <Libs/Utils/StringUtils.h>
#include <itkRegionOfInterestImageFilter.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>
#include <vtkCenterOfMass.h>
#include <vtkLandmarkTransform.h>
#include <vtkPointSet.h>

#include <boost/filesystem.hpp>
#include <vector>

using namespace shapeworks;

typedef float PixelType;
typedef itk::Image<PixelType, 3> ImageType;

//---------------------------------------------------------------------------
Groom::Groom(ProjectHandle project) { this->project_ = project; }

//---------------------------------------------------------------------------
bool Groom::run() {
  this->progress_ = 0;
  this->progress_counter_ = 0;
  this->total_ops_ = this->get_total_ops();

  auto subjects = this->project_->get_subjects();

  if (subjects.empty()) {
    throw std::invalid_argument("No subjects to groom");
  }
  tbb::atomic<bool> success = true;

  tbb::parallel_for(tbb::blocked_range<size_t>{0, subjects.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      for (int domain = 0; domain < subjects[i]->get_number_of_domains(); domain++) {
        if (this->abort_) {
          success = false;
          continue;
        }

        bool is_image = subjects[0]->get_domain_types()[domain] == DomainType::Image;
        bool is_mesh = subjects[0]->get_domain_types()[domain] == DomainType::Mesh;

        if (is_image) {
          if (!this->image_pipeline(subjects[i], domain)) {
            success = false;
          }
        }

        if (is_mesh) {
          if (!this->mesh_pipeline(subjects[i], domain)) {
            success = false;
          }
        }
      }
    }
  });

  if (!this->run_alignment()) {
    success = false;
  }

  // store back to project
  this->project_->store_subjects();
  return success;
}

//---------------------------------------------------------------------------
bool Groom::image_pipeline(std::shared_ptr<Subject> subject, size_t domain) {
  // grab parameters
  auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

  auto path = subject->get_segmentation_filenames()[domain];

  // load the image
  Image image(path);

  // define a groom transform
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();

  if (this->skip_grooming_) {
    std::vector<std::vector<double>> groomed_transforms;
    groomed_transforms.push_back(ProjectUtils::convert_transform(transform));
    subject->set_groomed_transforms(groomed_transforms);

    {
      // lock for project data structure
      tbb::mutex::scoped_lock lock(mutex_);

      // update groomed filenames
      std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
      if (domain >= groomed_filenames.size()) {
        groomed_filenames.resize(domain + 1);
      }
      groomed_filenames[domain] = path;

      // store filenames back to subject
      subject->set_groomed_filenames(groomed_filenames);
    }

    return true;
  }

  this->run_image_pipeline(image, params);

  // reflection
  if (params.get_reflect()) {
    auto table = subject->get_table_values();
    if (table.find(params.get_reflect_column()) != table.end()) {
      if (table[params.get_reflect_column()] == params.get_reflect_choice()) {
        this->add_reflect_transform(transform, params.get_reflect_axis());
      }
    }
  }

  // centering
  if (params.get_use_center()) {
    this->add_center_transform(transform, image);
  }

  if (this->abort_) {
    return false;
  }

  // groomed filename
  std::string groomed_name = this->get_output_filename(path, DomainType::Image);

  if (params.get_convert_to_mesh()) {
    Mesh mesh = image.toMesh(0.0);
    this->run_mesh_pipeline(mesh, params);
    groomed_name = this->get_output_filename(path, DomainType::Mesh);
    // save the groomed mesh
    MeshUtils::threadSafeWriteMesh(groomed_name, mesh);
  } else {
    // save image
    image.write(groomed_name);
  }

  {
    // lock for project data structure
    tbb::mutex::scoped_lock lock(mutex_);

    subject->set_groomed_transform(domain, ProjectUtils::convert_transform(transform));

    // update groomed filenames
    std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
    if (domain >= groomed_filenames.size()) {
      groomed_filenames.resize(domain + 1);
    }
    groomed_filenames[domain] = groomed_name;

    // store filenames back to subject
    subject->set_groomed_filenames(groomed_filenames);
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::run_image_pipeline(Image& image, GroomParameters params) {
  // isolate
  if (params.get_isolate_tool()) {
    image.isolate();
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // fill holes
  if (params.get_fill_holes_tool()) {
    image.closeHoles();
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // crop
  if (params.get_crop()) {
    PhysicalRegion region = image.physicalBoundingBox(0.5);
    image.crop(region);
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // autopad
  if (params.get_auto_pad_tool()) {
    image.pad(params.get_padding_amount());
    this->fix_origin(image);
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // antialias
  if (params.get_antialias_tool()) {
    image.antialias(params.get_antialias_iterations());
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // resample
  if (params.get_resample()) {
    auto spacing = params.get_spacing();
    if (params.get_isotropic()) {
      auto iso = params.get_iso_spacing();
      spacing = {iso, iso, iso};
    }
    Vector v;
    v[0] = spacing[0];
    v[1] = spacing[1];
    v[2] = spacing[2];
    if (v[0] == 0 || v[1] == 0 || v[2] == 0) {
      // skip resample
    } else {
      image.resample(v, Image::InterpolationType::Linear);
    }
    this->increment_progress();
  }
  if (this->abort_) {
    return false;
  }

  // create distance transform
  if (params.get_fast_marching()) {
    image.computeDT();
    this->increment_progress(10);
  }

  if (this->abort_) {
    return false;
  }

  // blur
  if (params.get_blur_tool()) {
    image.gaussianBlur(params.get_blur_amount());
    this->increment_progress();
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain) {
  // grab parameters
  auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

  auto path = subject->get_segmentation_filenames()[domain];

  // groomed mesh name
  std::string groom_name = this->get_output_filename(path, DomainType::Mesh);

  Mesh mesh = MeshUtils::threadSafeReadMesh(path);

  // define a groom transform
  auto transform = vtkSmartPointer<vtkTransform>::New();

  if (!this->skip_grooming_) {
    this->run_mesh_pipeline(mesh, params);

    // reflection
    if (params.get_reflect()) {
      auto table = subject->get_table_values();
      if (table.find(params.get_reflect_column()) != table.end()) {
        if (table[params.get_reflect_column()] == params.get_reflect_choice()) {
          this->add_reflect_transform(transform, params.get_reflect_axis());
        }
      }
    }

    // centering
    if (params.get_use_center()) {
      this->add_center_transform(transform, mesh);
    }
  }

  // save the groomed mesh
  MeshUtils::threadSafeWriteMesh(groom_name, mesh);

  {
    // lock for project data structure
    tbb::mutex::scoped_lock lock(mutex_);

    // store transform
    subject->set_groomed_transform(domain, ProjectUtils::convert_transform(transform));

    // update groomed filenames
    std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
    if (domain >= groomed_filenames.size()) {
      groomed_filenames.resize(domain + 1);
    }
    groomed_filenames[domain] = groom_name;

    // store filename back to subject
    subject->set_groomed_filenames(groomed_filenames);
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::run_mesh_pipeline(Mesh& mesh, GroomParameters params) {
  if (params.get_fill_mesh_holes_tool()) {
    mesh.fillHoles();
    this->increment_progress();
  }

  if (params.get_remesh()) {
    int total_vertices = mesh.getVTKMesh()->GetNumberOfPoints();
    int num_vertices = params.get_remesh_num_vertices();
    if (params.get_remesh_percent_mode()) {
      num_vertices = total_vertices * params.get_remesh_percent() / 100.0;
    }
    mesh.remesh(num_vertices, params.get_remesh_gradation());
    this->increment_progress();
  }

  if (params.get_mesh_smooth()) {
    if (params.get_mesh_smoothing_method() == GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C) {
      mesh.smooth(params.get_mesh_vtk_laplacian_iterations(), params.get_mesh_vtk_laplacian_relaxation());
    } else if (params.get_mesh_smoothing_method() == GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C) {
      mesh.smoothSinc(params.get_mesh_vtk_windowed_sinc_iterations(), params.get_mesh_vtk_windowed_sinc_passband());
    }
    this->increment_progress();
  }
  return true;
}

//---------------------------------------------------------------------------
void Groom::fix_origin(Image& image) {
  ImageType::Pointer img = image.getITKImage();
  using RegionFilterType = itk::RegionOfInterestImageFilter<ImageType, ImageType>;
  RegionFilterType::Pointer region_filter = RegionFilterType::New();
  region_filter->SetInput(img);
  region_filter->SetRegionOfInterest(img->GetLargestPossibleRegion());
  region_filter->UpdateLargestPossibleRegion();
  image = Image(region_filter->GetOutput());
}

//---------------------------------------------------------------------------
int Groom::get_total_ops() {
  int num_subjects = this->project_->get_subjects().size();

  int num_tools = 0;

  auto domains = this->project_->get_domain_names();
  auto subjects = this->project_->get_subjects();

  for (int i = 0; i < domains.size(); i++) {
    auto params = GroomParameters(this->project_, domains[i]);

    if (subjects[i]->get_domain_types()[i] == DomainType::Image) {
      num_tools += params.get_isolate_tool() ? 1 : 0;
      num_tools += params.get_fill_holes_tool() ? 1 : 0;
      num_tools += params.get_crop() ? 1 : 0;
      num_tools += params.get_auto_pad_tool() ? 1 : 0;
      num_tools += params.get_antialias_tool() ? 1 : 0;
      num_tools += params.get_resample() ? 1 : 0;
      num_tools += params.get_fast_marching() ? 10 : 0;
      num_tools += params.get_blur_tool() ? 1 : 0;
    }

    bool run_mesh = subjects[i]->get_domain_types()[i] == DomainType::Mesh ||
                    (subjects[i]->get_domain_types()[i] == DomainType::Image && params.get_convert_to_mesh());

    if (run_mesh) {
      num_tools += params.get_fill_holes_tool() ? 1 : 0;
      num_tools += params.get_mesh_smooth() ? 1 : 0;
      num_tools += params.get_remesh() ? 1 : 0;
    }
  }

  return num_subjects * num_tools;
}

//---------------------------------------------------------------------------
void Groom::increment_progress(int amount) {
  this->progress_counter_ += amount;
  this->progress_ = static_cast<float>(this->progress_counter_) / static_cast<float>(this->total_ops_) * 100.0;
  this->update_progress();
}

//---------------------------------------------------------------------------
void Groom::set_skip_grooming(bool skip) { this->skip_grooming_ = skip; }

//---------------------------------------------------------------------------
void Groom::abort() { this->abort_ = true; }

//---------------------------------------------------------------------------
bool Groom::get_aborted() { return this->abort_; }

//---------------------------------------------------------------------------
bool Groom::run_alignment() {
  size_t num_domains = this->project_->get_number_of_domains_per_subject();
  auto subjects = this->project_->get_subjects();

  auto base_params = GroomParameters(this->project_);

  bool global_icp = false;
  bool global_landmarks = false;
  // per-domain alignment
  for (size_t domain = 0; domain < num_domains; domain++) {
    if (this->abort_) {
      return false;
    }

    auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

    if (params.get_use_icp()) {
      global_icp = true;
      std::vector<Mesh> meshes;
      for (size_t i = 0; i < subjects.size(); i++) {
        auto mesh = this->get_mesh(i, domain);

        auto list = subjects[i]->get_groomed_transforms()[domain];
        vtkSmartPointer<vtkTransform> transform = ProjectUtils::convert_transform(list);
        mesh.applyTransform(transform);
        meshes.push_back(mesh);
      }

      size_t reference_mesh = MeshUtils::findReferenceMesh(meshes);

      auto transforms = Groom::get_icp_transforms(meshes, reference_mesh);

      assign_transforms(transforms, domain);
    } else if (params.get_use_landmarks()) {
      global_landmarks = true;
      std::vector<vtkSmartPointer<vtkPoints>> landmarks;
      for (size_t i = 0; i < subjects.size(); i++) {
        landmarks.push_back(get_landmarks(i, domain));
      }

      size_t reference = Groom::find_reference_landmarks(landmarks);

      auto transforms = Groom::get_landmark_transforms(landmarks, reference);
      assign_transforms(transforms, domain);
    }
  }

  if (num_domains > 1) {  // global alignment for multiple domains
    std::vector<Mesh> meshes;

    for (size_t i = 0; i < subjects.size(); i++) {
      Mesh mesh = this->get_mesh(i, 0);
      for (size_t domain = 1; domain < num_domains; domain++) {
        mesh += this->get_mesh(i, domain);  // combine
      }

      // grab the first domain's initial transform (e.g. potentially reflect) and use before ICP
      auto list = subjects[i]->get_groomed_transforms()[0];
      vtkSmartPointer<vtkTransform> transform = ProjectUtils::convert_transform(list);
      mesh.applyTransform(transform);

      meshes.push_back(mesh);
    }

    if (global_icp) {
      size_t reference_mesh = MeshUtils::findReferenceMesh(meshes);
      auto transforms = Groom::get_icp_transforms(meshes, reference_mesh);
      size_t domain = num_domains;  // end
      assign_transforms(transforms, domain, true /* global */);

    } else if (global_landmarks) {
      std::vector<vtkSmartPointer<vtkPoints>> landmarks = get_combined_points();

      size_t reference = Groom::find_reference_landmarks(landmarks);
      auto transforms = Groom::get_landmark_transforms(landmarks, reference);
      size_t domain = num_domains;  // end
      assign_transforms(transforms, domain, true /* global */);

    } else {  // just center

      for (size_t i = 0; i < subjects.size(); i++) {
        auto subject = subjects[i];
        auto transform = vtkSmartPointer<vtkTransform>::New();
        Groom::add_center_transform(transform, meshes[i]);
        // store transform
        size_t domain = num_domains;  // end
        subject->set_groomed_transform(domain, ProjectUtils::convert_transform(transform));
      }
    }
  }

  return true;
}

//---------------------------------------------------------------------------
void Groom::assign_transforms(std::vector<std::vector<double>> transforms, int domain, bool global) {
  auto subjects = this->project_->get_subjects();

  for (size_t i = 0; i < subjects.size(); i++) {
    auto subject = subjects[i];

    int base_domain = domain;
    if (global) {
      base_domain = 0;
    }

    auto list = subjects[i]->get_groomed_transforms()[base_domain];
    vtkSmartPointer<vtkTransform> transform = ProjectUtils::convert_transform(list);
    transform->PostMultiply();
    transform->Concatenate(ProjectUtils::convert_transform(transforms[i]));

    // store transform
    subject->set_groomed_transform(domain, ProjectUtils::convert_transform(transform));
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkMatrix4x4> Groom::compute_landmark_transform(vtkSmartPointer<vtkPoints> source,
                                                                vtkSmartPointer<vtkPoints> target) {
  auto landmark_transform = vtkSmartPointer<vtkLandmarkTransform>::New();
  landmark_transform->SetSourceLandmarks(source);
  landmark_transform->SetTargetLandmarks(target);
  landmark_transform->Update();

  vtkSmartPointer<vtkMatrix4x4> matrix = landmark_transform->GetMatrix();
  return matrix;
}

//---------------------------------------------------------------------------
std::string Groom::get_output_filename(std::string input, DomainType domain_type) {
  // grab parameters
  auto params = GroomParameters(this->project_);

  // if the project is not saved, use the path of the input filename
  auto filename = this->project_->get_filename();
  if (filename == "") {
    filename = input;
  }

  auto base = StringUtils::getPath(filename);
  if (base == filename) {
    base = ".";
  }

  auto prefix = params.get_groom_output_prefix();

  std::string suffix = "_DT.nrrd";
  if (domain_type == DomainType::Mesh) {
    suffix = "_groomed.vtk";
  }

  auto path = base;
  if (prefix != "") {
    path = base + "/" + prefix;

    try {
      boost::filesystem::create_directories(path);
    } catch (std::exception& e) {
      throw std::runtime_error("Unable to create groom output directory: \"" + path + "\"");
    }
  }

  auto output = path + "/" + StringUtils::getFileNameWithoutExtension(input) + suffix;

  return output;
}

//---------------------------------------------------------------------------
Mesh Groom::get_mesh(int subject, int domain) {
  auto subjects = this->project_->get_subjects();
  auto path = subjects[subject]->get_segmentation_filenames()[domain];

  if (subjects[subject]->get_domain_types()[domain] == DomainType::Image) {
    Image image(path);
    return image.toMesh(0.5);
  } else if (subjects[subject]->get_domain_types()[domain] == DomainType::Mesh) {
    Mesh mesh = MeshUtils::threadSafeReadMesh(path);
    return mesh;
  }
  throw std::invalid_argument("invalid domain type");
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPoints> Groom::get_landmarks(int subject, int domain) {
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  auto subjects = this->project_->get_subjects();
  auto path = subjects[subject]->get_landmarks_filenames()[domain];

  std::ifstream in(path.c_str());
  if (!in.good()) {
    return vtk_points;
  }
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) {
      break;
    }
    vtk_points->InsertNextPoint(x, y, z);
  }
  in.close();

  return vtk_points;
}

//---------------------------------------------------------------------------
double Groom::compute_landmark_distance(vtkSmartPointer<vtkPoints> one, vtkSmartPointer<vtkPoints> two) {
  double sumSquared = 0;
  if (one->GetNumberOfPoints() != two->GetNumberOfPoints()) {
    return -1;
  }

  for (int i = 0; i < one->GetNumberOfPoints(); i++) {
    sumSquared += vtkMath::Distance2BetweenPoints(one->GetPoint(i), two->GetPoint(i));
  }
  return sumSquared;
}

//---------------------------------------------------------------------------
int Groom::find_reference_landmarks(std::vector<vtkSmartPointer<vtkPoints>> landmarks) {
  std::vector<std::pair<int, int>> pairs;

  // enumerate all pairs
  for (size_t i = 0; i < landmarks.size(); i++) {
    for (size_t j = i + 1; j < landmarks.size(); j++) {
      pairs.push_back(std::make_pair(i, j));
    }
  }

  // map of pair to distance value
  std::map<size_t, double> results;
  // mutex for access to results
  tbb::mutex mutex;

  tbb::parallel_for(tbb::blocked_range<size_t>{0, pairs.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      auto pair = pairs[i];

      auto matrix = Groom::compute_landmark_transform(landmarks[pair.first], landmarks[pair.second]);

      auto transform = createMeshTransform(matrix);

      auto transformed = vtkSmartPointer<vtkPoints>::New();
      transform->TransformPoints(landmarks[pair.first], transformed);

      // compute distance
      double distance = Groom::compute_landmark_distance(landmarks[pair.second], transformed);
      {
        // lock and store results
        tbb::mutex::scoped_lock lock(mutex);
        results[i] = distance;
      }
    }
  });

  std::vector<double> means(landmarks.size(), 0);

  double count = landmarks.size() - 1;
  for (size_t i = 0; i < pairs.size(); i++) {
    auto pair = pairs[i];
    double result = results[i];
    means[pair.first] += result / count;
    means[pair.second] += result / count;
  }

  auto smallest = std::min_element(means.begin(), means.end());

  return std::distance(means.begin(), smallest);
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Groom::get_icp_transforms(const std::vector<Mesh> meshes, size_t reference) {
  std::vector<std::vector<double>> transforms(meshes.size());

  tbb::parallel_for(tbb::blocked_range<size_t>{0, meshes.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
      matrix->Identity();

      Mesh target = meshes[reference];
      if (i != reference) {
        Mesh source = meshes[i];
        matrix = MeshUtils::createICPTransform(source.getVTKMesh(), target.getVTKMesh(), Mesh::Rigid, 100, true);
      }

      auto transform = createMeshTransform(matrix);
      transform->PostMultiply();
      Groom::add_center_transform(transform, target);
      transforms[i] = ProjectUtils::convert_transform(transform);
    }
  });
  return transforms;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Groom::get_landmark_transforms(const std::vector<vtkSmartPointer<vtkPoints>> landmarks,
                                                                size_t reference) {
  std::vector<std::vector<double>> transforms(landmarks.size());

  tbb::parallel_for(tbb::blocked_range<size_t>{0, landmarks.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
      matrix->Identity();

      vtkSmartPointer<vtkPoints> target = landmarks[reference];
      if (i != reference) {
        vtkSmartPointer<vtkPoints> source = landmarks[i];
        matrix = Groom::compute_landmark_transform(source, target);
      }

      auto transform = createMeshTransform(matrix);
      transform->PostMultiply();
      Groom::add_center_transform(transform, target);
      transforms[i] = ProjectUtils::convert_transform(transform);
    }
  });
  return transforms;
}

//---------------------------------------------------------------------------
std::vector<double> Groom::get_identity_transform() {
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  return ProjectUtils::convert_transform(transform);
}

//---------------------------------------------------------------------------
void Groom::add_center_transform(vtkSmartPointer<vtkTransform> transform, const Image& image) {
  auto com = image.centerOfMass();
  transform->Translate(-com[0], -com[1], -com[2]);
}

//---------------------------------------------------------------------------
void Groom::add_center_transform(vtkSmartPointer<vtkTransform> transform, const Mesh& mesh) {
  auto com = mesh.centerOfMass();
  transform->Translate(-com[0], -com[1], -com[2]);
}

//---------------------------------------------------------------------------
void Groom::add_center_transform(vtkSmartPointer<vtkTransform> transform, vtkSmartPointer<vtkPoints> points) {
  double center[3];
  vtkCenterOfMass::ComputeCenterOfMass(points, nullptr, center);
  transform->Translate(-center[0], -center[1], -center[2]);
}

//---------------------------------------------------------------------------
std::vector<vtkSmartPointer<vtkPoints>> Groom::get_combined_points() {
  auto subjects = project_->get_subjects();
  size_t num_domains = this->project_->get_number_of_domains_per_subject();

  std::vector<vtkSmartPointer<vtkPoints>> landmarks;
  for (size_t i = 0; i < subjects.size(); i++) {
    auto points = vtkSmartPointer<vtkPoints>::New();
    for (size_t domain = 0; domain < num_domains; domain++) {
      auto add_points = get_landmarks(i, domain);
      for (int j = 0; j < add_points->GetNumberOfPoints(); j++) {
        points->InsertNextPoint(add_points->GetPoint(j));
      }
    }
  }
  return landmarks;
}

//---------------------------------------------------------------------------
void Groom::add_reflect_transform(vtkSmartPointer<vtkTransform> transform, const std::string& reflect_axis) {
  Vector scale(makeVector({1, 1, 1}));
  if (reflect_axis == "X") {
    scale[Axis::X] = -1;
  } else if (reflect_axis == "Y") {
    scale[Axis::Y] = -1;
  } else if (reflect_axis == "Z") {
    scale[Axis::Z] = -1;
  }
  transform->Scale(scale[0], scale[1], scale[2]);
}
//---------------------------------------------------------------------------
