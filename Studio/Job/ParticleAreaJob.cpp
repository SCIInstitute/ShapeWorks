
#include "ParticleAreaJob.h"

#include <Analyze/ParticleArea.h>
#include <Data/Session.h>
#include <Logging.h>
#include <Utils/EigenUtils.h>
#include <vtkLookupTable.h>

#include "ParticleArea.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleAreaJob::ParticleAreaJob(QSharedPointer<Session> session) { session_ = session; }

//---------------------------------------------------------------------------
void ParticleAreaJob::run() {
  SW_DEBUG("Running particle area job");
  Q_EMIT progress(0);

  int num_domains = session_->get_domains_per_shape();

  int num_particles = session_->get_num_particles();

  float total = (num_domains * session_->get_shapes().size()) + 1;
  float count = 0;

  auto lut = session_->get_glyph_lut();
  auto shapes = session_->get_shapes();

  // matrix num_particles by num_shapes
  Eigen::MatrixXd all_areas(num_particles, shapes.size());
  all_areas.setZero();

  // for each shape
  for (int s = 0; s < shapes.size(); s++) {
    auto& shape = shapes[s];

    auto meshes = shape->get_meshes(session_->get_display_mode(), true);

    if (meshes.valid() && lut) {
      // for each domain
      for (size_t i = 0; i < meshes.meshes().size(); i++) {
        auto points = shape->get_particles().get_local_points(i);
        auto poly_data = meshes.meshes()[i]->get_poly_data();
        if (!poly_data) {
          continue;
        }
        ParticleArea::assign_vertex_particles(poly_data, points);
        auto colors = ParticleArea::colors_from_lut(lut);
        ParticleArea::assign_vertex_colors(poly_data, colors);

        // auto areas = ParticleArea::compute_particle_areas(poly_data, points);
        auto areas = ParticleArea::compute_particle_triangle_areas(poly_data, points);

        // assign to matrix for shape s
        all_areas.col(s) = areas;

        count++;
        Q_EMIT progress(count / total);
        if (is_aborted()) {
          return;
        }
      }
    }
  }

  // now compute the mean and std dev for each particle
  mean_areas_.resize(num_particles);
  stddev_areas_.resize(num_particles);
  for (int i = 0; i < num_particles; i++) {
    auto pair = EigenUtils::compute_mean_and_stddev(all_areas, i);
    mean_areas_(i) = pair.first;
    stddev_areas_(i) = pair.second;
  }
}

}  // namespace shapeworks
