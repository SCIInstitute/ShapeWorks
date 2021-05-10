#include <functional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <Libs/Image/Image.h>
#include <Libs/Utils/StringUtils.h>
#include <Libs/Mesh/MeshUtils.h>
#include <ParticleSystem/VtkMeshWrapper.h>

#include "OptimizeParameters.h"
#include "Optimize.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
OptimizeParameters::OptimizeParameters(ProjectHandle project)
{
  this->project_ = project;
  this->params_ = this->project_->get_parameters(Parameters::OPTIMIZE_PARAMS);
}

//---------------------------------------------------------------------------
void OptimizeParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::OPTIMIZE_PARAMS, this->params_);
}

//---------------------------------------------------------------------------
std::vector<int> OptimizeParameters::get_number_of_particles()
{
  return this->params_.get("number_of_particles", {128});
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_number_of_particles(std::vector<int> number_of_particles)
{
  return this->params_.set("number_of_particles", number_of_particles);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_initial_relative_weighting()
{
  return this->params_.get("initial_relative_weighting", 0.05);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_initial_relative_weighting(double value)
{
  this->params_.set("initial_relative_weighting", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_relative_weighting()
{
  return this->params_.get("relative_weighting", 1.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_relative_weighting(double value)
{
  this->params_.set("relative_weighting", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_starting_regularization()
{
  return this->params_.get("starting_regularization", 1000.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_starting_regularization(double value)
{
  this->params_.set("starting_regularization", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_ending_regularization()
{
  return this->params_.get("ending_regularization", 10.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_ending_regularization(double value)
{
  this->params_.set("ending_regularization", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_iterations_per_split()
{
  return this->params_.get("iterations_per_split", 1000);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_iterations_per_split(int value)
{
  this->params_.set("iterations_per_split", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_optimization_iterations()
{
  return this->params_.get("optimization_iterations", 1000);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_optimization_iterations(int value)
{
  this->params_.set("optimization_iterations", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_geodesic_distance()
{
  return this->params_.get("use_geodesic_distance", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_geodesic_distance(bool value)
{
  this->params_.set("use_geodesic_distance", value);
}

//---------------------------------------------------------------------------
std::vector<bool> OptimizeParameters::get_use_normals()
{
  std::vector<bool> use_normals = this->params_.get("use_normals", {false});
  if (use_normals.empty()) use_normals.push_back(false);
  return use_normals;
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_normals(std::vector<bool> use_normals)
{
  this->params_.set("use_normals", use_normals);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_normals_strength()
{
  return this->params_.get("normals_strength", 10);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_normals_strength(double value)
{
  this->params_.set("normals_strength", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_procrustes()
{
  return this->params_.get("procrustes", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_procrustes(bool value)
{
  this->params_.set("procrustes", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_procrustes_scaling()
{
  return this->params_.get("procrustes_scaling", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_procrustes_scaling(bool value)
{
  this->params_.set("procrustes_scaling", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_procrustes_interval()
{
  return this->params_.get("procrustes_interval", 10);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_procrustes_interval(int value)
{
  this->params_.set("procrustes_interval", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_multiscale()
{
  return this->params_.get("multiscale", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_multiscale(bool value)
{
  this->params_.set("multiscale", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_multiscale_particles()
{
  return this->params_.get("multiscale_particles", 32);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_multiscale_particles(int value)
{
  this->params_.set("multiscale_particles", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::set_up_optimize(Optimize* optimize)
{
  int domains_per_shape = this->project_->get_number_of_domains_per_subject();
  bool normals_enabled = this->get_use_normals()[0];
  optimize->SetDomainsPerShape(domains_per_shape);
  optimize->SetNumberOfParticles(this->get_number_of_particles());
  optimize->SetInitialRelativeWeighting(this->get_initial_relative_weighting());
  optimize->SetRelativeWeighting(this->get_relative_weighting());
  optimize->SetStartingRegularization(this->get_starting_regularization());
  optimize->SetEndingRegularization(this->get_ending_regularization());
  optimize->SetIterationsPerSplit(this->get_iterations_per_split());
  optimize->SetOptimizationIterations(this->get_optimization_iterations());
  optimize->SetGeodesicsEnabled(this->get_use_geodesic_distance());
  optimize->SetGeodesicsCacheSizeMultiplier(this->get_geodesic_cache_multiplier());
  optimize->SetOutputDir(this->get_output_prefix());

  // TODO Remove this once Studio has controls for shared boundary
  optimize->SetSharedBoundaryEnabled(true);
  optimize->SetSharedBoundaryWeight(0.5);

  std::vector<bool> use_normals;
  std::vector<bool> use_xyz;
  std::vector<double> attr_scales;

  // xyz forced
  for (int i = 0; i < domains_per_shape; i++) {
    use_xyz.push_back(1);
    attr_scales.push_back(1);
    attr_scales.push_back(1);
    attr_scales.push_back(1);

    if (normals_enabled) { // not yet differentiating per domain
      use_normals.push_back(1);
      double normals_strength = this->get_normals_strength();
      attr_scales.push_back(normals_strength);
      attr_scales.push_back(normals_strength);
      attr_scales.push_back(normals_strength);
    }
    else {
      use_normals.push_back(0);
    }
  }

  optimize->SetUseNormals(use_normals);
  optimize->SetUseXYZ(use_xyz);
  optimize->SetUseMeshBasedAttributes(normals_enabled);
  optimize->SetAttributeScales(attr_scales);

  std::vector<int> attributes_per_domain;
  for (int i = 0; i < domains_per_shape; i++) {
    attributes_per_domain.push_back(0);
  }
  optimize->SetAttributesPerDomain(attributes_per_domain);

  int procrustes_interval = 0;
  if (this->get_use_procrustes()) {
    procrustes_interval = this->get_procrustes_interval();
  }
  optimize->SetProcrustesInterval(procrustes_interval);
  optimize->SetProcrustesScaling(this->get_use_procrustes_scaling());
  optimize->SetVerbosity(0);

  int multiscale_particles = 0;
  if (this->get_use_multiscale()) {
    multiscale_particles = this->get_multiscale_particles();
  }
  optimize->SetUseShapeStatisticsAfter(multiscale_particles);


  // should add the images last
  auto subjects = this->project_->get_subjects();

  if (subjects.empty()) {
    throw std::invalid_argument("No subjects to optimize");
  }

  // passing cutting plane constraints
  // planes dimensions [number_of_inputs, planes_per_input, normal/point]
  std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d> > > planes = optimize->GetSampler()->ComputeCuttingPlanes();

  std::vector<std::string> filenames;
  int count = 0;
  int domain_count = 0;
  for (auto s : subjects) {
    if (this->abort_load_) {
      return false;
    }
    auto files = s->get_groomed_filenames();
    if (files.empty()) {
      throw std::invalid_argument("No groomed inputs for optimization");
    }
    auto transforms = s->get_groomed_transforms();
    std::vector<std::string> local_particle_filenames;
    std::vector<std::string> world_particle_filenames;

    for (int i = 0; i < files.size(); i++) {

      auto filename = files[i];
      auto domain_type = s->get_domain_types()[i];
      filenames.push_back(filename);

      if (domain_type == DomainType::Mesh) {
        Mesh mesh = MeshUtils::threadSafeReadMesh(filename.c_str());

        if (count < planes.size()) {
          for (size_t i = 0; i < planes[count].size(); i++) {
            // Create vtk plane
            vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
            plane->SetNormal(planes[count][i].first[0], planes[count][i].first[1],
                             planes[count][i].first[2]);
            plane->SetOrigin(planes[count][i].second[0], planes[count][i].second[1],
                             planes[count][i].second[2]);

            mesh.clip(plane);
          }
        }
        auto poly_data = mesh.getVTKMesh();

        if (poly_data) {
          // TODO This is a HACK for detecting contours
          if(poly_data->GetCell(0)->GetNumberOfPoints() == 2) {
            optimize->AddContour(poly_data);
          } else {
            optimize->AddMesh(poly_data);
          }
        }
        else {
          throw std::invalid_argument("Error loading mesh: " + filename);
        }
      }
      else {
        Image image(filename);
        optimize->AddImage(image);
      }

      using TransformType = vnl_matrix_fixed<double, 4, 4>;
      TransformType prefix_transform;
      prefix_transform.set_identity();

      /*
      if (transforms.size() > 0) {
        int idx = 0;
        for (int r = 0; r < 4; r++) {
          for (int c = 0; c < 3; c++) {
            prefix_transform[r][c] = transforms[0][idx++];
          }
        }
      }*/

      if (i < transforms.size() && transforms[i].size() >= 12) {
        prefix_transform[0][3] = transforms[i][9];
        prefix_transform[1][3] = transforms[i][10];
        prefix_transform[2][3] = transforms[i][11];
      }

      optimize->GetSampler()->GetParticleSystem()->SetPrefixTransform(domain_count++,
                                                                      prefix_transform);

      auto name = StringUtils::getFileNameWithoutExtension(filename);

      auto prefix = this->get_output_prefix();
      local_particle_filenames.push_back(prefix + name + "_local.particles");
      world_particle_filenames.push_back(prefix + name + "_world.particles");
    }
    s->set_local_particle_filenames(local_particle_filenames);
    s->set_world_particle_filenames(world_particle_filenames);

    count++;
    if (this->load_callback_) {
      this->load_callback_(count);
    }
  }

  optimize->SetFilenames(StringUtils::getFileNamesFromPaths(filenames));
  optimize->SetOutputTransformFile("transform");

  return true;
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_abort_load(bool value)
{
  this->abort_load_ = value;
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_load_callback(const std::function<void(int)>& f)
{
  this->load_callback_ = f;
}

//---------------------------------------------------------------------------
std::string OptimizeParameters::get_optimize_output_prefix()
{
  return this->params_.get("optimize_output_prefix", "<project>_particles");
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_optimize_output_prefix(std::string prefix)
{
  this->params_.set("optimize_output_prefix", prefix);
}

//---------------------------------------------------------------------------
std::string OptimizeParameters::get_output_prefix()
{
  // if the project is not saved, use the path of the input filename
  auto filename = this->project_->get_filename();
  if (filename == "") {
    filename = ".";
  }

  auto base = StringUtils::getPath(filename);
  if (base == filename) {
    base = ".";
  }

  auto project_name = StringUtils::getFileNameWithoutExtension(this->project_->get_filename());

  if (project_name == "") {
    project_name = "new_project";
  }

  auto prefix = this->get_optimize_output_prefix();
  boost::replace_all(prefix, "<project>", project_name);

  auto path = base + "/" + prefix;
  boost::filesystem::create_directories(path);

  auto output = path + "/";
  return output;
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_geodesic_cache_multiplier()
{
  return this->params_.get("geodesic_cache_multiplier", 0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_geodesic_cache_multiplier(int value)
{
  this->params_.set("geodesic_cache_multiplier", value);

}

