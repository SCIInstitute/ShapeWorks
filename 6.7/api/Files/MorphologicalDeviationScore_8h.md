---
title: Libs/Optimize/Function/EarlyStop/MorphologicalDeviationScore.h

---

# Libs/Optimize/Function/EarlyStop/MorphologicalDeviationScore.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MorphologicalDeviationScore](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Dense>

namespace shapeworks {
class MorphologicalDeviationScore {
public:
    MorphologicalDeviationScore();
    // Fit PPCA model on control shapes
    bool SetControlShapes(const Eigen::MatrixXd& X); // (n_samples x n_features)
    Eigen::VectorXd GetMahalanobisDistance(const Eigen::MatrixXd& X); // (n,)

private:
    bool is_fitted_ = false;              

    // Fitted model parameters
    Eigen::RowVectorXd mean_; // (1 x d)
    Eigen::MatrixXd components_; // (d x q)
    Eigen::VectorXd eigvals_; // (q,)
    double noise_variance_;
    // Derived matrices
    Eigen::MatrixXd precision_matrix_; // (d x d)
    // Helper functions
    bool FitPPCA(const Eigen::MatrixXd& X);
    Eigen::MatrixXd ComputeCovarianceMatrix();
    Eigen::MatrixXd ComputePrecisionMatrix();
};
} // namespace shapeworks
```


-------------------------------

Updated on 2025-10-11 at 04:45:08 +0000
