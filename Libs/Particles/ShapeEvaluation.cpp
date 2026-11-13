#include "ShapeEvaluation.h"

#include <Logging.h>
#include <Profiling.h>
#include <tbb/parallel_for.h>

#include <Eigen/Core>
#include <Eigen/SVD>
#include <atomic>
#include <fstream>
#include <thread>

#include "EvaluationUtil.h"

using RowMajorMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace shapeworks {

namespace {

//! Lets parallel workers honor the progress and abort callbacks while only ever invoking them on the calling
//! thread. The callbacks may be Python callables, which need the GIL that the calling thread holds, so calling them
//! from a worker thread would deadlock.
class CallerThreadCallbacks {
 public:
  CallerThreadCallbacks(std::function<void(float)> progress_callback, std::function<bool()> check_abort, long total)
      : progress_callback_(std::move(progress_callback)),
        check_abort_(std::move(check_abort)),
        total_(total),
        caller_(std::this_thread::get_id()) {}

  //! Returns true if the computation should stop, polling check_abort when on the calling thread
  bool aborted() {
    if (!aborted_ && check_abort_ && std::this_thread::get_id() == caller_ && check_abort_()) {
      aborted_ = true;
    }
    return aborted_;
  }

  //! Returns true if an abort was observed, without polling check_abort again
  bool was_aborted() const { return aborted_; }

  //! Records one finished unit of work, reporting progress when on the calling thread
  void complete_one() {
    const long done = ++completed_;
    if (progress_callback_ && std::this_thread::get_id() == caller_) {
      progress_callback_(static_cast<float>(done) / static_cast<float>(total_));
    }
  }

 private:
  std::function<void(float)> progress_callback_;
  std::function<bool()> check_abort_;
  const long total_;
  const std::thread::id caller_;
  std::atomic<bool> aborted_{false};
  std::atomic<long> completed_{0};
};

//! Run f(i) for i in [0, n) in parallel. With OpenMP enabled, this uses an OpenMP loop rather than TBB: Eigen
//! parallelizes its matrix products with OpenMP and only stands down inside an OpenMP parallel region, so each TBB
//! task would otherwise start its own full OpenMP team and oversubscribe the machine.
template <class Function>
void parallel_for_index(long n, const Function& f) {
#ifdef SW_USE_OPENMP
#pragma omp parallel for schedule(dynamic, 1)
  for (long i = 0; i < n; i++) {
    f(i);
  }
#else
  tbb::parallel_for(long{0}, n, f);
#endif
}

//! Build each mesh's cell locator up front. Locators are built lazily, so this must happen before closestPoint is
//! called concurrently, and it avoids rebuilding a locator for every copy of a mesh.
void build_cell_locators(const std::vector<Mesh>& meshes) {
  tbb::parallel_for(size_t{0}, meshes.size(), [&](size_t i) { meshes[i].getCellLocator(); });
}

}  // namespace

//---------------------------------------------------------------------------
double ShapeEvaluation::compute_compactness(const ParticleSystemEvaluation& particle_system, const int num_modes,
                                            const std::string& save_to) {
  const int n = particle_system.num_samples();
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
  TIME_SCOPE("ShapeEvaluation::compute_full_compactness");
  const int n = particle_system.num_samples();
  const int d = particle_system.num_dims();
  const int num_modes = n - 1;  // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  Eigen::MatrixXd y = particle_system.get_matrix();
  const Eigen::VectorXd mu = y.rowwise().mean();
  y.colwise() -= mu;

  Eigen::BDCSVD<Eigen::MatrixXd> svd(y);
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
double ShapeEvaluation::compute_generalization(const ParticleSystemEvaluation& particle_system, const int num_modes,
                                               const std::string& save_to, bool surface_distance_mode) {
  TIME_SCOPE("ShapeEvaluation::compute_generalization");
  const long n = particle_system.num_samples();
  const long d = particle_system.num_dims();
  const Eigen::MatrixXd& p = particle_system.get_matrix();

  if (num_modes > n - 1) {
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  int num_values = particle_system.get_num_values_per_particle();

  const auto& meshes = particle_system.get_meshes();
  if (surface_distance_mode) {
    build_cell_locators(meshes);
  }

  const long num_particles = d / num_values;
  std::vector<double> dists(n);
  reconstructions.resize(n);

  // leave one out, each left-out shape is independent
  parallel_for_index(n, [&](long leave) {
    // Create a new matrix, `y` excluding the column to be left out, `leave`
    Eigen::MatrixXd y(d, n - 1);
    y.leftCols(leave) = p.leftCols(leave);
    y.rightCols(n - leave - 1) = p.rightCols(n - leave - 1);

    // Compute the mean of each row and centralize the data
    const Eigen::VectorXd mu = y.rowwise().mean();
    y.colwise() -= mu;
    // Define the test vector as the column that was left out (`leave`) of `p`
    const Eigen::VectorXd y_test = p.col(leave);

    // SVD of the centralized data. Only the thin U (the first n-1 left singular vectors) can be used.
    Eigen::BDCSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeThinU);
    // `epsi` contains the first `num_modes` columns of matrix `U` (leading singular vectors).
    const auto epsi = svd.matrixU().leftCols(num_modes);
    // Project the centralized test vector onto the leading singular vectors and map it back
    const Eigen::VectorXd betas = epsi.transpose() * (y_test - mu);
    const Eigen::VectorXd rec = epsi * betas + mu;

    // Reshape vectors and calculate distance measure
    const Eigen::Map<const RowMajorMatrix> y_test_reshaped(y_test.data(), num_particles, num_values);
    const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), num_particles, num_values);

    double dist = 0;
    if (surface_distance_mode) {
      // distance between each particle and the surface
      const Mesh& mesh = meshes[leave];
      for (int i = 0; i < num_particles; i++) {
        vtkIdType face_id = 0;
        double this_dist = 0;
        Point3 point;
        point[0] = rec_reshaped(i, 0);
        point[1] = rec_reshaped(i, 1);
        point[2] = rec_reshaped(i, 2);
        mesh.closestPoint(point, this_dist, face_id);
        dist += this_dist;
      }

    } else {
      // euclidean distance between the original and reconstructed
      dist = (rec_reshaped - y_test_reshaped).rowwise().norm().sum() / static_cast<double>(num_particles);
    }

    dists[leave] = dist;
    reconstructions[leave] = {dist, static_cast<int>(leave), rec_reshaped};
  });

  // sum in order so the result does not depend on scheduling
  double total_dist = 0.0;
  for (long leave = 0; leave < n; leave++) {
    total_dist += dists[leave];
  }
  const double generalization = total_dist / static_cast<double>(n);

  // Save the reconstructions if needed. Generates XML files that can be opened in
  // ShapeWorksView2
  if (!save_to.empty()) {
    SaveReconstructions(reconstructions, particle_system.get_paths(), save_to);
  }

  return generalization;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::compute_full_generalization(const ParticleSystemEvaluation& particle_system,
                                                             std::function<void(float)> progress_callback,
                                                             std::function<bool()> check_abort,
                                                             bool surface_distance_mode) {
  TIME_SCOPE("ShapeEvaluation::compute_full_generalization");
  const long n = particle_system.num_samples();  // number of samples
  const long d = particle_system.num_dims();     // number of dimensions (e.g. number of particles * 3)
  const Eigen::MatrixXd& p = particle_system.get_matrix();

  if (n <= 1) {
    return {};
  }

  Eigen::VectorXd generalizations(n - 1);

  const int num_values = particle_system.get_num_values_per_particle();
  const long num_particles = d / num_values;

  const auto& meshes = particle_system.get_meshes();
  if (surface_distance_mode) {
    build_cell_locators(meshes);
  }

  // dists(mode - 1, leave) is the error reconstructing left-out shape `leave` from `mode` modes
  Eigen::MatrixXd dists(n - 1, n);

  CallerThreadCallbacks callbacks(progress_callback, check_abort, n);

  // leave one out, each left-out shape is independent
  parallel_for_index(n, [&](long leave) {
    if (callbacks.aborted()) {
      return;
    }
    Eigen::MatrixXd y(d, n - 1);
    y.leftCols(leave) = p.leftCols(leave);
    y.rightCols(n - leave - 1) = p.rightCols(n - leave - 1);

    const Eigen::VectorXd mu = y.rowwise().mean();
    y.colwise() -= mu;
    const Eigen::VectorXd y_test = p.col(leave);

    // Only the thin U (at most n-1 left singular vectors) is ever used
    Eigen::BDCSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeThinU);
    const Eigen::MatrixXd& u = svd.matrixU();

    // Project onto all modes once, then grow the reconstruction one mode at a time
    const Eigen::VectorXd betas = u.transpose() * (y_test - mu);
    Eigen::VectorXd rec = mu;

    const Eigen::Map<const RowMajorMatrix> ytest_reshaped(y_test.data(), num_particles, num_values);
    const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), num_particles, num_values);

    for (long mode = 1; mode < n; mode++) {
      if (callbacks.aborted()) {
        return;
      }
      if (mode <= u.cols()) {
        rec += betas(mode - 1) * u.col(mode - 1);
      }

      double dist = 0;
      if (surface_distance_mode) {
        const Mesh& mesh = meshes[leave];
        for (int i = 0; i < num_particles; i++) {
          vtkIdType face_id = 0;
          double this_dist = 0;
          Point3 point;
          point[0] = rec_reshaped(i, 0);
          point[1] = rec_reshaped(i, 1);
          point[2] = rec_reshaped(i, 2);
          mesh.closestPoint(point, this_dist, face_id);
          dist += this_dist / num_particles;
        }
      } else {
        dist = (rec_reshaped - ytest_reshaped).rowwise().norm().sum() / num_particles;
      }
      dists(mode - 1, leave) = dist;
    }
    callbacks.complete_one();
  });

  if (callbacks.was_aborted()) {
    return {};
  }

  // sum in order so the result does not depend on scheduling
  Eigen::VectorXd total_dists = Eigen::VectorXd::Zero(n - 1);
  for (long leave = 0; leave < n; leave++) {
    total_dists += dists.col(leave);
  }

  generalizations = total_dists / n;

  return generalizations;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::compute_specificity(const ParticleSystemEvaluation& particle_system, const int num_modes,
                                            const std::string& save_to, bool surface_distance_mode) {
  TIME_SCOPE("ShapeEvaluation::compute_specificity");
  const long n = particle_system.num_samples();
  const long d = particle_system.num_dims();
  int num_values = particle_system.get_num_values_per_particle();
  const auto& meshes = particle_system.get_meshes();

  if (num_modes > n - 1) {
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  if (surface_distance_mode && static_cast<long>(meshes.size()) != n) {
    throw std::invalid_argument("Surface distance mode requires one mesh per sample");
  }
  const int num_samples = 1000;

  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  Eigen::VectorXd mean_specificity(num_modes);
  Eigen::VectorXd std_specificity(num_modes);
  Eigen::MatrixXd spec_store(num_modes, 4);

  // PCA calculations
  const Eigen::MatrixXd& pts_models = particle_system.get_matrix();
  const Eigen::VectorXd mu = pts_models.rowwise().mean();
  Eigen::MatrixXd y = pts_models;

  y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeThinU);
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

    const long num_particles = d / num_values;
    const long num_train = pts_models.cols();

    Eigen::VectorXd distance_to_closest_training_sample(num_samples);

    for (int i = 0; i < num_samples; i++) {
      Eigen::VectorXd pts_m = sampling_points.col(i);
      Eigen::MatrixXd pts_distance(Eigen::MatrixXd::Constant(1, num_train, 0.0));

      if (surface_distance_mode) {
        // sum of the distances from each sampled particle to each original subject's surface
        for (int j = 0; j < n; j++) {
          const Mesh& mesh = meshes[j];
          for (int k = 0; k < num_particles; k++) {
            vtkIdType face_id = 0;
            double this_dist = 0;
            Point3 point;
            point[0] = pts_m(k * num_values + 0);
            point[1] = pts_m(k * num_values + 1);
            point[2] = pts_m(k * num_values + 2);
            mesh.closestPoint(point, this_dist, face_id);
            pts_distance(j) += this_dist;
          }
        }
      } else {
        Eigen::MatrixXd pts_distance_vec = pts_models.colwise() - pts_m;
        for (int j = 0; j < num_train; j++) {
          Eigen::Map<const RowMajorMatrix> pts_distance_vec_reshaped(pts_distance_vec.col(j).data(), num_particles,
                                                                     num_values);
          pts_distance(j) = (pts_distance_vec_reshaped).rowwise().norm().sum();
        }
      }

      int closest_idx, r;
      distance_to_closest_training_sample(i) = pts_distance.minCoeff(&r, &closest_idx);

      Eigen::Map<const RowMajorMatrix> pts_m_reshaped(pts_m.data(), num_particles, num_values);
      reconstructions.push_back(Reconstruction{
          distance_to_closest_training_sample(i),
          closest_idx,
          pts_m_reshaped,
      });
    }

    mean_specificity(mode_number) = distance_to_closest_training_sample.mean();
  }

  if (!save_to.empty()) {
    SaveReconstructions(reconstructions, particle_system.get_paths(), save_to);
  }

  const long num_particles = d / num_values;
  const double specificity = mean_specificity(num_modes - 1) / static_cast<double>(num_particles);

  return specificity;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::compute_full_specificity(const ParticleSystemEvaluation& particle_system,
                                                          std::function<void(float)> progress_callback,
                                                          std::function<bool()> check_abort,
                                                          bool surface_distance_mode) {
  TIME_SCOPE("ShapeEvaluation::compute_full_specificity");
  const long n = particle_system.num_samples();
  const long d = particle_system.num_dims();
  const int num_values = particle_system.get_num_values_per_particle();
  const int num_particles = d / num_values;

  Eigen::VectorXd specificities(n - 1);

  // PCA calculations
  const Eigen::MatrixXd& pts_models = particle_system.get_matrix();
  const long num_train = pts_models.cols();

  const Eigen::VectorXd mu = pts_models.rowwise().mean();
  Eigen::MatrixXd y = pts_models;
  y.colwise() -= mu;
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(y, Eigen::ComputeThinU);
  const auto all_eigen_values = svd.singularValues();

  const auto& meshes = particle_system.get_meshes();
  if (surface_distance_mode) {
    build_cell_locators(meshes);
  }

  CallerThreadCallbacks callbacks(nullptr, check_abort, 0);

  for (int num_modes = 1; num_modes < n; num_modes++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(num_modes) / static_cast<float>(n));
    }

    const int num_samples = 1000;

    const auto eigen_values = all_eigen_values.head(num_modes);
    const auto epsi = svd.matrixU().block(0, 0, d, num_modes);

    MultiVariateNormalRandom sampling{eigen_values.asDiagonal()};
    const Eigen::MatrixXd sampling_betas = sampling.sample(num_samples);

    const Eigen::MatrixXd sampling_points = (epsi * sampling_betas).colwise() + mu;
    Eigen::VectorXd distance_to_closest_training_sample(num_samples);

    // the samples are drawn above, so the distance computations are independent
    parallel_for_index(num_samples, [&](long i) {
      if (callbacks.aborted()) {
        return;
      }

      const Eigen::VectorXd pts_m = sampling_points.col(i);
      Eigen::VectorXd pts_distance = Eigen::VectorXd::Zero(num_train);

      if (surface_distance_mode) {
        // sum the distances to the surface for each original subject
        for (size_t j = 0; j < meshes.size(); ++j) {
          const Mesh& mesh = meshes[j];
          double total_dist = 0.0;
          for (int k = 0; k < num_particles; k++) {
            vtkIdType face_id = 0;
            double this_dist = 0;
            Point3 point;
            point[0] = pts_m(k * num_values + 0);
            point[1] = pts_m(k * num_values + 1);
            point[2] = pts_m(k * num_values + 2);
            mesh.closestPoint(point, this_dist, face_id);
            total_dist = total_dist + this_dist;
          }
          pts_distance(j) = total_dist;
        }
      } else {
        Eigen::VectorXd diff(d);
        for (long j = 0; j < num_train; j++) {
          diff.noalias() = pts_models.col(j) - pts_m;
          const Eigen::Map<const RowMajorMatrix> diff_reshaped(diff.data(), num_particles, num_values);
          pts_distance(j) = diff_reshaped.rowwise().norm().sum();
        }
      }

      distance_to_closest_training_sample(i) = pts_distance.minCoeff();
    });

    if (callbacks.was_aborted()) {
      return {};
    }

    double mean_specificity = distance_to_closest_training_sample.mean();
    const double specificity = mean_specificity / num_particles;
    specificities(num_modes - 1) = specificity;
  }
  return specificities;
}

}  // namespace shapeworks
