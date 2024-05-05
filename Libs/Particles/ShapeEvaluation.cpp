#include "ShapeEvaluation.h"

#include <Eigen/Core>
#include <Eigen/SVD>
#include <fstream>

#include "EvaluationUtil.h"

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

namespace shapeworks {

//---------------------------------------------------------------------------
double ShapeEvaluation::compute_compactness(const ParticleSystemEvaluation& particle_system,
                                            const int num_modes,
                                            const std::string& save_to) {
  const int n = particle_system.N();
  if (num_modes > n - 1) {
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  Eigen::VectorXd cumsum = ShapeEvaluation::compute_full_compactness(particle_system);

  if (!save_to.empty()) {
    std::ofstream of(save_to);
    of << cumsum;
    of.close();
  }

  return cumsum(num_modes - 1);
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::compute_full_compactness(const ParticleSystemEvaluation& particle_system,
                                                          std::function<void(float)> progress_callback) {
  const int n = particle_system.N();
  const int d = particle_system.D();
  const int num_modes = n - 1;  // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  Eigen::MatrixXd y = particle_system.Particles();
  const Eigen::VectorXd mu = y.rowwise().mean();
  y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(y);
  const auto s = svd.singularValues().array().pow(2) / (n * d);

  // Compute cumulative sum
  Eigen::VectorXd cumsum(num_modes);
  cumsum(0) = s(0);
  for (int i = 1; i < num_modes; i++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(i) / static_cast<float>(n));
    }
    cumsum(i) = cumsum(i - 1) + s(i);
  }
  cumsum /= s.sum();
  return cumsum;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::compute_generalization(const ParticleSystemEvaluation& particle_system,
                                               const int num_modes,
                                               const std::string& save_to) {
  const int n = particle_system.N();
  const int d = particle_system.D();
  const Eigen::MatrixXd& p = particle_system.Particles();

  if (num_modes > n - 1) {
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  double total_dist = 0.0;
  for (int leave = 0; leave < n; leave++) {
    Eigen::MatrixXd y(d, n - 1);
    y.leftCols(leave) = p.leftCols(leave);
    y.rightCols(n - leave - 1) = p.rightCols(n - leave - 1);

    const Eigen::VectorXd mu = y.rowwise().mean();
    y.colwise() -= mu;
    const Eigen::VectorXd y_test = p.col(leave);

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeFullU);
    const auto epsi = svd.matrixU().block(0, 0, d, num_modes);
    const auto betas = epsi.transpose() * (y_test - mu);
    const Eigen::VectorXd rec = epsi * betas + mu;

    const int num_particles = d / VDimension;
    const Eigen::Map<const RowMajorMatrix>
        y_test_reshaped(y_test.data(), num_particles, VDimension);
    const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), num_particles, VDimension);
    const double dist = (rec_reshaped - y_test_reshaped).rowwise().norm().sum() / num_particles;
    total_dist += dist;

    reconstructions.push_back({dist, leave, rec_reshaped});
  }
  const double generalization = total_dist / n;

  // Save the reconstructions if needed. Generates XML files that can be opened in
  // ShapeWorksView2
  if (!save_to.empty()) {
    SaveReconstructions(reconstructions, particle_system.Paths(), save_to);
  }

  return generalization;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::compute_full_generalization(const ParticleSystemEvaluation& particle_system,
                                                             std::function<void(float)> progress_callback) {
  const int n = particle_system.N();  // number of samples
  const int d = particle_system.D();  // number of dimensions (e.g. number of particles * 3)
  const Eigen::MatrixXd& p = particle_system.Particles();

  if (n <= 1) {
    return Eigen::VectorXd();
  }

  Eigen::VectorXd generalizations(n - 1);

  Eigen::VectorXd total_dists = Eigen::VectorXd::Zero(n - 1);

  for (int leave = 0; leave < n; leave++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(leave) / static_cast<float>(n));
    }
    Eigen::MatrixXd y(d, n - 1);
    y.leftCols(leave) = p.leftCols(leave);
    y.rightCols(n - leave - 1) = p.rightCols(n - leave - 1);

    const Eigen::VectorXd mu = y.rowwise().mean();
    y.colwise() -= mu;
    const Eigen::VectorXd y_test = p.col(leave);

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeFullU);

    for (int mode = 1; mode < n; mode++) {
      const auto epsi = svd.matrixU().block(0, 0, d, mode);
      const auto betas = epsi.transpose() * (y_test - mu);
      const Eigen::VectorXd rec = epsi * betas + mu;

      const int num_particles = d / VDimension;
      const Eigen::Map<const RowMajorMatrix>
          ytest_reshaped(y_test.data(), num_particles, VDimension);
      const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), num_particles, VDimension);
      const double dist = (rec_reshaped - ytest_reshaped).rowwise().norm().sum() / num_particles;
      total_dists(mode - 1) += dist;
    }
  }

  generalizations = total_dists / n;

  return generalizations;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::compute_specificity(const ParticleSystemEvaluation& particle_system,
                                            const int num_modes,
                                            const std::string& save_to) {
  const int n = particle_system.N();
  const int d = particle_system.D();

  if (num_modes > n - 1) {
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  const int num_samples = 1000;

  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  Eigen::VectorXd mean_specificity(num_modes);
  Eigen::VectorXd std_specificity(num_modes);
  Eigen::MatrixXd spec_store(num_modes, 4);

  // PCA calculations
  const Eigen::MatrixXd& pts_models = particle_system.Particles();
  const Eigen::VectorXd mu = pts_models.rowwise().mean();
  Eigen::MatrixXd y = pts_models;

  y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeFullU);
  const auto epsi = svd.matrixU().block(0, 0, d, num_modes);
  const auto all_eigen_values = svd.singularValues();
  const auto eigen_values = all_eigen_values.head(num_modes);

  Eigen::MatrixXd sampling_betas(num_modes, num_samples);
  MultiVariateNormalRandom sampling{eigen_values.asDiagonal()};
  for (int mode_number = 0; mode_number < num_modes; mode_number++) {
    for (int i = 0; i < num_samples; i++) {
      sampling_betas.col(i) = sampling();
    }

    Eigen::MatrixXd sampling_points = (epsi * sampling_betas).colwise() + mu;

    const int num_particles = d / VDimension;
    const int num_train = pts_models.cols();

    Eigen::VectorXd distance_to_closest_training_sample(num_samples);

    for (int i = 0; i < num_samples; i++) {
      Eigen::VectorXd pts_m = sampling_points.col(i);
      Eigen::MatrixXd pts_distance_vec = pts_models.colwise() - pts_m;
      Eigen::MatrixXd pts_distance(Eigen::MatrixXd::Constant(1, num_train, 0.0));

      for (int j = 0; j < num_train; j++) {
        Eigen::Map<const RowMajorMatrix>
            pts_distance_vec_reshaped(pts_distance_vec.col(j).data(), num_particles,
                                      VDimension);
        pts_distance(j) = (pts_distance_vec_reshaped).rowwise().norm().sum();
      }

      int closest_idx, r;
      distance_to_closest_training_sample(i) = pts_distance.minCoeff(&r, &closest_idx);

      Eigen::Map<const RowMajorMatrix> pts_m_reshaped(pts_m.data(), num_particles, VDimension);
      reconstructions.push_back(Reconstruction{
          distance_to_closest_training_sample(i),
          (int) closest_idx,
          pts_m_reshaped,
      });
    }

    mean_specificity(mode_number) = distance_to_closest_training_sample.mean();
  }

  if (!save_to.empty()) {
    SaveReconstructions(reconstructions, particle_system.Paths(), save_to);
  }

  const int num_particles = d / VDimension;
  const double specificity = mean_specificity(num_modes - 1) / num_particles;

  return specificity;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::compute_full_specificity(const ParticleSystemEvaluation& particle_system,
                                                          std::function<void(float)> progress_callback) {
  const int n = particle_system.N();
  const int d = particle_system.D();
  const int num_particles = d / VDimension;

  Eigen::VectorXd specificities(n - 1);

  // PCA calculations
  const Eigen::MatrixXd& pts_models = particle_system.Particles();
  const int num_train = pts_models.cols();

  const Eigen::VectorXd mu = pts_models.rowwise().mean();
  Eigen::MatrixXd y = pts_models;
  y.colwise() -= mu;
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeFullU);
  const auto all_eigen_values = svd.singularValues();

  for (int num_modes = 1; num_modes < n; num_modes++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(num_modes) / static_cast<float>(n));
    }

    const int num_samples = 1000;

    Eigen::VectorXd std_specificity(num_modes);
    Eigen::MatrixXd spec_store(num_modes, 4);
    const auto eigen_values = all_eigen_values.head(num_modes);
    const auto epsi = svd.matrixU().block(0, 0, d, num_modes);

    Eigen::MatrixXd sampling_betas(num_modes, num_samples);
    MultiVariateNormalRandom sampling{eigen_values.asDiagonal()};
    for (int i = 0; i < num_samples; i++) {
      sampling_betas.col(i) = sampling();
    }

    Eigen::MatrixXd sampling_points = (epsi * sampling_betas).colwise() + mu;
    Eigen::VectorXd distance_to_closest_training_sample(num_samples);

    for (int i = 0; i < num_samples; i++) {
      Eigen::VectorXd pts_m = sampling_points.col(i);
      Eigen::MatrixXd pts_distance_vec = pts_models.colwise() - pts_m;
      Eigen::MatrixXd pts_distance(Eigen::MatrixXd::Constant(1, num_train, 0.0));

      for (int j = 0; j < num_train; j++) {
        Eigen::Map<const RowMajorMatrix>
            pts_distance_vec_reshaped(pts_distance_vec.col(j).data(), num_particles,
                                      VDimension);
        pts_distance(j) = (pts_distance_vec_reshaped).rowwise().norm().sum();
      }

      int closest_idx, r;
      distance_to_closest_training_sample(i) = pts_distance.minCoeff(&r, &closest_idx);
    }

    double mean_specificity = distance_to_closest_training_sample.mean();
    const double specificity = mean_specificity / num_particles;
    specificities(num_modes - 1) = specificity;
  }
  return specificities;
}

}  // namespace shapeworks
