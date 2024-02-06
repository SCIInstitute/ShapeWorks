#include <Groom.h>
#include <GroomParameters.h>
#include <Image/Image.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshUtils.h>
#include <Optimize/Constraints/Constraints.h>
#include <Project/ProjectUtils.h>
#include <Utils/StringUtils.h>
#include <itkRegionOfInterestImageFilter.h>
#include <tbb/parallel_for.h>
#include <vtkCenterOfMass.h>
#include <vtkLandmarkTransform.h>
#include <vtkPointSet.h>

#include <boost/filesystem.hpp>
#include <vector>

using namespace shapeworks;

// for concurrent access
static std::mutex mutex;

typedef float PixelType;
typedef itk::Image<PixelType, 3> ImageType;

//---------------------------------------------------------------------------
Groom::Groom(ProjectHandle project) { project_ = project; }

//---------------------------------------------------------------------------
bool Groom::run() {
  ShapeWorksUtils::setup_threads();

  used_names_.clear();
  progress_ = 0;
  progress_counter_ = 0;

  auto subjects = project_->get_subjects();

  if (subjects.empty()) {
    throw std::invalid_argument("No subjects to groom");
  }

  total_ops_ = get_total_ops();

  std::atomic<bool> success = true;

  tbb::parallel_for(tbb::blocked_range<size_t>{0, subjects.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      for (int domain = 0; domain < project_->get_number_of_domains_per_subject(); domain++) {
        if (abort_) {
          success = false;
          continue;
        }

        if (subjects[i]->is_fixed()) {
          continue;
        }

        if (subjects[i]->is_excluded()) {
          // clear groomed filenames
          subjects[i]->set_groomed_filenames(std::vector<std::string>());
          return;
        }

        bool is_image = project_->get_original_domain_types()[domain] == DomainType::Image;
        bool is_mesh = project_->get_original_domain_types()[domain] == DomainType::Mesh;
        bool is_contour = project_->get_original_domain_types()[domain] == DomainType::Contour;

        if (is_image) {
          if (!image_pipeline(subjects[i], domain)) {
            success = false;
          }
        }

        if (is_mesh) {
          if (!mesh_pipeline(subjects[i], domain)) {
            success = false;
          }
        }

        if (is_contour) {
          if (!contour_pipeline(subjects[i], domain)) {
            success = false;
          }
        }
      }
    }
  });

  if (!run_alignment()) {
    success = false;
  }
  increment_progress(10);  // alignment complete

  project_->update_subjects();
  return success;
}

//---------------------------------------------------------------------------
bool Groom::image_pipeline(std::shared_ptr<Subject> subject, size_t domain) {
  // grab parameters
  auto params = GroomParameters(project_, project_->get_domain_names()[domain]);

  auto original = subject->get_original_filenames()[domain];

  // load the image
  Image image(original);

  // define a groom transform
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();

  if (params.get_skip_grooming()) {
    std::vector<std::vector<double>> groomed_transforms;
    groomed_transforms.push_back(ProjectUtils::convert_transform(transform));
    subject->set_groomed_transforms(groomed_transforms);

    {
      // lock for project data structure
      std::scoped_lock lock(mutex_);

      // update groomed filenames
      std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
      if (domain >= groomed_filenames.size()) {
        groomed_filenames.resize(domain + 1);
      }
      groomed_filenames[domain] = original;

      // store filenames back to subject
      subject->set_groomed_filenames(groomed_filenames);
    }

    return true;
  }

  run_image_pipeline(image, params);

  // reflection
  if (params.get_reflect()) {
    auto table = subject->get_table_values();
    if (table.find(params.get_reflect_column()) != table.end()) {
      if (table[params.get_reflect_column()] == params.get_reflect_choice()) {
        add_reflect_transform(transform, params.get_reflect_axis());
      }
    }
  }

  // centering
  if (params.get_use_center()) {
    add_center_transform(transform, image);
  }

  if (abort_) {
    return false;
  }

  // groomed filename
  std::string groomed_name = get_output_filename(original, DomainType::Image);

  if (params.get_convert_to_mesh()) {
    Mesh mesh = image.toMesh(0.0);
    run_mesh_pipeline(mesh, params);
    groomed_name = get_output_filename(original, DomainType::Mesh);
    // save the groomed mesh
    MeshUtils::threadSafeWriteMesh(groomed_name, mesh);
  } else {
    // save image
    image.write(groomed_name);
  }

  {
    // lock for project data structure
    std::scoped_lock lock(mutex_);

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
    increment_progress();
  }
  if (abort_) {
    return false;
  }

  // fill holes
  if (params.get_fill_holes_tool()) {
    image.closeHoles();
    increment_progress();
  }
  if (abort_) {
    return false;
  }

  // crop
  if (params.get_crop()) {
    PhysicalRegion region = image.physicalBoundingBox(0.5);
    image.crop(region);
    increment_progress();
  }
  if (abort_) {
    return false;
  }

  // autopad
  if (params.get_auto_pad_tool()) {
    image.pad(params.get_padding_amount());
    fix_origin(image);
    increment_progress();
  }
  if (abort_) {
    return false;
  }

  // antialias
  if (params.get_antialias_tool()) {
    image.antialias(params.get_antialias_iterations());
    increment_progress();
  }
  if (abort_) {
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
    increment_progress();
  }
  if (abort_) {
    return false;
  }

  // create distance transform
  if (params.get_fast_marching()) {
    image.computeDT();
    increment_progress(10);
  }

  if (abort_) {
    return false;
  }

  // blur
  if (params.get_blur_tool()) {
    image.gaussianBlur(params.get_blur_amount());
    increment_progress();
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain) {
  // grab parameters
  auto params = GroomParameters(project_, project_->get_domain_names()[domain]);

  auto original = subject->get_original_filenames()[domain];

  // groomed mesh name
  std::string groom_name = get_output_filename(original, DomainType::Mesh);

  Mesh mesh = MeshUtils::threadSafeReadMesh(original);

  // define a groom transform
  auto transform = vtkSmartPointer<vtkTransform>::New();

  if (!params.get_skip_grooming()) {
    run_mesh_pipeline(mesh, params);

    // reflection
    if (params.get_reflect()) {
      auto table = subject->get_table_values();
      if (table.find(params.get_reflect_column()) != table.end()) {
        if (table[params.get_reflect_column()] == params.get_reflect_choice()) {
          add_reflect_transform(transform, params.get_reflect_axis());
        }
      }
    }

    // centering
    if (params.get_use_center()) {
      add_center_transform(transform, mesh);
    }
    // save the groomed mesh
    MeshUtils::threadSafeWriteMesh(groom_name, mesh);
  } else {
    groom_name = original;
  }

  {
    // lock for project data structure
    std::scoped_lock lock(mutex_);

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
    increment_progress();
  }

  if (params.get_remesh()) {
    auto poly_data = mesh.getVTKMesh();
    if (poly_data->GetNumberOfCells() == 0 || poly_data->GetCell(0)->GetNumberOfPoints() == 2) {
      throw std::runtime_error("malformed mesh, mesh should be triangular");
    }
    int total_vertices = mesh.getVTKMesh()->GetNumberOfPoints();
    int num_vertices = params.get_remesh_num_vertices();
    if (params.get_remesh_percent_mode()) {
      num_vertices = total_vertices * params.get_remesh_percent() / 100.0;
    }
    num_vertices = std::max<int>(num_vertices, 25);
    double gradation = clamp<double>(params.get_remesh_gradation(), 0.0, 2.0);
    mesh.remesh(num_vertices, gradation);
    increment_progress();
  }

  if (params.get_mesh_smooth()) {
    if (params.get_mesh_smoothing_method() == GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C) {
      mesh.smooth(params.get_mesh_vtk_laplacian_iterations(), params.get_mesh_vtk_laplacian_relaxation());
    } else if (params.get_mesh_smoothing_method() == GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C) {
      mesh.smoothSinc(params.get_mesh_vtk_windowed_sinc_iterations(), params.get_mesh_vtk_windowed_sinc_passband());
    }
    increment_progress();
  }
  return true;
}

//---------------------------------------------------------------------------
bool Groom::contour_pipeline(std::shared_ptr<Subject> subject, size_t domain) {
  // grab parameters
  auto params = GroomParameters(project_, project_->get_domain_names()[domain]);

  auto original = subject->get_original_filenames()[domain];

  // groomed mesh name
  std::string groom_name = get_output_filename(original, DomainType::Mesh);

  Mesh mesh = MeshUtils::threadSafeReadMesh(original);

  // define a groom transform
  auto transform = vtkSmartPointer<vtkTransform>::New();

  if (!params.get_skip_grooming()) {
    // reflection
    if (params.get_reflect()) {
      auto table = subject->get_table_values();
      if (table.find(params.get_reflect_column()) != table.end()) {
        if (table[params.get_reflect_column()] == params.get_reflect_choice()) {
          add_reflect_transform(transform, params.get_reflect_axis());
        }
      }
    }

    // centering
    if (params.get_use_center()) {
      add_center_transform(transform, mesh);
    }

    // save the groomed contour
    MeshUtils::threadSafeWriteMesh(groom_name, mesh);

  } else {
    groom_name = original;
  }

  {
    // lock for project data structure
    std::scoped_lock lock(mutex_);

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
  int num_tools = 0;

  project_->update_subjects();
  auto domains = project_->get_domain_names();
  auto subjects = project_->get_subjects();

  for (int i = 0; i < domains.size(); i++) {
    auto params = GroomParameters(project_, domains[i]);

    if (project_->get_original_domain_types().size() <= i) {
      throw std::runtime_error("invalid domain, number of original file types does not match number of domains");
    }

    if (project_->get_original_domain_types()[i] == DomainType::Image) {
      num_tools += params.get_isolate_tool() ? 1 : 0;
      num_tools += params.get_fill_holes_tool() ? 1 : 0;
      num_tools += params.get_crop() ? 1 : 0;
      num_tools += params.get_auto_pad_tool() ? 1 : 0;
      num_tools += params.get_antialias_tool() ? 1 : 0;
      num_tools += params.get_resample() ? 1 : 0;
      num_tools += params.get_fast_marching() ? 10 : 0;
      num_tools += params.get_blur_tool() ? 1 : 0;
    }

    bool run_mesh = project_->get_original_domain_types()[i] == DomainType::Mesh ||
                    (project_->get_original_domain_types()[i] == DomainType::Image && params.get_convert_to_mesh());

    if (run_mesh) {
      num_tools += params.get_fill_mesh_holes_tool() ? 1 : 0;
      num_tools += params.get_mesh_smooth() ? 1 : 0;
      num_tools += params.get_remesh() ? 1 : 0;
    }
  }

  // count non-excluded subjects
  int num_non_excluded = 0;
  for (int i = 0; i < subjects.size(); i++) {
    if (!subjects[i]->is_excluded()) {
      num_non_excluded++;
    }
  }

  // +10 for alignment
  return num_non_excluded * num_tools + 10;
}

//---------------------------------------------------------------------------
void Groom::increment_progress(int amount) {
  std::scoped_lock lock(mutex);
  progress_counter_ += amount;
  progress_ = static_cast<float>(progress_counter_) / static_cast<float>(total_ops_) * 100.0;
  SW_PROGRESS(progress_, fmt::format("Grooming ({}/{} ops)", progress_counter_, total_ops_));
}

//---------------------------------------------------------------------------
void Groom::abort() { abort_ = true; }

//---------------------------------------------------------------------------
bool Groom::get_aborted() { return abort_; }

//---------------------------------------------------------------------------
bool Groom::run_alignment() {
  size_t num_domains = project_->get_number_of_domains_per_subject();
  auto subjects = project_->get_subjects();

  auto base_params = GroomParameters(project_);

  bool global_icp = false;
  bool global_landmarks = false;

  int reference_index = -1;
  int subset_size = -1;

  // per-domain alignment
  for (size_t domain = 0; domain < num_domains; domain++) {
    if (abort_) {
      return false;
    }

    auto params = GroomParameters(project_, project_->get_domain_names()[domain]);

    if (params.get_use_icp()) {
      global_icp = true;
      std::vector<Mesh> reference_meshes;
      std::vector<Mesh> meshes;
      for (size_t i = 0; i < subjects.size(); i++) {
        if (!subjects[i]->is_excluded()) {
          Mesh mesh = get_mesh(i, domain, true);
          // if fixed subjects are present, only add the fixed subjects
          if (subjects[i]->is_fixed() || !project_->get_fixed_subjects_present()) {
            reference_meshes.push_back(mesh);
          }
          meshes.push_back(mesh);
        }
      }

      reference_index = params.get_alignment_reference();
      subset_size = params.get_alignment_subset_size();

      Mesh reference_mesh = vtkSmartPointer<vtkPolyData>::New();
      if (reference_index < 0 || reference_index >= reference_meshes.size()) {
        reference_index = MeshUtils::findReferenceMesh(reference_meshes, subset_size);
        reference_index = reference_meshes[reference_index].get_id();
      }
      reference_mesh = get_mesh(reference_index, domain, true);

      params.set_alignment_reference_chosen(reference_index);
      params.save_to_project();

      auto transforms = Groom::get_icp_transforms(meshes, reference_mesh);
      assign_transforms(transforms, domain);
    } else if (params.get_use_landmarks()) {
      global_landmarks = true;
      std::vector<vtkSmartPointer<vtkPoints>> landmarks;
      for (size_t i = 0; i < subjects.size(); i++) {
        landmarks.push_back(get_landmarks(i, domain));
      }

      int reference_index = Groom::find_reference_landmarks(landmarks);
      params.set_alignment_reference_chosen(reference_index);
      params.save_to_project();

      auto transforms = Groom::get_landmark_transforms(landmarks, reference_index);
      assign_transforms(transforms, domain);
    }
  }

  if (num_domains > 1) {  // global alignment for multiple domains
    std::vector<Mesh> reference_meshes;
    std::vector<Mesh> meshes;

    for (size_t i = 0; i < subjects.size(); i++) {
      Mesh mesh = get_mesh(i, 0, true);
      for (size_t domain = 1; domain < num_domains; domain++) {
        mesh += get_mesh(i, domain, true);  // combine
      }

      // grab the first domain's initial transform (e.g. potentially reflect) and use before ICP
      auto list = subjects[i]->get_groomed_transforms()[0];
      auto transform = ProjectUtils::convert_transform(list);
      mesh.applyTransform(transform);

      if (!subjects[i]->is_excluded()) {
        // if fixed subjects are present, only add the fixed subjects
        if (subjects[i]->is_fixed() || !project_->get_fixed_subjects_present()) {
          reference_meshes.push_back(mesh);
        }
        meshes.push_back(mesh);
      } else {
        // insert blank for each excluded shape
        meshes.push_back(vtkSmartPointer<vtkPolyData>::New());
      }
    }

    if (global_icp) {
      std::vector<Mesh> reference_meshes;

      Mesh reference_mesh = vtkSmartPointer<vtkPolyData>::New();
      if (reference_index < 0 || reference_index >= reference_meshes.size()) {
        reference_index = MeshUtils::findReferenceMesh(reference_meshes, subset_size);
        reference_mesh = reference_meshes[reference_index];
      } else {
        reference_mesh = get_mesh(reference_index, 0, true);
        for (size_t domain = 1; domain < num_domains; domain++) {
          reference_mesh += get_mesh(reference_index, domain, true);  // combine
        }
      }
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
      std::vector<std::vector<double>> transforms;
      for (size_t i = 0; i < subjects.size(); i++) {
        auto subject = subjects[i];
        auto transform = vtkSmartPointer<vtkTransform>::New();
        Groom::add_center_transform(transform, meshes[i]);
        transforms.push_back(ProjectUtils::convert_transform(transform));
      }
      size_t domain = num_domains;  // end
      assign_transforms(transforms, domain, true /* global */);
    }
  }

  return true;
}

//---------------------------------------------------------------------------
void Groom::assign_transforms(std::vector<std::vector<double>> transforms, int domain, bool global) {
  auto subjects = project_->get_subjects();

  int count = 0;
  for (size_t i = 0; i < subjects.size(); i++) {
    if (subjects[i]->is_excluded()) {
      continue;
    }
    auto subject = subjects[i];

    int base_domain = domain;
    if (global) {
      base_domain = 0;
    }

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();
    if (base_domain < subjects[i]->get_groomed_transforms().size()) {
      auto list = subjects[i]->get_groomed_transforms()[base_domain];
      transform = ProjectUtils::convert_transform(list);
    }
    transform->PostMultiply();
    transform->Concatenate(ProjectUtils::convert_transform(transforms[count++]));

    // store transform
    if (!subject->is_fixed()) {
      subject->set_groomed_transform(domain, ProjectUtils::convert_transform(transform));
    }
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
  // lock for thread-safe member access
  std::scoped_lock lock(mutex_);

  // grab parameters
  auto params = GroomParameters(project_);

  // if the project is not saved, use the path of the input filename
  auto filename = project_->get_filename();
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
      if (!boost::filesystem::exists(path)) {
        boost::filesystem::create_directories(path);
      }
    } catch (std::exception& e) {
      throw std::runtime_error("Unable to create groom output directory: \"" + path + "\"");
    }
  }

  auto name = path + "/" + StringUtils::getBaseFilenameWithoutExtension(input);

  // check for and handle name clashes, e.g. a/foo.vtk, b/foo.vtk (see #1387)
  auto base_name = name;
  int count = 2;
  while (used_names_.find(name) != used_names_.end()) {
    name = base_name + std::to_string(count++);
  }
  used_names_.insert(name);

  auto output = name + suffix;

  return output;
}

//---------------------------------------------------------------------------
Mesh Groom::get_mesh(int subject, int domain, bool transformed) {
  auto subjects = project_->get_subjects();
  assert(subject < subjects.size());
  assert(domain < subjects[subject]->get_original_filenames().size());
  auto path = subjects[subject]->get_original_filenames()[domain];

  auto constraint_filename = subjects[subject]->get_constraints_filenames();
  Constraints constraint;
  if (constraint_filename.size() > domain) {
    constraint.read(constraint_filename[domain]);
  }

  if (project_->get_original_domain_types()[domain] == DomainType::Contour) {
    Mesh mesh = MeshUtils::threadSafeReadMesh(path);
    mesh.set_id(subject);
    return mesh;
  }

  Mesh mesh = vtkSmartPointer<vtkPolyData>::New();

  if (project_->get_original_domain_types()[domain] == DomainType::Image) {
    Image image(path);
    mesh = image.toMesh(0.5);
    constraint.clipMesh(mesh);
  } else if (project_->get_original_domain_types()[domain] == DomainType::Mesh) {
    mesh = MeshUtils::threadSafeReadMesh(path);
    constraint.clipMesh(mesh);
  } else {
    throw std::invalid_argument("invalid domain type");
  }

  if (transformed) {
    auto transforms = subjects[subject]->get_groomed_transforms();
    if (transforms.size() > domain) {
      auto list = transforms[domain];
      auto transform = ProjectUtils::convert_transform(list);
      mesh.applyTransform(transform);
    }
  }

  mesh.set_id(subject);
  return mesh;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPoints> Groom::get_landmarks(int subject, int domain) {
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  auto subjects = project_->get_subjects();
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
  std::mutex mutex;

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
        std::scoped_lock lock(mutex);
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
std::vector<std::vector<double>> Groom::get_icp_transforms(const std::vector<Mesh> meshes, Mesh reference) {
  std::vector<std::vector<double>> transforms(meshes.size());

  // tbb::parallel_for(tbb::blocked_range<size_t>{0, meshes.size()}, [&](const tbb::blocked_range<size_t>& r) {
  // for (size_t i = r.begin(); i < r.end(); ++i) {
  for (size_t i = 0; i < meshes.size(); i++) {
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    matrix->Identity();

    Mesh source = meshes[i];
    if (source.getVTKMesh()->GetNumberOfPoints() != 0) {
      // create copies for thread safety
      auto poly_data1 = vtkSmartPointer<vtkPolyData>::New();
      poly_data1->DeepCopy(source.getVTKMesh());
      auto poly_data2 = vtkSmartPointer<vtkPolyData>::New();
      poly_data2->DeepCopy(reference.getVTKMesh());

      matrix = MeshUtils::createICPTransform(poly_data1, poly_data2, Mesh::Rigid, 100, true);
    }
    auto transform = createMeshTransform(matrix);
    transform->PostMultiply();
    Groom::add_center_transform(transform, reference);
    transforms[i] = ProjectUtils::convert_transform(transform);
  }
  //  });

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
  size_t num_domains = project_->get_number_of_domains_per_subject();

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
