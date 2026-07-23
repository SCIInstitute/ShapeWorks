#pragma once

#include <Eigen/Core>
#include <memory>
#include <string>
#include <vector>

namespace shapeworks {

class Project;
using ProjectHandle = std::shared_ptr<Project>;

//! Per-subject-per-domain correspondence quality result row.
struct CorrespondenceQualityRow {
  std::string subject;
  int domain = 0;
  double mean_dist = 0.0;    //!< mean point-to-cell (or point-to-point) distance, reconstructed -> groomed
  double max_dist = 0.0;     //!< max per-vertex distance
  double bbox_diag = 0.0;    //!< diagonal of the subject's groomed-mesh bounding box
  double norm_mean = 0.0;    //!< mean_dist / bbox_diag (scale-invariant)
  double norm_max = 0.0;     //!< max_dist / bbox_diag
  bool is_template = false;  //!< true for the L1-medoid template row (excluded from aggregates)
};

//! Aggregate summary statistics.
struct CorrespondenceQualityStats {
  double mean = 0.0;
  double median = 0.0;
  double p95 = 0.0;
  double max = 0.0;
};

//! Full evaluation report.
struct CorrespondenceQualityReport {
  std::vector<CorrespondenceQualityRow> rows;
  std::string template_subject;
  int num_evaluated = 0;                 //!< rows.size() - template rows
  int num_template_rows = 0;
  CorrespondenceQualityStats agg_raw;    //!< aggregates over raw mean_dist (template excluded)
  CorrespondenceQualityStats agg_norm;   //!< aggregates over bbox-normalized values (template excluded)
};

/**
 * \class CorrespondenceEvaluation
 * \ingroup Group-Particles
 *
 * Per-subject correspondence-quality metric: reconstruct each subject's shape
 * from its local particles via biharmonic mesh warp from the cohort L1-medoid
 * template (matches Studio's median-subject selection), then measure distance
 * from the reconstruction to that subject's groomed mesh. Distances are also
 * normalized by each subject's bounding-box diagonal so the metric is
 * scale-invariant.
 *
 * The template row itself is included in `rows` (with is_template=true) but
 * excluded from aggregate statistics — its reconstruction is near-identity
 * and would skew small cohorts.
 */
class CorrespondenceEvaluation {
 public:
  enum class DistanceMethod { PointToCell, PointToPoint };

  //! Evaluate. Project is expected to be already loaded, and the current
  //! working directory must be one from which the project's relative paths
  //! (groomed, local particles) resolve.
  //!
  //! If \p output_meshes_dir is non-empty, per-subject reconstructed meshes
  //! are written there as .vtk with an embedded per-vertex "distance" field.
  //! The path is used verbatim (interpreted relative to the current CWD if
  //! not absolute).
  //!
  //! Throws std::runtime_error on setup failures (no subjects, warp failure,
  //! mismatched particle counts across subjects).
  static CorrespondenceQualityReport evaluate(ProjectHandle project,
                                              DistanceMethod method = DistanceMethod::PointToCell,
                                              const std::string& output_meshes_dir = "");
};

}  // namespace shapeworks
