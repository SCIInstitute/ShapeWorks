#include "Analyze.h"

#include <Logging.h>
#include <StringUtils.h>

#include <MeshWarper.h>

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

namespace shapeworks {


//---------------------------------------------------------------------------
static json get_eigen_vectors(ParticleShapeStatistics *stats) {
  auto values = stats->Eigenvectors();

  std::vector<double> vals;
  for (size_t i = values.cols() - 1, ii = 0; i > 0; i--, ii++) {
    auto col = values.col(i);
    for (int j = 0; j < col.size(); j++) {
      vals.push_back(col[j]);
    }
  }

  return vals;
}

//---------------------------------------------------------------------------
static json create_charts(ParticleShapeStatistics *stats) {
  std::vector<int> x(stats->get_num_modes());
  for (int i = 0; i < x.size(); i++) {
    x[i] = i + 1;
  }

  json compactness;
  compactness["title"] = "Compactness";
  compactness["type"] = "line";
  compactness["x_label"] = "Number of Modes";
  compactness["y_label"] = "Explained Variance";
  compactness["x"] = x;
  compactness["y"] = stats->get_compactness();

  json generalization;
  generalization["title"] = "Generalization";
  generalization["type"] = "line";
  generalization["x_label"] = "Number of Modes";
  generalization["y_label"] = "Generalization";
  generalization["x"] = x;
  generalization["y"] = stats->get_generalization();

  json specificity;
  specificity["title"] = "Specificity";
  specificity["type"] = "line";
  specificity["x_label"] = "Number of Modes";
  specificity["y_label"] = "Specificity";
  specificity["x"] = x;
  specificity["y"] = stats->get_specificity();

  json charts = {compactness, generalization, specificity};
  return charts;
}

//---------------------------------------------------------------------------
Analyze::Analyze(ProjectHandle project) : project_(project), mesh_manager_(new MeshManager()) {
  mesh_manager_->set_cache_enabled(false);
}

//---------------------------------------------------------------------------
void Analyze::run_offline_analysis(std::string outfile) {
  SW_LOG("ShapeWorks Offline Analysis");
  if (!project_->get_particles_present()) {
    throw std::runtime_error("Project has not been optimized, please run optimize first");
  }

  auto base = boost::filesystem::path(outfile).parent_path();

  // load data from the project
  update_shapes();

  // compute stats
  compute_stats();

  initialize_mesh_warper();

  json j;

  double range = 2.0; // TODO: make this configurable
  double steps = 11; // TODO: make this configurable
  int half_steps = (steps / 2.0);
  double increment = range / half_steps;

  std::vector<double> eigen_vals;
  for (int i = stats_.Eigenvalues().size() - 1; i > 0; i--) {
    eigen_vals.push_back(stats_.Eigenvalues()[i]);
  }
  double sum = std::accumulate(eigen_vals.begin(), eigen_vals.end(), 0.0);

  auto domain_names = project_->get_domain_names();
  int num_domains = domain_names.size();
  int num_modes = get_num_modes();
  j["number_of_modes"] = num_modes;
  SW_LOG("number of modes: {}", num_modes);
  j["domains"] = domain_names;

  // output mean shape
  auto mean_shape = get_mean_shape();
  auto meshes = mean_shape->get_reconstructed_meshes(true);
  std::vector<std::string> mean_meshes;
  std::vector<std::string> mean_particles;
  for (int d = 0; d < num_domains; d++) {
    std::string domain_id = std::to_string(d);
    auto mesh = meshes.meshes()[d];
    std::string filename = "mean_shape_" + domain_id + ".vtk";
    Mesh(mesh->get_poly_data()).write(filename);
    mean_meshes.push_back(filename);

    filename = "mean_shape_" + domain_id + ".pts";
    auto local_particles = mean_shape->get_particles().get_local_particles(d);
    Particles::save_particles_file(filename, local_particles);
    mean_particles.push_back(filename);
  }
  json mean_meshes_item;
  mean_meshes_item["meshes"] = mean_meshes;
  mean_meshes_item["particle_files"] = mean_particles;
  j["mean"] = mean_meshes_item;

  // export modes
  std::vector<json> modes;
  for (int mode = 0; mode < num_modes; mode++) {
    unsigned int m = stats_.Eigenvectors().cols() - (mode + 1);
    json jmode;
    jmode["mode"] = mode + 1;
    double eigen_value = eigen_vals[mode];
    jmode["eigen_value"] = eigen_value;
    SW_LOG("eigen value [{}]: {}", mode, eigen_value);

    double cumulation = 0;
    for (size_t i = 0; i <= mode; ++i) {
      cumulation += eigen_vals[i];
    }

    double explained_variance = eigen_vals[mode] / sum * 100;
    double cumulative_explained_variance = cumulation / sum * 100;
    jmode["explained_variance"] = explained_variance;
    jmode["cumulative_explained_variance"] = cumulative_explained_variance;
    SW_LOG("explained_variance [{}]: {:.2f}", mode, explained_variance);
    SW_LOG("cumulative_explained_variance [{}]: {:.2f}", mode, cumulative_explained_variance);

    double lambda = sqrt(stats_.Eigenvalues()[m]);

    std::vector<json> jmodes;
    for (int i = 1; i <= half_steps; i++) {
      double pca_value = increment * i;
      std::string pca_string = QString::number(pca_value, 'g', 2).toStdString();

      auto process_value = [&](double pca_value, std::string prefix) {
        auto shape = get_mode_shape(mode, pca_value);
        auto mode_meshes = shape->get_reconstructed_meshes(true);

        json item;
        item["pca_value"] = pca_value;
        item["lambda"] = lambda * pca_value;

        std::vector<std::string> items;
        std::vector<std::string> worlds;
        for (int d = 0; d < num_domains; d++) {
          std::string domain_id = std::to_string(d);
          auto mesh = mode_meshes.meshes()[d];
          std::string name = "pca_mode_" + std::to_string(mode + 1) + "_domain_" + std::to_string(d) + "_" + prefix +
              "_" + pca_string;
          std::string vtk_filename = name + ".vtk";
          items.push_back(vtk_filename);

          auto filename = base / boost::filesystem::path(vtk_filename);
          Mesh(mesh->get_poly_data()).write(filename.string());

          auto particle_filename = name + ".pts";
          worlds.push_back(particle_filename);
          filename = base / boost::filesystem::path(particle_filename);
          Particles::save_particles_file(filename.string(), shape->get_particles().get_world_particles(d));

        }
        item["meshes"] = items;
        item["particles"] = worlds;
        jmodes.push_back(item);
      };

      process_value(-pca_value, "minus");
      process_value(pca_value, "plus");
    }
    jmode["pca_values"] = jmodes;
    modes.push_back(jmode);
  }

  j["eigen_vectors"] = get_eigen_vectors(&stats_);
  j["eigen_values"] = stats_.Eigenvalues();
  j["modes"] = modes;
  j["charts"] = create_charts(&stats_);

  std::vector<json> shapes;
  for (int i=0;i<shapes_.size();i++) {
    auto &s = shapes_[i];
    auto meshes = s->get_reconstructed_meshes(true);
    std::vector<std::string> filenames;
    for (int d = 0; d < num_domains; d++) {
      auto mesh = meshes.meshes()[d];
      std::string vtk_filename = "sample_"+std::to_string(i)+"_"+std::to_string(d)+".vtk";
      auto filename = base / boost::filesystem::path(vtk_filename);
      Mesh(mesh->get_poly_data()).write(filename.string());
      filenames.push_back(vtk_filename);
    }
    shapes.push_back(filenames);
  }
  j["reconstructed_samples"] = shapes;

  std::ofstream file(outfile);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + outfile + " for writing");
  }
  file << j.dump(4);
}

//---------------------------------------------------------------------------
ShapeList Analyze::get_shapes() { return shapes_; }

//---------------------------------------------------------------------------
int Analyze::get_num_modes() { return shapes_.size() - 1; }

//---------------------------------------------------------------------------
Particles Analyze::get_mean_shape_points() {
  if (!compute_stats()) {
    return Particles();
  }

  return convert_from_combined(stats_.Mean());
}

//---------------------------------------------------------------------------
ShapeHandle Analyze::get_mean_shape() {
  auto shape_points = get_mean_shape_points();
  ShapeHandle shape = create_shape_from_points(shape_points);

  return shape;
}

//---------------------------------------------------------------------------
Particles Analyze::get_shape_points(int mode, double value) {
  if (!compute_stats() || stats_.Eigenvectors().size() <= 1) {
    return Particles();
  }
  if (mode + 2 > stats_.Eigenvalues().size()) {
    mode = stats_.Eigenvalues().size() - 2;
  }

  unsigned int m = stats_.Eigenvectors().cols() - (mode + 1);

  Eigen::VectorXd e = stats_.Eigenvectors().col(m);

  double lambda = sqrt(stats_.Eigenvalues()[m]);

  std::vector<double> vals;
  for (int i = stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(stats_.Eigenvalues()[i]);
  }
  double sum = std::accumulate(vals.begin(), vals.end(), 0.0);
  double cumulation = 0;
  for (size_t i = 0; i < mode + 1; ++i) {
    cumulation += vals[i];
  }

  auto temp_shape = stats_.Mean() + (e * (value * lambda));

  return convert_from_combined(temp_shape);
}

//---------------------------------------------------------------------------
ShapeHandle Analyze::get_mode_shape(int mode, double value) {
  return create_shape_from_points(get_shape_points(mode, value));
}

//---------------------------------------------------------------------------
ShapeHandle Analyze::create_shape_from_points(Particles points) {
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(mesh_manager_);
  shape->set_particles(points);
  shape->get_reconstructed_meshes();
  /// shape->set_reconstruction_transforms(reconstruction_transforms_);
  return shape;
}

//---------------------------------------------------------------------------
bool Analyze::update_shapes() {
  int num_subjects = project_->get_number_of_subjects();

  auto subjects = project_->get_subjects();

  SW_LOG("number of subjects: {}", num_subjects);

  for (int i = 0; i < num_subjects; i++) {
    auto shape = std::make_shared<Shape>();
    shape->set_mesh_manager(mesh_manager_);
    shape->set_subject(subjects[i]);

    auto locals = subjects[i]->get_local_particle_filenames();
    auto worlds = subjects[i]->get_world_particle_filenames();
    auto landmark_files = subjects[i]->get_landmarks_filenames();
    auto constraints_files = subjects[i]->get_constraints_filenames();

    if (!shape->import_local_point_files(locals)) {
      return false;
    }
    if (!shape->import_global_point_files(worlds)) {
      return false;
    }
    if (!shape->import_landmarks_files(landmark_files)) {
      return false;
    }
    if (!shape->import_constraints(constraints_files)) {
      return false;
    }

    // add to landmark definitions if landmark files contain more landmarks than the project definitions
    auto landmarks = shape->landmarks();
    for (int row = 0; row < landmarks.rows(); row++) {
      int domain_id = landmarks(row, 0);
      int point_id = landmarks(row, 1);
      auto landmark_definitions = project_->get_landmarks(domain_id);
      while (point_id >= landmark_definitions.size()) {
        project_->new_landmark(domain_id);
        landmark_definitions = project_->get_landmarks(domain_id);
      }
    }
    shapes_.push_back(shape);
  }

  SW_DEBUG("Successfully loaded shapes");

  return true;
}

//---------------------------------------------------------------------------
bool Analyze::compute_stats() {
  if (stats_ready_) {
    return true;
  }

  SW_LOG("Computing stats...");

  std::vector<Eigen::VectorXd> points;
  std::vector<int> group_ids;

  std::string group_set = "need to add groups1";
  std::string left_group = "need to add groups2";
  std::string right_group = "need to add groups3";

  /// TODO: implement groups
  /// bool groups_enabled = groups_active();
  bool groups_enabled = false;

  group1_list_.clear();
  group2_list_.clear();

  for (auto& shape : shapes_) {
    if (groups_enabled) {
      auto value = shape->get_subject()->get_group_value(group_set);
      if (value == left_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(1);
        group1_list_.push_back(shape);
      } else if (value == right_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(2);
        group2_list_.push_back(shape);
      } else {
        // we don't include it
      }
    } else {
      points.push_back(shape->get_global_correspondence_points());
      group_ids.push_back(1);
    }
  }

  if (points.empty()) {
    return false;
  }

  // consistency check
  size_t point_size = points[0].size();
  for (auto&& p : points) {
    if (p.size() != point_size) {
      SW_ERROR("Inconsistency in data, particle files must contain the same number of points");
      return false;
    }
  }

  stats_.ImportPoints(points, group_ids);
  stats_.ComputeModes();

  stats_ready_ = true;
  SW_LOG("Computed stats successfully");
  return true;
}

//---------------------------------------------------------------------------
Particles Analyze::convert_from_combined(const Eigen::VectorXd& points) {
  Particles particles;
  if (shapes_.empty()) {
    return particles;
  }
  auto base = shapes_[0]->get_particles();

  auto worlds = base.get_world_particles();
  int idx = 0;
  for (int d = 0; d < worlds.size(); d++) {
    Eigen::VectorXd new_world(worlds[d].size());
    for (int i = 0; i < worlds[d].size(); i++) {
      new_world[i] = points[idx++];
    }
    particles.set_world_particles(d, new_world);
    particles.set_local_particles(d, new_world);
  }

  return particles;
}

//---------------------------------------------------------------------------
void Analyze::initialize_mesh_warper() {
  int median = stats_.ComputeMedianShape(-32);  //-32 = both groups

  if (median < 0 || median >= get_shapes().size()) {
    SW_ERROR("Unable to set reference mesh, stats returned invalid median index");
    return;
  }
  std::shared_ptr<Shape> median_shape = get_shapes()[median];

  auto mesh_group = median_shape->get_groomed_meshes(true);

  if (!mesh_group.valid()) {
    SW_ERROR("Unable to set reference mesh, groomed mesh is unavailable");
    return;
  }
  auto meshes = mesh_group.meshes();
  for (int i = 0; i < mesh_group.meshes().size(); i++) {
    Eigen::VectorXd particles = median_shape->get_particles().get_local_particles(i);
    Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>((double*)particles.data(), particles.size());
    points.resize(3, points.size() / 3);
    points.transposeInPlace();

    auto poly_data = meshes[i]->get_poly_data();
    Mesh mesh(poly_data);
    median_shape->get_constraints(i).clipMesh(mesh);

    mesh_manager_->get_mesh_warper(i)->set_reference_mesh(mesh.getVTKMesh(), points);
  }

}

}  // namespace shapeworks
