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
 * method) and measures how far that reconstruction and the sample's groomed mesh
 * disagree, in both directions.  The disagreement is left on each groomed mesh, and
 * the one-directional pull distance on each reconstructed mesh, both under
 * FEATURE_NAME so either view can display it as a surface scalar.
 */
class CorrespondenceQualityJob : public Job {
  Q_OBJECT
 public:
  CorrespondenceQualityJob(QSharedPointer<Session> session, CorrespondenceEvaluation::DistanceMethod method);

  void run() override;
  QString name() override { return "Correspondence Quality"; }

  //! name of the per-vertex arrays: disagreement on each groomed mesh, pull distance on each reconstructed mesh
  static constexpr const char* FEATURE_NAME = "correspondence_distance";

  const CorrespondenceQualityReport& get_report() const { return report_; }

  //! index into Session::get_shapes() for each row of the report
  const std::vector<int>& get_row_shape_indices() const { return row_shape_indices_; }

  //! the disagreement averaged around each particle, keyed by index into Session::get_shapes().
  //! The glyphs are colored from these, so they need to be applied to the shapes (on the GUI
  //! thread) with Shape::set_point_features() before the field can be displayed.
  const std::map<int, Eigen::VectorXd>& get_particle_values() const { return particle_values_; }

  //! the per-vertex pull distance on the reconstructed mesh, for each domain of each shape, keyed by index
  //! into Session::get_shapes().  Shape::set_point_features() interpolates the particle values back over
  //! the reconstructed mesh under the same name, so these have to be re-applied after it to survive.
  const std::map<int, std::vector<vtkSmartPointer<vtkDataArray>>>& get_distance_fields() const {
    return distance_fields_;
  }

  //! the per-vertex disagreement on the groomed mesh, for each domain of each shape, keyed by index into
  //! Session::get_shapes()
  const std::map<int, std::vector<vtkSmartPointer<vtkDataArray>>>& get_disagreement_fields() const {
    return disagreement_fields_;
  }

 private:
  QSharedPointer<Session> session_;
  CorrespondenceEvaluation::DistanceMethod method_;

  CorrespondenceQualityReport report_;
  std::vector<int> row_shape_indices_;
  std::map<int, Eigen::VectorXd> particle_values_;
  std::map<int, std::vector<vtkSmartPointer<vtkDataArray>>> distance_fields_;
  std::map<int, std::vector<vtkSmartPointer<vtkDataArray>>> disagreement_fields_;
};

}  // namespace shapeworks
