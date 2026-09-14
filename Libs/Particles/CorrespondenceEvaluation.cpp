#include "CorrespondenceEvaluation.h"

#include <Image/Image.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshWarper.h>
#include <Particles/ParticleFile.h>
#include <Project/Project.h>
#include <Project/Subject.h>
#include <Utils/StringUtils.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkPolyData.h>

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

//! Surface area each vertex stands for: every cell's area shared evenly among its corners.  Cells are
//! fan-triangulated, so polygons other than triangles are handled too.
std::vector<double> vertex_areas(vtkPolyData* poly_data) {
  std::vector<double> areas(poly_data->GetNumberOfPoints(), 0.0);
  auto ids = vtkSmartPointer<vtkIdList>::New();
  for (vtkIdType cell = 0; cell < poly_data->GetNumberOfCells(); cell++) {
    poly_data->GetCellPoints(cell, ids);
    const vtkIdType corners = ids->GetNumberOfIds();
    if (corners < 3) {
      continue;
    }
    double origin[3];
    poly_data->GetPoint(ids->GetId(0), origin);
    double cell_area = 0.0;
    for (vtkIdType k = 1; k + 1 < corners; k++) {
      double a[3];
      double b[3];
      poly_data->GetPoint(ids->GetId(k), a);
      poly_data->GetPoint(ids->GetId(k + 1), b);
      double edge_a[3];
      double edge_b[3];
      vtkMath::Subtract(a, origin, edge_a);
      vtkMath::Subtract(b, origin, edge_b);
      double cross[3];
      vtkMath::Cross(edge_a, edge_b, cross);
      cell_area += 0.5 * vtkMath::Norm(cross);
    }
    for (vtkIdType k = 0; k < corners; k++) {
      areas[ids->GetId(k)] += cell_area / corners;
    }
  }
  return areas;
}

//! Mean, median, p99 and max of per-vertex values, each vertex weighted by the area it stands for, so
//! a region counts in proportion to its size however finely it happens to be meshed.
CorrespondenceDistanceStats weighted_stats(const std::vector<double>& values, std::vector<double> weights,
                                           double bbox_diag) {
  CorrespondenceDistanceStats stats;
  if (values.empty() || weights.size() != values.size()) {
    return stats;
  }

  double total = std::accumulate(weights.begin(), weights.end(), 0.0);
  if (!(total > 0.0)) {
    // nothing to weight by (no cells), so every vertex counts the same
    std::fill(weights.begin(), weights.end(), 1.0);
    total = static_cast<double>(weights.size());
  }

  std::vector<size_t> order(values.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&values](size_t a, size_t b) { return values[a] < values[b]; });

  // the smallest value with at least this fraction of the total area at or below it
  auto percentile = [&](double fraction) {
    const double target = fraction * total;
    double cumulative = 0.0;
    for (size_t i : order) {
      cumulative += weights[i];
      if (cumulative >= target) {
        return values[i];
      }
    }
    return values[order.back()];
  };

  double weighted_sum = 0.0;
  for (size_t i = 0; i < values.size(); i++) {
    weighted_sum += weights[i] * values[i];
  }
  stats.mean = weighted_sum / total;
  stats.median = percentile(0.5);
  stats.p99 = percentile(0.99);
  stats.max = values[order.back()];

  if (bbox_diag > 0.0) {
    stats.norm_mean = stats.mean / bbox_diag;
    stats.norm_median = stats.median / bbox_diag;
    stats.norm_p99 = stats.p99 / bbox_diag;
    stats.norm_max = stats.max / bbox_diag;
  }
  return stats;
}

}  // namespace

//---------------------------------------------------------------------------
CorrespondenceQualityStats CorrespondenceEvaluation::summarize(std::vector<double> values) {
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

//---------------------------------------------------------------------------
CorrespondenceQualityRow CorrespondenceEvaluation::evaluate_reconstruction(
    vtkSmartPointer<vtkPolyData> reconstructed, const Mesh& groomed, DistanceMethod method,
    vtkSmartPointer<vtkDataArray>* out_distance, vtkSmartPointer<vtkDataArray>* out_disagreement,
    vtkSmartPointer<vtkDataArray>* out_push) {
  CorrespondenceQualityRow row;
  if (!reconstructed || reconstructed->GetNumberOfPoints() == 0) {
    return row;
  }

  const Mesh::DistanceMethod distance_method =
      (method == DistanceMethod::PointToPoint) ? Mesh::DistanceMethod::PointToPoint : Mesh::DistanceMethod::PointToCell;

  // pull: each reconstructed vertex to the groomed surface, with the groomed cell (point-to-cell) or
  // vertex (point-to-point) it is nearest
  Mesh recon_mesh(reconstructed);
  auto pull_fields = recon_mesh.distance(groomed, distance_method);
  auto field = pull_fields[0];
  auto nearest = pull_fields[1];

  const int n = field->GetNumberOfTuples();
  if (n == 0) {
    return row;
  }

  std::vector<double> values(n);
  double sum = 0.0;
  double maxv = 0.0;
  for (int k = 0; k < n; ++k) {
    const double v = std::fabs(field->GetTuple1(k));
    values[k] = v;
    sum += v;
    if (v > maxv) maxv = v;
  }

  const size_t mid = values.size() / 2;
  std::nth_element(values.begin(), values.begin() + mid, values.end());
  const double median = values[mid];

  const size_t p99_idx = std::min(values.size() - 1, static_cast<size_t>(0.99 * values.size()));
  std::nth_element(values.begin(), values.begin() + p99_idx, values.end());

  row.mean_dist = sum / n;
  row.median_dist = median;
  row.p99_dist = values[p99_idx];
  row.max_dist = maxv;

  const auto bbox = groomed.boundingBox();
  row.bbox_diag = (bbox.max - bbox.min).GetNorm();
  if (row.bbox_diag > 0.0) {
    row.norm_mean = row.mean_dist / row.bbox_diag;
    row.norm_median = row.median_dist / row.bbox_diag;
    row.norm_p99 = row.p99_dist / row.bbox_diag;
    row.norm_max = row.max_dist / row.bbox_diag;
  }

  // push: each groomed vertex to the reconstructed surface
  auto push = groomed.distance(recon_mesh, distance_method)[0];
  push->SetName("push");

  // Disagreement lives on the groomed vertices: the push distance, raised wherever a reconstructed vertex
  // lands alongside with a larger pull distance.  Each pull distance is carried onto the corners of the
  // groomed cell that reconstructed vertex is nearest (or onto that vertex, point-to-point).  There is no
  // check that the two surfaces face the same way, because a fold in the reconstruction has reversed
  // normals and is exactly what the pull term is there to catch.
  auto groomed_poly_data = groomed.getVTKMesh();
  const vtkIdType num_groomed = groomed_poly_data->GetNumberOfPoints();
  std::vector<double> push_values(num_groomed);
  std::vector<double> disagreement_values(num_groomed);
  for (vtkIdType v = 0; v < num_groomed; v++) {
    push_values[v] = std::fabs(push->GetTuple1(v));
    disagreement_values[v] = push_values[v];
  }

  auto corners = vtkSmartPointer<vtkIdList>::New();
  for (int k = 0; k < n; ++k) {
    const double pull = std::fabs(field->GetTuple1(k));
    const auto target = static_cast<vtkIdType>(nearest->GetTuple1(k));
    if (distance_method == Mesh::DistanceMethod::PointToPoint) {
      if (target >= 0 && target < num_groomed) {
        disagreement_values[target] = std::max(disagreement_values[target], pull);
      }
      continue;
    }
    if (target < 0 || target >= groomed_poly_data->GetNumberOfCells()) {
      continue;
    }
    groomed_poly_data->GetCellPoints(target, corners);
    for (vtkIdType c = 0; c < corners->GetNumberOfIds(); c++) {
      const vtkIdType corner = corners->GetId(c);
      disagreement_values[corner] = std::max(disagreement_values[corner], pull);
    }
  }

  const auto areas = vertex_areas(groomed_poly_data);
  row.push = weighted_stats(push_values, areas, row.bbox_diag);
  row.disagreement = weighted_stats(disagreement_values, areas, row.bbox_diag);

  if (out_distance) {
    field->SetName("distance");
    *out_distance = field;
  }

  if (out_disagreement) {
    auto disagreement = vtkSmartPointer<vtkDoubleArray>::New();
    disagreement->SetName("disagreement");
    disagreement->SetNumberOfComponents(1);
    disagreement->SetNumberOfTuples(num_groomed);
    for (vtkIdType v = 0; v < num_groomed; v++) {
      disagreement->SetValue(v, disagreement_values[v]);
    }
    *out_disagreement = disagreement;
  }

  if (out_push) {
    *out_push = push;
  }

  return row;
}

//---------------------------------------------------------------------------
void CorrespondenceEvaluation::compute_aggregates(CorrespondenceQualityReport& report) {
  std::vector<double> means;
  std::vector<double> norm_means;
  std::vector<double> push_means;
  std::vector<double> push_norm_means;
  std::vector<double> disagreement_means;
  std::vector<double> disagreement_norm_means;
  int num_template_rows = 0;
  for (const auto& r : report.rows) {
    if (r.is_template) {
      num_template_rows++;
      continue;
    }
    means.push_back(r.mean_dist);
    norm_means.push_back(r.norm_mean);
    push_means.push_back(r.push.mean);
    push_norm_means.push_back(r.push.norm_mean);
    disagreement_means.push_back(r.disagreement.mean);
    disagreement_norm_means.push_back(r.disagreement.norm_mean);
  }
  report.num_template_rows = num_template_rows;
  report.num_evaluated = static_cast<int>(means.size());
  report.agg_raw = summarize(means);
  report.agg_norm = summarize(norm_means);
  report.agg_push_raw = summarize(push_means);
  report.agg_push_norm = summarize(push_norm_means);
  report.agg_disagreement_raw = summarize(disagreement_means);
  report.agg_disagreement_norm = summarize(disagreement_norm_means);
}

//---------------------------------------------------------------------------
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

      Mesh groomed_mesh = load_groomed_as_mesh(groomed_per_subject_domain[i][domain]);
      vtkSmartPointer<vtkDataArray> field;
      vtkSmartPointer<vtkDataArray> disagreement;
      vtkSmartPointer<vtkDataArray> push;
      CorrespondenceQualityRow row =
          evaluate_reconstruction(reconstructed, groomed_mesh, method, &field, &disagreement, &push);
      if (!field) continue;

      row.subject = name_per_subject[i];
      row.domain = domain;
      row.is_template = (i == template_idx);
      report.rows.push_back(row);

      if (!meshes_dir.empty()) {
        const std::string stem = name_per_subject[i] + "_domain" + std::to_string(domain);
        const std::string template_suffix = row.is_template ? "_TEMPLATE" : "";

        Mesh recon_mesh(reconstructed);
        recon_mesh.setField("distance", field, Mesh::FieldType::Point);
        recon_mesh.write((meshes_dir / (stem + "_reconstructed" + template_suffix + ".vtk")).string());

        // the groomed surface too, carrying the fields only it can show: a gap in the reconstruction has
        // no reconstructed surface to color
        groomed_mesh.setField("disagreement", disagreement, Mesh::FieldType::Point);
        groomed_mesh.setField("push", push, Mesh::FieldType::Point);
        groomed_mesh.write((meshes_dir / (stem + "_groomed_disagreement" + template_suffix + ".vtk")).string());
      }
    }
  }

  if (report.rows.empty()) {
    throw std::runtime_error("no subjects evaluated");
  }

  compute_aggregates(report);
  return report;
}

}  // namespace shapeworks
