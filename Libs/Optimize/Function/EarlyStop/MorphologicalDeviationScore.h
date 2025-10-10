#pragma once

#include <Eigen/Dense>

namespace shapeworks {
class MorphologicalDeviationScore {
public:
    MorphologicalDeviationScore();
    // Fit PPCA model on control shapes
    bool SetControlShapes(const Eigen::MatrixXd& X); // (n_samples x n_features)
    /// Get Mahalanobis-based deviation score for test samples (non-fixed shapes/domains)
    Eigen::VectorXd GetMahalanobisDistance(const Eigen::MatrixXd& X); // (n,)

private:
    /// Flag to ensure control shapes are set and PCA model is in place
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