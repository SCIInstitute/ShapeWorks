
#include "ParticleAreaJob.h"

#include <Analyze/ParticleArea.h>
#include <Data/Session.h>
#include <Logging.h>
#include <vtkLookupTable.h>

#include "ParticleArea.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleAreaJob::ParticleAreaJob(QSharedPointer<Session> session) { session_ = session; }

//---------------------------------------------------------------------------
void ParticleAreaJob::run() {
  SW_DEBUG("running");

  /**************
  {  // TODO: temporarily here
    if (meshes_.valid() && glyph_lut_) {
      // for each domain
      for (size_t i = 0; i < meshes_.meshes().size(); i++) {
        auto points = shape_->get_particles().get_local_points(i);
        auto poly_data = meshes_.meshes()[i]->get_poly_data();
        if (!poly_data) {
          continue;
        }
        ParticleArea::assign_vertex_particles(poly_data, points);
        auto colors = ParticleArea::colors_from_lut(glyph_lut_);
        ParticleArea::assign_vertex_colors(poly_data, colors);
      }
    }
  }
  **********/

  int num_domains = session_->get_domains_per_shape();

  float total = (num_domains * session_->get_shapes().size()) + 1;
  float count = 0;

  auto lut = session_->get_glyph_lut();

  // for each shape
  auto shapes = session_->get_shapes();
  for (auto& shape : shapes) {
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
        count++;
        Q_EMIT progress(count / total);
      }
    }
  }

  Q_EMIT progress(1.0);
}

//---------------------------------------------------------------------------
QString ParticleAreaJob::name() { return "Particle Normal Evaluation"; }

}  // namespace shapeworks
