#include "CorrespondenceQualityJob.h"

#include <Analysis/AnalysisTool.h>
#include <Data/Session.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Project/Project.h>
#include <StudioMesh.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <cmath>

namespace shapeworks {

//---------------------------------------------------------------------------
CorrespondenceQualityJob::CorrespondenceQualityJob(QSharedPointer<Session> session,
                                                   CorrespondenceEvaluation::DistanceMethod method)
    : session_(session), method_(method) {}

//---------------------------------------------------------------------------
void CorrespondenceQualityJob::run() {
  SW_DEBUG("Running correspondence quality job");
  Q_EMIT progress(0);

  report_ = CorrespondenceQualityReport();
  row_shape_indices_.clear();
  particle_values_.clear();
  distance_fields_.clear();

  auto shapes = session_->get_shapes();
  auto non_excluded = session_->get_non_excluded_shapes();

  // The template is whichever shape Studio is warping from, so its reconstruction is
  // near-identity.  Mark it here so it can be excluded from the aggregates.
  int template_shape_index = -1;
  auto params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  int template_index = params.get(AnalysisTool::MESH_WARP_TEMPLATE_INDEX, -1);
  if (template_index >= 0 && template_index < static_cast<int>(non_excluded.size())) {
    auto template_shape = non_excluded[template_index];
    for (int i = 0; i < static_cast<int>(shapes.size()); i++) {
      if (shapes[i] == template_shape) {
        template_shape_index = i;
        report_.template_subject = shapes[i]->get_display_name();
        break;
      }
    }
  }

  const float total = std::max<float>(1, non_excluded.size());
  float count = 0;

  for (int s = 0; s < static_cast<int>(shapes.size()); s++) {
    auto& shape = shapes[s];
    if (shape->is_excluded()) {
      continue;
    }

    auto reconstructed = shape->get_reconstructed_meshes(true);
    auto groomed = shape->get_groomed_meshes(true);

    if (!reconstructed.valid() || !groomed.valid()) {
      SW_LOG("Correspondence quality: skipping '{}', reconstructed or groomed mesh unavailable",
             shape->get_display_name());
      count++;
      Q_EMIT progress(count / total);
      continue;
    }

    const int num_domains =
        std::min(reconstructed.meshes().size(), groomed.meshes().size());

    // the distance sampled at each particle, all domains concatenated, which is the order
    // Shape stores point features in
    std::vector<double> per_particle;

    for (int d = 0; d < num_domains; d++) {
      auto reconstructed_poly_data = reconstructed.meshes()[d]->get_poly_data();
      auto groomed_poly_data = groomed.meshes()[d]->get_poly_data();
      if (!reconstructed_poly_data || !groomed_poly_data) {
        continue;
      }

      Mesh groomed_mesh(groomed_poly_data);
      vtkSmartPointer<vtkDataArray> distance;
      auto row = CorrespondenceEvaluation::evaluate_reconstruction(reconstructed_poly_data, groomed_mesh, method_,
                                                                   &distance);
      if (!distance) {
        continue;
      }

      row.subject = shape->get_display_name();
      row.domain = d;
      row.is_template = (s == template_shape_index);
      report_.rows.push_back(row);
      row_shape_indices_.push_back(s);

      // leave the per-vertex field on the reconstructed mesh so it can be shown as a surface scalar
      distance->SetName(FEATURE_NAME);
      reconstructed_poly_data->GetPointData()->AddArray(distance);
      distance_fields_[s].push_back(distance);

      // Color each particle by the error around it rather than at it.  The warp inserts the
      // particles into the mesh as vertices and maps them onto this shape's particles, which lie on
      // its surface, so the distance at a particle is zero by construction and sampling there would
      // give every glyph the same value.  All the signal is in the gaps between particles, so
      // assign every vertex to its nearest particle and average over that neighbourhood.
      auto particles = shape->get_particles().get_local_points(d);

      auto particle_points = vtkSmartPointer<vtkPoints>::New();
      for (auto& particle : particles) {
        particle_points->InsertNextPoint(particle[0], particle[1], particle[2]);
      }
      auto particle_poly_data = vtkSmartPointer<vtkPolyData>::New();
      particle_poly_data->SetPoints(particle_points);

      auto locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
      locator->SetDataSet(particle_poly_data);
      locator->BuildLocator();

      std::vector<double> sums(particles.size(), 0.0);
      std::vector<int> counts(particles.size(), 0);
      for (vtkIdType v = 0; v < reconstructed_poly_data->GetNumberOfPoints(); v++) {
        double vertex[3];
        reconstructed_poly_data->GetPoint(v, vertex);
        vtkIdType id = locator->FindClosestPoint(vertex);
        if (id >= 0 && id < static_cast<vtkIdType>(sums.size())) {
          sums[id] += std::fabs(distance->GetTuple1(v));
          counts[id]++;
        }
      }
      for (size_t i = 0; i < particles.size(); i++) {
        per_particle.push_back(counts[i] > 0 ? sums[i] / counts[i] : 0.0);
      }
    }

    if (!per_particle.empty()) {
      Eigen::VectorXd values(per_particle.size());
      for (int i = 0; i < static_cast<int>(per_particle.size()); i++) {
        values(i) = per_particle[i];
      }
      particle_values_[s] = values;
    }

    count++;
    Q_EMIT progress(count / total);
    if (is_aborted()) {
      return;
    }
  }

  CorrespondenceEvaluation::compute_aggregates(report_);

  if (report_.rows.empty()) {
    SW_ERROR("Correspondence quality: no samples could be evaluated");
    set_failed();
  }
}

}  // namespace shapeworks
