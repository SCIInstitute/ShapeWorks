#include <Groom.h>
#include <GroomParameters.h>
#include <Image/Image.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshUtils.h>
#include <Optimize/Constraints/Constraints.h>
#include <Project/ProjectUtils.h>
#include <Utils/StringUtils.h>
#include <Utils/Utils.h>
#include <itkRegionOfInterestImageFilter.h>
#include <tbb/parallel_for.h>
#include <vtkCenterOfMass.h>
#include <vtkLandmarkTransform.h>
#include <vtkPointSet.h>

#include <boost/filesystem.hpp>
#include <vector>

using namespace shapeworks;

namespace {
// for concurrent access
static std::mutex mutex;
}  // namespace

using PixelType = float;
using ImageType = itk::Image<PixelType, 3>;

//---------------------------------------------------------------------------
Groom::Groom(ProjectHandle project) : project_{project} {}

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

  // clear alignment transforms
  for (auto& subject : subjects) {
    subject->set_groomed_transforms({});
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

        auto domain_name = project_->get_domain_names()[domain];
        // skip "shared_surface" and "shared_boundary"
        bool is_shared_boundary = domain_name.find("shared_surface_") != std::string::npos ||
                                  domain_name.find("shared_boundary_") != std::string::npos;
        if (is_shared_boundary) {
          SW_DEBUG("Skipping shared boundary domain '{}' during grooming", domain_name);
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

  if (success && !run_shared_boundaries()) {
    success = false;
  }

  if (success && !run_alignment()) {
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

  SW_DEBUG("Grooming image: {}", original);

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
  SW_DEBUG("Grooming mesh: {}", original);

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
      SW_DEBUG("Number of cells: {}", poly_data->GetNumberOfCells());
      SW_DEBUG("Number of points in first cell: {}", poly_data->GetCell(0)->GetNumberOfPoints());
      // write out to /tmp
      std::string tmpname = "/tmp/bad_mesh.vtk";
      MeshUtils::threadSafeWriteMesh(tmpname, mesh);
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
  SW_DEBUG("Running alignment, number of domains = {}", num_domains);
  auto subjects = project_->get_subjects();

  if (subjects.empty()) {
    SW_ERROR("No subjects to groom");
    return false;
  }

  auto base_params = GroomParameters(project_);

  bool global_icp = false;
  bool global_landmarks = false;
  bool any_alignment = false;

  int reference_index = -1;
  int subset_size = -1;

  // per-domain alignment
  for (size_t domain = 0; domain < num_domains; domain++) {
    auto params = GroomParameters(project_, project_->get_domain_names()[domain]);
    if (params.get_use_icp()) {
      global_icp = true;
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
        meshes.emplace_back(vtkSmartPointer<vtkPolyData>::New());
      }
    }

    if (global_icp) {
      Mesh reference_mesh = vtkSmartPointer<vtkPolyData>::New();
      if (reference_meshes.empty()) {
        SW_ERROR("No reference meshes available");
        return false;
      }
      if (reference_index < 0 || reference_index >= reference_meshes.size()) {
        reference_index = MeshUtils::findReferenceMesh(reference_meshes, subset_size);
        if (reference_index < 0 || reference_index >= reference_meshes.size()) {
          throw std::runtime_error("could not find reference mesh");
        }
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

    } else {
      std::vector<std::vector<double>> transforms;
      for (size_t i = 0; i < subjects.size(); i++) {
        auto subject = subjects[i];
        auto transform = vtkSmartPointer<vtkTransform>::New();
        if (any_alignment) {  // just center
          Groom::add_center_transform(transform, meshes[i]);
        }
        transforms.push_back(ProjectUtils::convert_transform(transform));
      }
      size_t domain = num_domains;  // end
      assign_transforms(transforms, domain, true /* global */);
    }
  }

  // per-domain alignment
  for (size_t domain = 0; domain < num_domains; domain++) {
    if (abort_) {
      return false;
    }

    auto params = GroomParameters(project_, project_->get_domain_names()[domain]);
    if (params.get_alignment_enabled()) {
      any_alignment = true;
    }

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

      if (reference_meshes.empty()) {
        SW_ERROR("No reference meshes available");
        return false;
      }

      Mesh reference_mesh = vtkSmartPointer<vtkPolyData>::New();
      if (reference_index < 0 || reference_index >= subjects.size()) {
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

      // confirm that each subject has landmarks and has the same number of landmarks
      for (size_t i = 0; i < subjects.size(); i++) {
        if (landmarks[i]->GetNumberOfPoints() == 0) {
          SW_ERROR(
              "Unable to perform landmark-based alignment.  Subject '{}' does not have landmarks defined for domain {}",
              subjects[i]->get_display_name(), domain);
          return false;
        }
        if (i > 0 && landmarks[i]->GetNumberOfPoints() != landmarks[0]->GetNumberOfPoints()) {
          SW_ERROR(
              "Unable to perform landmark-based alignment.  Subject '{}' has a different number of landmarks than "
              "subject '{}'",
              subjects[i]->get_display_name(), subjects[0]->get_display_name());
          return false;
        }
      }

      int reference_index = Groom::find_reference_landmarks(landmarks);
      params.set_alignment_reference_chosen(reference_index);
      params.save_to_project();

      auto transforms = Groom::get_landmark_transforms(landmarks, reference_index);
      assign_transforms(transforms, domain);
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::run_shared_boundaries() {
  // only need to check on one domain
  auto params = GroomParameters(project_, project_->get_domain_names()[0]);

  // first, we must remove any existing shared surfaces and boundaries from the project
  clear_unused_shared_boundaries();

  if (!params.get_shared_boundaries_enabled()) {
    return true;
  }

  SW_LOG("Processing shared boundaries...");

  auto domain_names = project_->get_domain_names();
  auto original_domain_types = project_->get_original_domain_types();
  auto groomed_domain_types = project_->get_groomed_domain_types();

  std::atomic<bool> error_encountered = false;

  for (const auto& shared_boundary : params.get_shared_boundaries()) {
    std::string first_domain_name = shared_boundary.first_domain;
    std::string second_domain_name = shared_boundary.second_domain;
    std::string shared_surface_name = "shared_surface_" + first_domain_name + "_" + second_domain_name;
    std::string shared_boundary_name = "shared_boundary_" + first_domain_name + "_" + second_domain_name;

    // if domain_names doesn't have the shared domains, add them
    if (std::find(domain_names.begin(), domain_names.end(), shared_surface_name) == domain_names.end()) {
      domain_names.push_back(shared_surface_name);
      original_domain_types.push_back(DomainType::Mesh);
      groomed_domain_types.push_back(DomainType::Mesh);
    }
    if (std::find(domain_names.begin(), domain_names.end(), shared_boundary_name) == domain_names.end()) {
      domain_names.push_back(shared_boundary_name);
      original_domain_types.push_back(DomainType::Contour);
      groomed_domain_types.push_back(DomainType::Contour);
    }

    int shared_domain_index =
        std::find(domain_names.begin(), domain_names.end(), shared_surface_name) - domain_names.begin();
    int shared_boundary_index =
        std::find(domain_names.begin(), domain_names.end(), shared_boundary_name) - domain_names.begin();

    // find the index of the first domain and second domain
    int first_domain = 0;
    int second_domain = 1;
    for (int i = 0; i < domain_names.size(); i++) {
      if (domain_names[i] == first_domain_name) {
        first_domain = i;
      }
      if (domain_names[i] == second_domain_name) {
        second_domain = i;
      }
    }

    groomed_domain_types[first_domain] = DomainType::Mesh;
    groomed_domain_types[second_domain] = DomainType::Mesh;

    project_->set_domain_names(domain_names);
    project_->set_original_domain_types(original_domain_types);
    project_->set_groomed_domain_types(groomed_domain_types);

    auto subjects = project_->get_subjects();

    std::mutex progress_mutex;
    int progress = 0;

    tbb::parallel_for(tbb::blocked_range<size_t>{0, subjects.size()}, [&](const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i < r.end(); ++i) {
        if (abort_) {
          return;
        }

        auto subject = subjects[i];

        Mesh first_mesh = get_mesh(i, first_domain, false, MeshSource::Groomed);
        Mesh second_mesh = get_mesh(i, second_domain, false, MeshSource::Groomed);

        // create an empty vtk mesh
        auto empty_mesh = vtkSmartPointer<vtkPolyData>::New();

        Mesh extracted_l(empty_mesh);
        Mesh extracted_r(empty_mesh);
        Mesh extracted_s(empty_mesh);
        Mesh output_contour(empty_mesh);
        try {
          // returns left remainder, right remainder, and shared_surface
          std::tie(extracted_l, extracted_r, extracted_s) =
              MeshUtils::shared_boundary_extractor(first_mesh, second_mesh, shared_boundary.tolerance);

          extracted_l.remeshPercent(.99, 1.0);
          extracted_r.remeshPercent(.99, 1.0);

          output_contour = MeshUtils::extract_boundary_loop(extracted_s);

        } catch (const std::exception& e) {
          SW_ERROR("Error extracting shared boundary for subject '{}': {}", subject->get_display_name(), e.what());
          // need to continue to write the empty meshes out or we will have a broken project that can no longer be saved
          error_encountered = true;
        }

        // overwrite groomed filename for first and second with extracted_l and extracted_r
        auto first_filename = subject->get_groomed_filenames()[first_domain];
        auto second_filename = subject->get_groomed_filenames()[second_domain];
        // change file extension to .vtk if it's not already
        first_filename = StringUtils::removeExtension(first_filename) + "_extracted.vtk";
        second_filename = StringUtils::removeExtension(second_filename) + "_extracted.vtk";
        MeshUtils::threadSafeWriteMesh(first_filename, extracted_l);
        MeshUtils::threadSafeWriteMesh(second_filename, extracted_r);

        auto shared_surface_filename = get_output_filename(shared_surface_name, DomainType::Mesh);
        auto shared_boundary_filename = get_output_filename(shared_boundary_name, DomainType::Contour);

        extracted_s.write(shared_surface_filename);
        output_contour.write(shared_boundary_filename);

        // store filenames
        auto groomed_filenames = subject->get_groomed_filenames();
        groomed_filenames[first_domain] = first_filename;
        groomed_filenames[second_domain] = second_filename;
        if (shared_domain_index >= groomed_filenames.size()) {
          groomed_filenames.resize(shared_domain_index + 1);
        }
        if (shared_boundary_index >= groomed_filenames.size()) {
          groomed_filenames.resize(shared_boundary_index + 1);
        }
        groomed_filenames[shared_domain_index] = shared_surface_filename;
        groomed_filenames[shared_boundary_index] = shared_boundary_filename;
        subject->set_groomed_filenames(groomed_filenames);
        subject->set_number_of_domains(domain_names.size());

        // also store the two new ones as original
        auto original_filenames = subject->get_original_filenames();
        if (original_filenames.size() <= shared_domain_index) {
          original_filenames.resize(shared_domain_index + 1);
        }
        if (original_filenames.size() <= shared_boundary_index) {
          original_filenames.resize(shared_boundary_index + 1);
        }
        original_filenames[shared_domain_index] = shared_surface_filename;
        original_filenames[shared_boundary_index] = shared_boundary_filename;
        subject->set_original_filenames(original_filenames);

        {
          // lock
          std::scoped_lock lock(progress_mutex);
          progress++;
          progress_ = static_cast<float>(progress) / static_cast<float>(subjects.size()) * 100.0;
          SW_PROGRESS(progress_, fmt::format("Processing shared boundaries ({}/{})", progress, subjects.size()));
        }
      }
    });
  }

  if (error_encountered) {
    SW_ERROR("Errors encountered while processing shared boundaries. Please check the log for details.");

    // remove shared_surface and shared_boundary from domain_names
    auto domain_names = project_->get_domain_names();
    domain_names.erase(std::remove_if(domain_names.begin(), domain_names.end(),
                                      [](const std::string& name) {
                                        return name.find("shared_surface_") != std::string::npos ||
                                               name.find("shared_boundary_") != std::string::npos;
                                      }),
                       domain_names.end());
    project_->set_domain_names(domain_names);

    // go back and delete all the shared surfaces and boundaries that were created for all subjects
    auto subjects = project_->get_subjects();
    for (auto& subject : subjects) {
      auto original_filenames = subject->get_original_filenames();

      for (auto it = original_filenames.begin(); it != original_filenames.end();) {
        if (it->find("shared_surface_") != std::string::npos || it->find("shared_boundary_") != std::string::npos) {
          SW_DEBUG("Removing original filename: {}", *it);
          Utils::quiet_delete_file(*it);
          it = original_filenames.erase(it);
        } else {
          ++it;
        }
      }
      subject->set_original_filenames(original_filenames);

      subject->set_groomed_filenames({});
      subject->set_number_of_domains(domain_names.size());
    }

    return false;
  }

  return true;
}

//---------------------------------------------------------------------------
void Groom::clear_unused_shared_boundaries() {
  auto params = GroomParameters(project_, project_->get_domain_names()[0]);

  std::vector<std::string> shared_surfaces_to_keep;
  std::vector<std::string> shared_boundaries_to_keep;

  if (params.get_shared_boundaries_enabled()) {
    for (const auto& shared_boundary : params.get_shared_boundaries()) {
      std::string first_domain_name = shared_boundary.first_domain;
      std::string second_domain_name = shared_boundary.second_domain;
      std::string shared_surface_name = "shared_surface_" + first_domain_name + "_" + second_domain_name;
      std::string shared_boundary_name = "shared_boundary_" + first_domain_name + "_" + second_domain_name;
      shared_surfaces_to_keep.push_back(shared_surface_name);
      shared_boundaries_to_keep.push_back(shared_boundary_name);
    }
  }

  // Lambda to check if a name is a shared domain that should be removed
  auto is_shared_domain_to_remove = [&shared_surfaces_to_keep, &shared_boundaries_to_keep](const std::string& name) {
    // name may be dir/file and we only care about file
    std::string filename = StringUtils::getFilename(name);

    bool is_shared =
        filename.find("shared_surface_") != std::string::npos || filename.find("shared_boundary_") != std::string::npos;

    if (!is_shared) {
      return false;  // Not a shared domain at all
    }

    // Check if it's in either keep list
    bool should_keep = std::find(shared_surfaces_to_keep.begin(), shared_surfaces_to_keep.end(), name) !=
                           shared_surfaces_to_keep.end() ||
                       std::find(shared_boundaries_to_keep.begin(), shared_boundaries_to_keep.end(), name) !=
                           shared_boundaries_to_keep.end();

    return !should_keep;  // Remove if it's shared but not in keep lists
  };

  // Lambda to remove shared files from a vector and delete them from disk
  auto remove_shared_files = [&is_shared_domain_to_remove](std::vector<std::string>& filenames) {
    for (auto it = filenames.begin(); it != filenames.end();) {
      if (is_shared_domain_to_remove(*it)) {
        std::cerr << "Removing file: " << *it << "\n";
        Utils::quiet_delete_file(*it);
        it = filenames.erase(it);
      } else {
        ++it;
      }
    }
  };

  // Lambda to remove elements from vector by matching domain names
  auto remove_by_domain_names = [&is_shared_domain_to_remove](auto& vec, const std::vector<std::string>& domain_names) {
    for (auto it = vec.begin(); it != vec.end();) {
      size_t index = std::distance(vec.begin(), it);
      if (index < domain_names.size() && is_shared_domain_to_remove(domain_names[index])) {
        it = vec.erase(it);
      } else {
        ++it;
      }
    }
  };

  auto domain_names = project_->get_domain_names();
  auto original_domain_names = domain_names;  // Save original for reference

  // Remove shared domains from domain_names
  domain_names.erase(std::remove_if(domain_names.begin(), domain_names.end(), is_shared_domain_to_remove),
                     domain_names.end());

  // Remove corresponding entries from domain types arrays using original domain names as reference
  auto original_domain_types = project_->get_original_domain_types();
  auto groomed_domain_types = project_->get_groomed_domain_types();

  remove_by_domain_names(original_domain_types, original_domain_names);
  remove_by_domain_names(groomed_domain_types, original_domain_names);

  project_->set_domain_names(domain_names);
  project_->set_original_domain_types(original_domain_types);
  project_->set_groomed_domain_types(groomed_domain_types);

  // Clean up files from all subjects
  auto subjects = project_->get_subjects();
  for (auto& subject : subjects) {
    // Remove shared surface/boundary files using lambda
    auto original_filenames = subject->get_original_filenames();
    remove_shared_files(original_filenames);
    subject->set_original_filenames(original_filenames);

    auto groomed_filenames = subject->get_groomed_filenames();
    remove_shared_files(groomed_filenames);
    subject->set_groomed_filenames(groomed_filenames);

    // Remove alignment transforms for removed domains
    auto transforms = subject->get_groomed_transforms();
    remove_by_domain_names(transforms, original_domain_names);
    subject->set_groomed_transforms(transforms);

    subject->set_local_particle_filenames({});
    subject->set_world_particle_filenames({});

    subject->set_procrustes_transforms({});
    subject->set_number_of_domains(domain_names.size());
  }

  project_->set_domain_names(domain_names);
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
  if (filename.empty()) {
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
  } else if (domain_type == DomainType::Contour) {
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
  while (used_names_.find(name + suffix) != used_names_.end()) {
    name = base_name + "_" + std::to_string(count++);
  }
  auto output = name + suffix;
  used_names_.insert(output);
  return output;
}

//---------------------------------------------------------------------------
Mesh Groom::get_mesh(int subject, int domain, bool transformed, MeshSource source) {
  auto subjects = project_->get_subjects();
  if (subject >= subjects.size()) {
    throw std::out_of_range("subject index out of range");
  }

  std::string path;

  if (source == MeshSource::Original) {
    if (domain >= subjects[subject]->get_original_filenames().size()) {
      throw std::out_of_range("domain index out of range");
    }
    path = subjects[subject]->get_original_filenames()[domain];
  } else {
    if (domain >= subjects[subject]->get_groomed_filenames().size()) {
      throw std::out_of_range("domain index out of range");
    }
    path = subjects[subject]->get_groomed_filenames()[domain];
  }

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
  } else if (project_->get_original_domain_types()[domain] == DomainType::Contour) {
    mesh = MeshUtils::threadSafeReadMesh(path);
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
  auto landmarks_filenames = subjects[subject]->get_landmarks_filenames();
  if (domain >= landmarks_filenames.size()) {
    return vtk_points;
  }
  auto path = landmarks_filenames[domain];

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

  tbb::parallel_for(tbb::blocked_range<size_t>{0, meshes.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      // for (size_t i = 0; i < meshes.size(); i++) {
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
