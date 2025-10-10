#include "EarlyStopping.h"
#include "MorphologicalDeviationScore.h"
#include <Eigen/Dense>
#include <Profiling.h>
#include <Logging.h>

namespace shapeworks {

EarlyStopping::EarlyStopping()
    : frequency_(100),
      window_size_(5),
      threshold_(0.0001),
      strategy_(EarlyStoppingStrategy::RelativeDifference),
      ema_alpha_(0.2),
      enable_logging_(false),
      logger_name_("early_stopping"),
      last_checked_iter_(-1),
      warmup_iters_(1000),
      score_func_() {}

//---------------------------------------------------------------------------
void EarlyStopping::SetConfigParams(int frequency, int window_size,
                                    double threshold,
                                    EarlyStoppingStrategy strategy,
                                    double ema_alpha, bool enable_logging,
                                    const std::string& logger_name,
                                    int warmup_iters) {
  frequency_ = frequency;
  window_size_ = window_size;
  threshold_ = threshold;
  strategy_ = strategy;
  ema_alpha_ = ema_alpha;
  enable_logging_ = enable_logging;
  logger_name_ = logger_name;
  last_checked_iter_ = -1;
  warmup_iters_ = warmup_iters;
}

//---------------------------------------------------------------------------
void EarlyStopping::reset() {
  score_history_.clear();
  ema_initialized_ = false;
  ema_diff_ = Eigen::VectorXd();
  last_checked_iter_ = -1;
  // stop_flag_ = false;
  stop_flag_.store(false);
}

//---------------------------------------------------------------------------
void EarlyStopping::update(int iteration, const ParticleSystem* p) {
  if (stop_flag_ || (iteration - last_checked_iter_ < frequency_)) return;

  if (iteration < warmup_iters_) return;

  if (control_shapes_.size() == 0) {
    throw std::runtime_error("Control shapes not initialized yet.");
  }

  TIME_START("early_stopping_score");
  Eigen::MatrixXd X = GetTestShapes(p);
  if (X.size() == 0) return;

  last_checked_iter_ = iteration;
  Eigen::VectorXd score = ComputeScore(X);

  SW_DEBUG("Early stopping score mean at optimization iteration {} = {}", iteration, score.mean());

  if (!ema_initialized_) {
    ema_diff_ = Eigen::VectorXd::Zero(score.size());
  }

  score_history_.push_back(score);
  if (score_history_.size() > window_size_ + 1) score_history_.pop_front();

  if (score_history_.size() >= 2) {
    if (HasConverged())
      // stop_flag_ = true;
      stop_flag_.store(true);
  }
  TIME_STOP("early_stopping_score");

}

// bool EarlyStopping::ShouldStop() const {
//     return stop_flag_;
// }

//---------------------------------------------------------------------------
bool EarlyStopping::ShouldStop() const { return stop_flag_.load(); }

//---------------------------------------------------------------------------
Eigen::VectorXd EarlyStopping::ComputeScore(const Eigen::MatrixXd& X) {
  return score_func_.GetMahalanobisDistance(X);
}

//---------------------------------------------------------------------------
Eigen::VectorXd EarlyStopping::ComputeRelativeDiff(
    const Eigen::VectorXd& a, const Eigen::VectorXd& b) const {
  const double eps = 1e-8;
  return (a - b).cwiseAbs().array() / (b.cwiseAbs().array() + eps);
}

//---------------------------------------------------------------------------
bool EarlyStopping::HasConverged() const {

  switch (strategy_) {
    case EarlyStoppingStrategy::RelativeDifference:
      return CheckRelativeDifference();
    case EarlyStoppingStrategy::ExponentialMovingAverage:
      return CheckExponentialMovingAverage();
    default:
      return false;
  }
}

//---------------------------------------------------------------------------
bool EarlyStopping::CheckRelativeDifference() const {

  const Eigen::VectorXd& current = score_history_.back();
  const Eigen::VectorXd& prev = score_history_[score_history_.size() - 2];

  Eigen::VectorXd rel_diff = ComputeRelativeDiff(current, prev);
  std::vector<bool> converged(rel_diff.size());
  for (int i = 0; i < rel_diff.size(); ++i) {
    converged[i] = rel_diff[i] < threshold_;
    // std::cout << "DEBUG | Test Shape " << i << "/" << rel_diff.size() << " rel_diff = " << rel_diff[i] << " converged = " << converged[i];
  }

  if (enable_logging_)
    LogStatus(last_checked_iter_, current, rel_diff, converged);

  return std::all_of(converged.begin(), converged.end(),
                     [](bool x) { return x; });
}

//---------------------------------------------------------------------------
bool EarlyStopping::CheckExponentialMovingAverage() const {
  const Eigen::VectorXd& current = score_history_.back();
  const Eigen::VectorXd& prev = score_history_[score_history_.size() - 2];

  Eigen::VectorXd abs_diff = (current - prev).cwiseAbs();

  if (!ema_initialized_) {
    ema_diff_ = abs_diff;
    ema_initialized_ = true;
  } else {
    ema_diff_ = ema_alpha_ * abs_diff + (1.0 - ema_alpha_) * ema_diff_;
  }

  std::vector<bool> converged(ema_diff_.size());
  for (int i = 0; i < ema_diff_.size(); ++i)
  {
    converged[i] = ema_diff_[i] < threshold_;
    // std::cout << "DEBUG | Test Shape " << i << "/" << ema_diff_.size() << " ema_diff_ = " << ema_diff_[i] << " converged = " << converged[i];

  }

  if (enable_logging_)
    LogStatus(last_checked_iter_, current, ema_diff_, converged);

  return std::all_of(converged.begin(), converged.end(),
                     [](bool x) { return x; });
}

//---------------------------------------------------------------------------
void EarlyStopping::LogStatus(
    int iter, const Eigen::VectorXd& score, const Eigen::VectorXd& diff,
    const std::vector<bool>& per_subject_convergence) const {
  std::cout << "[EarlyStopping";
  if (!logger_name_.empty()) std::cout << " - " << logger_name_;
  std::cout << "] Iteration " << iter << ":\n";

  for (int i = 0; i < score.size(); ++i) {
    std::cout << "  TestShape[" << i << "]: Score=" << score[i]
              << ", Diff=" << diff[i]
              << ", Status= " << (per_subject_convergence[i] ? "Converged" : "Not Converged")
              << "\n";
  }

  int num_converged = std::count(per_subject_convergence.begin(),
                                 per_subject_convergence.end(), true);
  std::cout << "  --> " << num_converged << " / " << score.size()
            << " shapes converged\n\n";
}

//---------------------------------------------------------------------------
bool EarlyStopping::SetControlShapes(const ParticleSystem* p) {
  if (control_shapes_.size() > 0) return true;  // already initialized return back

  unsigned int numdomains = p->GetNumberOfDomains();
  const auto domains_per_shape = p->GetDomainsPerShape();
  unsigned int num_shapes = numdomains / domains_per_shape;

  std::vector<Eigen::RowVectorXd> shape_vectors;
  for (size_t shape = 0; shape < num_shapes; ++shape) {
    for (int shape_dom_idx = 0; shape_dom_idx < domains_per_shape;
         ++shape_dom_idx) {
      auto dom = shape * domains_per_shape + shape_dom_idx;

      if (p->GetDomainFlag(dom) == true) {
        auto num_points = p->GetPositions(dom)->GetSize();
        Eigen::RowVectorXd shape_vector(VDimension * num_points);

        for (int k = 0; k < num_points; ++k) {
        //   PointType pt = p->GetTransformedPosition(dom, k);
        PointType pt = p->GetPositions(dom)->Get(k);

          shape_vector(VDimension * k + 0) = pt[0];
          shape_vector(VDimension * k + 1) = pt[1];
          shape_vector(VDimension * k + 2) = pt[2];
        }

        shape_vectors.push_back(shape_vector);
      }
    }
  }

  int num_control_shapes = static_cast<int>(shape_vectors.size());
  if (num_control_shapes > 0) {
    int d = static_cast<int>(shape_vectors[0].cols());
    control_shapes_.resize(num_control_shapes, d);
    for (int i = 0; i < num_control_shapes; ++i) {
      control_shapes_.row(i) = shape_vectors[i];
    }
  } else {
    return false; // Forcibly turn off early stopping now if no fixed domains present
    // TODO: handle non-fixed domain cases when no fixed domains are present for control shapes pca initialization
    // Maybe, initialize with representative clusters?
    // control_shapes_.resize(0, 0);
    // throw std::runtime_error(
    //     "Fix some domains with particles initialized to use early stopping "
    //     "feature in fixed domains optimization");
  }
  return score_func_.SetControlShapes(control_shapes_);

}

//---------------------------------------------------------------------------
Eigen::MatrixXd EarlyStopping::GetTestShapes(const ParticleSystem* p) {
  unsigned int numdomains = p->GetNumberOfDomains();
  const auto domains_per_shape = p->GetDomainsPerShape();
  unsigned int num_shapes = numdomains / domains_per_shape;
  std::vector<Eigen::RowVectorXd> shape_vectors;
  Eigen::MatrixXd test_shapes;
  for (size_t shape = 0; shape < num_shapes; ++shape) {
    for (int shape_dom_idx = 0; shape_dom_idx < domains_per_shape;
         ++shape_dom_idx) {
      auto dom = shape * domains_per_shape + shape_dom_idx;

      if (!p->GetDomainFlag(dom)) {
        auto num_points = p->GetPositions(dom)->GetSize();
        Eigen::RowVectorXd shape_vector(VDimension * num_points);

        for (int k = 0; k < num_points; ++k) {
        //   PointType pt = p->GetTransformedPosition(dom, k);
            PointType pt = p->GetPositions(dom)->Get(k);
          shape_vector(3 * k + 0) = pt[0];
          shape_vector(3 * k + 1) = pt[1];
          shape_vector(3 * k + 2) = pt[2];
        }

        shape_vectors.push_back(shape_vector);
      }
    }
  }
  int num_test_shapes = static_cast<int>(shape_vectors.size());
//   std::cout << "DEBUG | Found " << num_test_shapes << " Test Shapes " << std::endl;
  if (num_test_shapes > 0) {
    int d = static_cast<int>(shape_vectors[0].cols());
    test_shapes.resize(num_test_shapes, d);
    for (int i = 0; i < num_test_shapes; ++i) {
      test_shapes.row(i) = shape_vectors[i];
    }
  } else {
    test_shapes.resize(0, 0);
  }
  return test_shapes;
}

}  // namespace shapeworks