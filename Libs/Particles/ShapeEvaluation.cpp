#include "ShapeEvaluation.h"
#include "EvaluationUtil.h"

#include <iostream>
#include <Eigen/Core>
#include <Eigen/SVD>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

namespace shapeworks {
template<unsigned int VDimension>
double ShapeEvaluation<VDimension>::ComputeCompactness(const ParticleSystem &particleSystem, const int nModes,
                                                       const std::string &saveScreePlotTo)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();

  std::cerr << "N = " << N << "\n";
  std::cerr << "D = " << D << "\n";
  Eigen::MatrixXd Y = particleSystem.Particles();
  const Eigen::VectorXd mu = Y.rowwise().mean();
  Y.colwise() -= mu;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
  const auto S = svd.singularValues().array().pow(2) / (N * D);

  // Compute cumulative sum
  Eigen::VectorXd cumsum(N);
  cumsum(0) = S(0);
  for (int i = 1; i < N; i++) {
    cumsum(i) = cumsum(i - 1) + S(i);
  }
  cumsum /= S.sum();

  if (!saveScreePlotTo.empty()) {
    std::ofstream of(saveScreePlotTo);
    of << cumsum;
    of.close();
  }

  return cumsum(nModes - 1);
}

template<unsigned int VDimension>
double ShapeEvaluation<VDimension>::ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes,
                                                          const std::string &saveTo)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const Eigen::MatrixXd &P = particleSystem.Particles();

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

template<unsigned int VDimension>
double ShapeEvaluation<VDimension>::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes,
                                                       const std::string &saveTo)
{

  const int N = particleSystem.N();
  const int D = particleSystem.D();

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

template
class ShapeEvaluation<3>;
}

