#include "CorrespondenceEvaluation.h"

#include <Image/Image.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshWarper.h>
#include <Particles/ParticleFile.h>
#include <Project/Project.h>
#include <Project/Subject.h>
#include <Utils/StringUtils.h>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace shapeworks {

namespace {

Mesh load_groomed_as_mesh(const std::string& path) {
  const std::string ext = StringUtils::getLowerExtension(path);
  if (ext == ".nrrd" || ext == ".mha" || ext == ".mhd" || ext == ".nii" || ext == ".gz" || ext == ".tif" ||
      ext == ".tiff") {
    Image img(path);
    return img.toMesh(0.0);
  }
  return Mesh(path);
}

Eigen::MatrixXd load_particles_matrix(const std::string& filename) {
  Eigen::VectorXd v = particles::read_particles(filename);
  const int n = static_cast<int>(v.size() / 3);
  Eigen::MatrixXd m(n, 3);
  for (int i = 0; i < n; ++i) {
    m(i, 0) = v(3 * i + 0);
    m(i, 1) = v(3 * i + 1);
    m(i, 2) = v(3 * i + 2);
  }
  return m;
}

CorrespondenceQualityStats summarize(std::vector<double> values) {
  CorrespondenceQualityStats s;
  if (values.empty()) return s;
  std::sort(values.begin(), values.end());
  s.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
  s.max = values.back();
  s.median = values[values.size() / 2];
  const size_t p95_idx = std::min(values.size() - 1, static_cast<size_t>(0.95 * values.size()));
  s.p95 = values[p95_idx];
  return s;
}

}  // namespace

CorrespondenceQualityReport CorrespondenceEvaluation::evaluate(ProjectHandle project, DistanceMethod method,
                                                               const std::string& output_meshes_dir) {
  if (!project) {
    throw std::runtime_error("null project");
  }

  auto subjects = project->get_non_excluded_subjects();
  if (subjects.empty()) {
    throw std::runtime_error("no (non-excluded) subjects in project");
  }
  const int num_domains = project->get_number_of_domains_per_subject();
  if (num_domains <= 0) {
    throw std::runtime_error("project has no domains");
  }

  const Mesh::DistanceMethod distance_method =
      (method == DistanceMethod::PointToPoint) ? Mesh::DistanceMethod::PointToPoint : Mesh::DistanceMethod::PointToCell;

  // Pass 1: load particles + groomed paths per (subject, domain). Only keep subjects
  // with complete data across all domains, so the L1-medoid is computed over a
  // consistent cohort and the same global template applies to every domain.
  std::vector<std::string> name_per_subject;
  std::vector<std::vector<Eigen::MatrixXd>> particles_per_subject_domain;
  std::vector<std::vector<std::string>> groomed_per_subject_domain;

  for (auto& subj : subjects) {
    auto particle_files = subj->get_local_particle_filenames();
    auto groomed_files = subj->get_groomed_filenames();

    bool complete = true;
    std::vector<Eigen::MatrixXd> subj_particles;
    std::vector<std::string> subj_groomed;
    subj_particles.reserve(num_domains);
    subj_groomed.reserve(num_domains);

    for (int d = 0; d < num_domains; ++d) {
      if (d >= static_cast<int>(particle_files.size()) || particle_files[d].empty() ||
          d >= static_cast<int>(groomed_files.size()) || groomed_files[d].empty()) {
        SW_LOG("Skipping subject '{}': missing particles or groomed for domain {}", subj->get_display_name(), d);
        complete = false;
        break;
      }
      try {
        subj_particles.push_back(load_particles_matrix(particle_files[d]));
      } catch (const std::exception& e) {
        SW_LOG("Skipping subject '{}' domain {}: particle load failed: {}", subj->get_display_name(), d, e.what());
        complete = false;
        break;
      }
      subj_groomed.push_back(groomed_files[d]);
    }

    if (!complete) continue;
    name_per_subject.push_back(subj->get_display_name());
    particles_per_subject_domain.push_back(std::move(subj_particles));
    groomed_per_subject_domain.push_back(std::move(subj_groomed));
  }

  const int num_subjects = static_cast<int>(name_per_subject.size());
  if (num_subjects < 2) {
    throw std::runtime_error("need at least 2 subjects with complete data across all domains (have " +
                             std::to_string(num_subjects) + ")");
  }

  // Verify particle counts match across subjects per domain.
  std::vector<int> num_particles_per_domain(num_domains);
  for (int d = 0; d < num_domains; ++d) {
    num_particles_per_domain[d] = static_cast<int>(particles_per_subject_domain[0][d].rows());
    for (int s = 1; s < num_subjects; ++s) {
      if (particles_per_subject_domain[s][d].rows() != num_particles_per_domain[d]) {
        throw std::runtime_error("domain " + std::to_string(d) + ": subject '" + name_per_subject[s] + "' has " +
                                 std::to_string(particles_per_subject_domain[s][d].rows()) + " particles, expected " +
                                 std::to_string(num_particles_per_domain[d]));
      }
    }
  }

  // L1-medoid template selection over concatenated per-domain local particles
  // (matches ParticleShapeStatistics::compute_median_shape).
  int template_idx = 0;
  double best_l1_sum = std::numeric_limits<double>::infinity();
  for (int i = 0; i < num_subjects; ++i) {
    double sum_l1 = 0.0;
    for (int j = 0; j < num_subjects; ++j) {
      if (i == j) continue;
      double pair_l1 = 0.0;
      for (int d = 0; d < num_domains; ++d) {
        pair_l1 += (particles_per_subject_domain[i][d] - particles_per_subject_domain[j][d]).cwiseAbs().sum();
      }
      sum_l1 += pair_l1;
    }
    if (sum_l1 < best_l1_sum) {
      best_l1_sum = sum_l1;
      template_idx = i;
    }
  }
  SW_LOG("Template subject (L1-medoid): '{}' (sum of L1 distances to others = {:.4f})", name_per_subject[template_idx],
         best_l1_sum);

  // Resolve output meshes dir.
  boost::filesystem::path meshes_dir;
  if (!output_meshes_dir.empty()) {
    meshes_dir = boost::filesystem::path(output_meshes_dir);
    boost::filesystem::create_directories(meshes_dir);
    SW_LOG("Writing reconstructed meshes to: {}", meshes_dir.string());
  }

  CorrespondenceQualityReport report;
  report.template_subject = name_per_subject[template_idx];
  std::vector<double> all_means;       // pooled raw mean distances (template excluded)
  std::vector<double> all_norm_means;  // pooled bbox-normalized values (template excluded)

  // Pass 2: per-domain warp + distance using the single global template.
  for (int domain = 0; domain < num_domains; ++domain) {
    SW_LOG("=== Domain {} ===", domain);

    Mesh template_mesh = load_groomed_as_mesh(groomed_per_subject_domain[template_idx][domain]);
    MeshWarper warper;
    warper.set_warp_method(WarpMethod::Biharmonic);
    warper.set_reference_mesh(template_mesh.getVTKMesh(), particles_per_subject_domain[template_idx][domain]);
    if (!warper.generate_warp()) {
      throw std::runtime_error("domain " + std::to_string(domain) + ": failed to generate warp from template '" +
                               name_per_subject[template_idx] + "'");
    }

    for (int i = 0; i < num_subjects; ++i) {
      vtkSmartPointer<vtkPolyData> reconstructed = warper.build_mesh(particles_per_subject_domain[i][domain]);
      if (!reconstructed) {
        SW_LOG("Domain {}: subject '{}' reconstruction returned null, skipping", domain, name_per_subject[i]);
        continue;
      }

      Mesh recon_mesh(reconstructed);
      Mesh groomed_mesh = load_groomed_as_mesh(groomed_per_subject_domain[i][domain]);
      auto field = recon_mesh.distance(groomed_mesh, distance_method)[0];

      const int n = field->GetNumberOfTuples();
      if (n == 0) continue;
      double sum = 0.0;
      double maxv = 0.0;
      for (int k = 0; k < n; ++k) {
        const double v = std::fabs(field->GetTuple1(k));
        sum += v;
        if (v > maxv) maxv = v;
      }
      const double mean_d = sum / n;

      const auto bbox = groomed_mesh.boundingBox();
      const double bbox_diag = (bbox.max - bbox.min).GetNorm();
      const double norm_mean = (bbox_diag > 0.0) ? mean_d / bbox_diag : 0.0;
      const double norm_max = (bbox_diag > 0.0) ? maxv / bbox_diag : 0.0;

      CorrespondenceQualityRow row;
      row.subject = name_per_subject[i];
      row.domain = domain;
      row.mean_dist = mean_d;
      row.max_dist = maxv;
      row.bbox_diag = bbox_diag;
      row.norm_mean = norm_mean;
      row.norm_max = norm_max;
      row.is_template = (i == template_idx);
      report.rows.push_back(row);

      if (!row.is_template) {
        all_means.push_back(mean_d);
        all_norm_means.push_back(norm_mean);
      }

      if (!meshes_dir.empty()) {
        field->SetName("distance");
        recon_mesh.setField("distance", field, Mesh::FieldType::Point);
        std::string fname = name_per_subject[i] + "_domain" + std::to_string(domain) + "_reconstructed.vtk";
        if (row.is_template) {
          fname = name_per_subject[i] + "_domain" + std::to_string(domain) + "_reconstructed_TEMPLATE.vtk";
        }
        boost::filesystem::path out_mesh = meshes_dir / fname;
        recon_mesh.write(out_mesh.string());
      }
    }
  }

  if (report.rows.empty()) {
    throw std::runtime_error("no subjects evaluated");
  }

  report.num_template_rows = static_cast<int>(report.rows.size() - all_means.size());
  report.num_evaluated = static_cast<int>(all_means.size());
  report.agg_raw = summarize(all_means);
  report.agg_norm = summarize(all_norm_means);
  return report;
}

}  // namespace shapeworks
