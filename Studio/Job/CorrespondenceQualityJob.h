#pragma once
#include <Job/Job.h>
#include <Particles/CorrespondenceEvaluation.h>

#include <Eigen/Core>
#include <QSharedPointer>
#include <map>
#include <vector>

namespace shapeworks {

class Session;

//! Scores each sample's correspondence quality.
/*!
 * Reconstructs each sample through Studio's own configured mesh warper (the same
 * reconstruction shown in the viewer, using the user's chosen template and warp
 * method) and measures the distance from that reconstruction back to the sample's
 * groomed mesh.  The per-vertex distance field is left on each reconstructed mesh
 * under FEATURE_NAME so it can be displayed as a surface scalar.
 */
class CorrespondenceQualityJob : public Job {
  Q_OBJECT
 public:
  CorrespondenceQualityJob(QSharedPointer<Session> session, CorrespondenceEvaluation::DistanceMethod method);

  void run() override;
  QString name() override { return "Correspondence Quality"; }

  //! name of the per-vertex distance array attached to each reconstructed mesh
  static constexpr const char* FEATURE_NAME = "correspondence_distance";

  const CorrespondenceQualityReport& get_report() const { return report_; }

  //! index into Session::get_shapes() for each row of the report
  const std::vector<int>& get_row_shape_indices() const { return row_shape_indices_; }

  //! the distance field sampled at each particle, keyed by index into Session::get_shapes().
  //! The glyphs are colored from these, so they need to be applied to the shapes (on the GUI
  //! thread) with Shape::set_point_features() before the field can be displayed.
  const std::map<int, Eigen::VectorXd>& get_particle_values() const { return particle_values_; }

 private:
  QSharedPointer<Session> session_;
  CorrespondenceEvaluation::DistanceMethod method_;

  CorrespondenceQualityReport report_;
  std::vector<int> row_shape_indices_;
  std::map<int, Eigen::VectorXd> particle_values_;
};

}  // namespace shapeworks
