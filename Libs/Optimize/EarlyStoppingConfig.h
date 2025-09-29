#pragma once
#include <string>

namespace shapeworks {

/// strategies for early stopping
enum class EarlyStoppingStrategy {
  /// stop when relative improvement falls below a threshold
  RelativeDifference,
  /// stop using EMA of improvements to detect convergence
  ExponentialMovingAverage
};

/// Configuration for early stopping criteria
struct EarlyStoppingConfig {
  bool enabled = false;

  /// how often (in iterations) to check the stopping criterion
  int frequency = 100;

  /// number of past values to consider in relative difference or EMA
  int window_size = 5;

  /// Threshold for stopping:
  /// For RelativeDifference: use ~1e-4 or smaller.
  /// For EMA: use ~1e-1 or smaller.
  double threshold = 0.0001;

  /// strategy used for determining early stopping.
  EarlyStoppingStrategy strategy = EarlyStoppingStrategy::RelativeDifference;

  /// higher value of alpha give more weight to recent iterations
  double ema_alpha = 0.2;

  bool enable_logging = false;

  std::string logger_name = "early_stopping_log_stats";

  /// to prevent premature stoppin
  int warmup_iters = 1000;
};

}  // namespace shapeworks
