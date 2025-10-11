---
title: Libs/Optimize/Function/EarlyStop/EarlyStopping.h

---

# Libs/Optimize/Function/EarlyStop/EarlyStopping.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::EarlyStopping](../Classes/classshapeworks_1_1EarlyStopping.md)**  |




## Source code

```cpp
#pragma once
#include <Eigen/Dense>

#include "Libs/Optimize/ParticleSystem.h"
#include "Libs/Optimize/EarlyStoppingConfig.h"
#include "MorphologicalDeviationScore.h"

namespace shapeworks {

class EarlyStopping {
public:
    typedef typename ParticleSystem::PointType PointType;
    constexpr static int VDimension = 3;
    EarlyStopping();
    void SetConfigParams(int frequency,
                  int window_size,
                  double threshold,
                  EarlyStoppingStrategy strategy = EarlyStoppingStrategy::RelativeDifference,
                  double ema_alpha = 0.2,
                  bool enable_logging = false,
                  const std::string& logger_name = "",
                  int warmup_iters = 1000);

    void reset();
    void update(int iteration, const ParticleSystem* p);
    bool ShouldStop() const;
    bool SetControlShapes(const ParticleSystem* p);
    Eigen::MatrixXd GetTestShapes(const ParticleSystem* p);

private:
    std::deque<Eigen::VectorXd> score_history_;
    int frequency_, window_size_;
    double threshold_, ema_alpha_;
    int last_checked_iter_;
    int warmup_iters_;
    // bool stop_flag_;
    mutable std::atomic<bool> stop_flag_{false};
    bool enable_logging_;
    std::string logger_name_;
    Eigen::MatrixXd control_shapes_; 
    MorphologicalDeviationScore score_func_;
    EarlyStoppingStrategy strategy_;
    mutable Eigen::VectorXd ema_diff_;
    mutable bool ema_initialized_ = false;

    Eigen::VectorXd ComputeScore(const Eigen::MatrixXd& X) ;
    Eigen::VectorXd ComputeRelativeDiff(const Eigen::VectorXd& a, const Eigen::VectorXd& b) const;
    bool HasConverged() const;
    bool CheckRelativeDifference() const;
    bool CheckExponentialMovingAverage() const;
    void LogStatus(int iter,
                   const Eigen::VectorXd& current_score,
                   const Eigen::VectorXd& diff,
                   const std::vector<bool>& per_subject_convergence) const;
};

} // namespace shapeworks
```


-------------------------------

Updated on 2025-10-11 at 18:16:34 +0000
