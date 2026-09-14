#pragma once

#include <vtkSmartPointer.h>

#include <Eigen/Core>
#include <memory>
#include <string>
#include <vector>

class vtkDataArray;
class vtkPolyData;

namespace shapeworks {

class Mesh;
class Project;
using ProjectHandle = std::shared_ptr<Project>;

//! Distance statistics over one surface, in world units and as a fraction of the subject's
//! groomed-mesh bounding box diagonal.
struct CorrespondenceDistanceStats {
  double mean = 0.0;
  double median = 0.0;
  double p99 = 0.0;  //!< the worst part of the surface, without following a single stray vertex the way max does
  double max = 0.0;
  double norm_mean = 0.0;
  double norm_median = 0.0;
  double norm_p99 = 0.0;
  double norm_max = 0.0;
};

//! Per-subject-per-domain correspondence quality result row.
/*!
 * The unprefixed distances measure one direction only, from each reconstructed vertex to the groomed
 * surface ("pull").  Any part of the groomed surface the reconstruction never reaches -- a tear, a
 * collapsed opening, a missing appendage -- leaves every reconstructed vertex on the groomed surface,
 * so pull cannot see it.  `push` measures the other direction, and `disagreement` combines the two on
 * the groomed surface; it is the one to rank by.
 */
struct CorrespondenceQualityRow {
  std::string subject;
  int domain = 0;
  double mean_dist = 0.0;    //!< pull: mean distance from the reconstructed vertices to the groomed surface
  double median_dist = 0.0;  //!< pull: median per-vertex distance
  double p99_dist = 0.0;     //!< pull: 99th percentile per-vertex distance, a max that ignores single outlier vertices
  double max_dist = 0.0;     //!< pull: max per-vertex distance
  double bbox_diag = 0.0;    //!< diagonal of the subject's groomed-mesh bounding box
  double norm_mean = 0.0;    //!< mean_dist / bbox_diag (scale-invariant)
  double norm_median = 0.0;  //!< median_dist / bbox_diag
  double norm_p99 = 0.0;     //!< p99_dist / bbox_diag
  double norm_max = 0.0;     //!< max_dist / bbox_diag
  bool is_template = false;  //!< true for the L1-medoid template row (excluded from aggregates)

  // appended after is_template so that existing brace initializers keep their meaning

  //! from each groomed vertex to the reconstructed surface, weighted by the area each vertex stands for
  CorrespondenceDistanceStats push;

  //! on each groomed vertex, the larger of its push distance and the pull distance of any reconstructed
  //! vertex that lands beside it, weighted by area
  CorrespondenceDistanceStats disagreement;
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
  int num_evaluated = 0;  //!< rows.size() - template rows
  int num_template_rows = 0;
  CorrespondenceQualityStats agg_raw;                //!< aggregates over raw pull mean_dist (template excluded)
  CorrespondenceQualityStats agg_norm;               //!< aggregates over bbox-normalized pull (template excluded)
  CorrespondenceQualityStats agg_push_raw;           //!< aggregates over push.mean
  CorrespondenceQualityStats agg_push_norm;          //!< aggregates over push.norm_mean
  CorrespondenceQualityStats agg_disagreement_raw;   //!< aggregates over disagreement.mean
  CorrespondenceQualityStats agg_disagreement_norm;  //!< aggregates over disagreement.norm_mean
};

/**
 * \class CorrespondenceEvaluation
 * \ingroup Group-Particles
 *
 * Per-subject correspondence-quality metric: reconstruct each subject's shape
 * from its local particles via biharmonic mesh warp from the cohort L1-medoid
 * template (matches Studio's median-subject selection), then measure how far
 * that reconstruction and the subject's groomed mesh disagree.
 *
 * Both directions are measured.  Pull, from each reconstructed vertex to the
 * groomed surface, catches reconstruction that departs from the surface: folds,
 * flaps, spikes.  Push, from each groomed vertex to the reconstruction, catches
 * groomed surface the reconstruction never reaches: tears and collapsed
 * openings.  Disagreement combines them into one field on the groomed mesh, the
 * only surface that can show a gap.  Distances are also normalized by each
 * subject's bounding-box diagonal so the metric is scale-invariant.
 *
 * The template row itself is included in `rows` (with is_template=true) but
 * excluded from aggregate statistics — its reconstruction is near-identity
 * and would skew small cohorts.
 *
 * `evaluate()` drives the whole thing from a project file. Callers that already
 * have reconstructions in memory (Studio, which reconstructs through its own
 * configured mesh warper) should use `evaluate_reconstruction()` and
 * `compute_aggregates()` instead so the metric definition stays in one place.
 */
class CorrespondenceEvaluation {
 public:
  enum class DistanceMethod { PointToCell, PointToPoint };

  //! Evaluate. Project is expected to be already loaded, and the current
  //! working directory must be one from which the project's relative paths
  //! (groomed, local particles) resolve.
  //!
  //! If \p output_meshes_dir is non-empty, per-subject reconstructed meshes
  //! are written there as .vtk with an embedded per-vertex pull "distance"
  //! field, and the groomed meshes alongside them with "disagreement" and
  //! "push" fields. The path is used verbatim (interpreted relative to the
  //! current CWD if not absolute).
  //!
  //! Throws std::runtime_error on setup failures (no subjects, warp failure,
  //! mismatched particle counts across subjects).
  static CorrespondenceQualityReport evaluate(ProjectHandle project,
                                              DistanceMethod method = DistanceMethod::PointToCell,
                                              const std::string& output_meshes_dir = "");

  //! Score a single already-reconstructed mesh against its groomed target.
  //!
  //! Fills everything on the row except `subject`, `domain` and `is_template`,
  //! which the caller owns. If \p out_distance is non-null it receives the
  //! pull field on the reconstruction's vertices (named "distance").
  //! \p out_disagreement and \p out_push receive fields on the groomed mesh's
  //! vertices (named "disagreement" and "push"); disagreement is the field to
  //! color by, since a gap in the reconstruction has no reconstructed surface
  //! to show it on.
  //!
  //! Returns a default-constructed row if \p reconstructed is null or empty.
  static CorrespondenceQualityRow evaluate_reconstruction(vtkSmartPointer<vtkPolyData> reconstructed,
                                                          const Mesh& groomed, DistanceMethod method,
                                                          vtkSmartPointer<vtkDataArray>* out_distance = nullptr,
                                                          vtkSmartPointer<vtkDataArray>* out_disagreement = nullptr,
                                                          vtkSmartPointer<vtkDataArray>* out_push = nullptr);

  //! Summary statistics (mean/median/p95/max) over a set of values.
  static CorrespondenceQualityStats summarize(std::vector<double> values);

  //! Fill num_evaluated, num_template_rows and the aggregates from report.rows.
  static void compute_aggregates(CorrespondenceQualityReport& report);
};

}  // namespace shapeworks
