#include "ShapeEvaluation.h"
#include "EvaluationUtil.h"

#include <iostream>
#include <Eigen/Core>
#include <Eigen/SVD>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

namespace shapeworks {

  //---------------------------------------------------------------------------
double ShapeEvaluation::ComputeCompactness(const ParticleSystem &particleSystem, const int nModes,
                                                       const std::string &saveTo)
{
  const int N = particleSystem.N();
  if (nModes > N-1){
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  Eigen::VectorXd cumsum = ShapeEvaluation::ComputeFullCompactness(particleSystem);

  if (!saveTo.empty()) {
    std::ofstream of(saveTo);
    of << cumsum;
    of.close();
  }

  return cumsum(nModes - 1);
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullCompactness(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const int num_modes = N-1; // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  Eigen::MatrixXd Y = particleSystem.Particles();
  const Eigen::VectorXd mu = Y.rowwise().mean();
  Y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
  const auto S = svd.singularValues().array().pow(2) / (N * D);

  // Compute cumulative sum
  Eigen::VectorXd cumsum(num_modes);
  cumsum(0) = S(0);
  for (int i = 1; i < num_modes; i++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(i) / static_cast<float>(N));
    }
    cumsum(i) = cumsum(i-1) + S(i);
  }
  cumsum /= S.sum();
  return cumsum;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes,
                                                          const std::string &saveTo)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const Eigen::MatrixXd &P = particleSystem.Particles();

  if (nModes > N-1){
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  double totalDist = 0.0;
  for (int leave = 0; leave < N; leave++) {

    Eigen::MatrixXd Y(D, N - 1);
    Y.leftCols(leave) = P.leftCols(leave);
    Y.rightCols(N - leave - 1) = P.rightCols(N - leave - 1);

    const Eigen::VectorXd mu = Y.rowwise().mean();
    Y.colwise() -= mu;
    const Eigen::VectorXd Ytest = P.col(leave);

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
    const auto epsi = svd.matrixU().block(0, 0, D, nModes);
    const auto betas = epsi.transpose() * (Ytest - mu);
    const Eigen::VectorXd rec = epsi * betas + mu;

    const int numParticles = D / VDimension;
    const Eigen::Map<const RowMajorMatrix> Ytest_reshaped(Ytest.data(), numParticles, VDimension);
    const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), numParticles, VDimension);
    const double dist = (rec_reshaped - Ytest_reshaped).rowwise().norm().sum() / numParticles;
    totalDist += dist;

    reconstructions.push_back({dist, leave, rec_reshaped});
  }
  const double generalization = totalDist / N;

  // Save the reconstructions if needed. Generates XML files that can be opened in
  // ShapeWorksView2
  if (!saveTo.empty()) {
    SaveReconstructions(reconstructions, particleSystem.Paths(), saveTo);
  }

  return generalization;
}

Eigen::VectorXd ShapeEvaluation::ComputeFullGeneralization(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const Eigen::MatrixXd &P = particleSystem.Particles();

  if (N <= 1) {
    return Eigen::VectorXd();
  }

  Eigen::VectorXd generalizations(N-1);

  Eigen::VectorXd totalDists = Eigen::VectorXd::Zero(N-1);

  for (int leave = 0; leave < N; leave++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(leave) / static_cast<float>(N));
    }
    Eigen::MatrixXd Y(D, N - 1);
    Y.leftCols(leave) = P.leftCols(leave);
    Y.rightCols(N - leave - 1) = P.rightCols(N - leave - 1);

    const Eigen::VectorXd mu = Y.rowwise().mean();
    Y.colwise() -= mu;
    const Eigen::VectorXd Ytest = P.col(leave);

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);

    for (int mode = 1; mode < N; mode++) {

      const auto epsi = svd.matrixU().block(0, 0, D, mode);
      const auto betas = epsi.transpose() * (Ytest - mu);
      const Eigen::VectorXd rec = epsi * betas + mu;

      const int numParticles = D / VDimension;
      const Eigen::Map<const RowMajorMatrix> Ytest_reshaped(Ytest.data(), numParticles, VDimension);
      const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), numParticles, VDimension);
      const double dist = (rec_reshaped - Ytest_reshaped).rowwise().norm().sum() / numParticles;
      totalDists(mode-1) += dist;
    }
  }

  generalizations = totalDists / N;

  return generalizations;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes,
                                                       const std::string &saveTo)
{

  const int N = particleSystem.N();
  const int D = particleSystem.D();

  if (nModes > N-1){
    throw std::invalid_argument("Invalid mode of variation specified");
  }
  const int nSamples = 1000;

  // Keep track of the reconstructions so we can visualize them later
  std::vector<Reconstruction> reconstructions;

  Eigen::VectorXd meanSpecificity(nModes);
  Eigen::VectorXd stdSpecificity(nModes);
  Eigen::MatrixXd spec_store(nModes, 4);

  // PCA calculations
  const Eigen::MatrixXd &ptsModels = particleSystem.Particles();
  const Eigen::VectorXd mu = ptsModels.rowwise().mean();
  Eigen::MatrixXd Y = ptsModels;

  Y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
  const auto epsi = svd.matrixU().block(0, 0, D, nModes);
  const auto allEigenValues = svd.singularValues();
  const auto eigenValues = allEigenValues.head(nModes);

  Eigen::MatrixXd samplingBetas(nModes, nSamples);
  MultiVariateNormalRandom sampling{eigenValues.asDiagonal()};
  for (int modeNumber = 0; modeNumber < nModes; modeNumber++) {
    for (int i = 0; i < nSamples; i++) {
      samplingBetas.col(i) = sampling();
    }

    Eigen::MatrixXd samplingPoints = (epsi * samplingBetas).colwise() + mu;

    const int numParticles = D / VDimension;
    const int nTrain = ptsModels.cols();

    Eigen::VectorXd distanceToClosestTrainingSample(nSamples);

    for (int i = 0; i < nSamples; i++) {

      Eigen::VectorXd pts_m = samplingPoints.col(i);
      Eigen::MatrixXd ptsDistance_vec = ptsModels.colwise() - pts_m;
      Eigen::MatrixXd ptsDistance(Eigen::MatrixXd::Constant(1, nTrain, 0.0));

      for (int j = 0; j < nTrain; j++) {
        Eigen::Map<const RowMajorMatrix> ptsDistance_vec_reshaped(ptsDistance_vec.col(j).data(), numParticles,
                                                                  VDimension);
        ptsDistance(j) = (ptsDistance_vec_reshaped).rowwise().norm().sum();
      }

      int closestIdx, _r;
      distanceToClosestTrainingSample(i) = ptsDistance.minCoeff(&_r, &closestIdx);

      Eigen::Map<const RowMajorMatrix> pts_m_reshaped(pts_m.data(), numParticles, VDimension);
      reconstructions.push_back(Reconstruction{
              distanceToClosestTrainingSample(i),
              (int) closestIdx,
              pts_m_reshaped,
      });
    }

    meanSpecificity(modeNumber) = distanceToClosestTrainingSample.mean();
  }

  if (!saveTo.empty()) {
    SaveReconstructions(reconstructions, particleSystem.Paths(), saveTo);
  }

  const int numParticles = D / VDimension;
  const double specificity = meanSpecificity(nModes - 1) / numParticles;

  return specificity;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullSpecificity(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const int numParticles = D / VDimension;

  Eigen::VectorXd specificities(N-1);

  // PCA calculations
  const Eigen::MatrixXd &ptsModels = particleSystem.Particles();
  const int nTrain = ptsModels.cols();

  const Eigen::VectorXd mu = ptsModels.rowwise().mean();
  Eigen::MatrixXd Y = ptsModels;
  Y.colwise() -= mu;
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
  const auto allEigenValues = svd.singularValues();

  for (int nModes=1;nModes<N;nModes++) {
    if (progress_callback) {
      progress_callback(static_cast<float>(nModes) / static_cast<float>(N));
    }

    const int nSamples = 1000;

    Eigen::VectorXd stdSpecificity(nModes);
    Eigen::MatrixXd spec_store(nModes, 4);
    const auto eigenValues = allEigenValues.head(nModes);
    const auto epsi = svd.matrixU().block(0, 0, D, nModes);


    Eigen::MatrixXd samplingBetas(nModes, nSamples);
    MultiVariateNormalRandom sampling{eigenValues.asDiagonal()};
    for (int i = 0; i < nSamples; i++) {
      samplingBetas.col(i) = sampling();
    }

    Eigen::MatrixXd samplingPoints = (epsi * samplingBetas).colwise() + mu;
    Eigen::VectorXd distanceToClosestTrainingSample(nSamples);

    for (int i = 0; i < nSamples; i++) {

      Eigen::VectorXd pts_m = samplingPoints.col(i);
      Eigen::MatrixXd ptsDistance_vec = ptsModels.colwise() - pts_m;
      Eigen::MatrixXd ptsDistance(Eigen::MatrixXd::Constant(1, nTrain, 0.0));

      for (int j = 0; j < nTrain; j++) {
        Eigen::Map<const RowMajorMatrix> ptsDistance_vec_reshaped(ptsDistance_vec.col(j).data(), numParticles,
                                                                  VDimension);
        ptsDistance(j) = (ptsDistance_vec_reshaped).rowwise().norm().sum();
      }

      int closestIdx, _r;
      distanceToClosestTrainingSample(i) = ptsDistance.minCoeff(&_r, &closestIdx);
    }

    double meanSpecificity = distanceToClosestTrainingSample.mean();
    const double specificity = meanSpecificity / numParticles;
    specificities(nModes-1) = specificity;
  }
  return specificities;
}

} // shapeworks

