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
Eigen::VectorXd ShapeEvaluation::ComputeFullCompactnessNew(const Eigen::MatrixXd &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();
  const int D = particleSystem.rows();
  const int num_modes = N-1; // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  auto Y = particleSystem;
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
Eigen::MatrixXd ShapeEvaluation::ComputeFullCompactnessWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();
  const int num_modes = N-1; // the number of modes is one less than the number of samples
  if (num_modes < 1) {
    return Eigen::MatrixXd();
  }
  unsigned int dps = num_particles_ar.size();
  std::cout << " dps = " << dps << " in within evaluate " << std::endl;

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in CompactnessWithin" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);

  Eigen::MatrixXd within_compactness_all;
  within_compactness_all.resize(num_modes, dps);

  for(unsigned int k = 0; k < dps; k++){
    unsigned int D = within_objectives[k].rows();
    Eigen::MatrixXd Y = within_objectives[k];
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
    within_compactness_all.col(k) = cumsum; 
  }

  return within_compactness_all;
}


//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullCompactnessBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();
  const int num_modes = N-1; // the number of modes is one less than the number of samples
  if (num_modes < 1) {
    return Eigen::VectorXd();
  }

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in CompactnessBetween" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);
 

  const int D = between_objective_all.rows();
  Eigen::MatrixXd Y = between_objective_all;
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


Eigen::VectorXd ShapeEvaluation::ComputeFullGeneralizationNew(const Eigen::MatrixXd &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.rows();
  const int D = particleSystem.cols();
  auto P = particleSystem;

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
Eigen::MatrixXd ShapeEvaluation::ComputeFullGeneralizationWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();

  if (N <= 1) {
    return Eigen::VectorXd();
  }

  unsigned int dps = num_particles_ar.size();
  std::cout << " dps = " << dps << " in within evaluate " << std::endl;

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in Generalization Between" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);
  
  
  Eigen::MatrixXd within_gen_all;
  within_gen_all.resize(N-1, dps);

  for(unsigned int k = 0; k < dps; k++){
    Eigen::VectorXd generalizations(N-1);
    unsigned int D = within_objectives[k].rows();
    Eigen::MatrixXd P = within_objectives[k];
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
    within_gen_all.col(k) = generalizations;
  }
  return within_gen_all;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullGeneralizationBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();

  if (N <= 1) {
    return Eigen::VectorXd();
  }

  unsigned int dps = num_particles_ar.size();
  std::cout << " dps = " << dps << " in within evaluate " << std::endl;

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in Generalization Between" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);

  unsigned int D = between_objective_all.rows();
  Eigen::MatrixXd P = between_objective_all;


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

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullSpecificityNew(const Eigen::MatrixXd &particleSystem, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.rows();
  const int D = particleSystem.cols();
  const int numParticles = D / VDimension;

  Eigen::VectorXd specificities(N-1);

  // PCA calculations
  // const Eigen::MatrixXd &ptsModels = particleSystem;
  auto ptsModels = particleSystem;

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

//---------------------------------------------------------------------------
Eigen::MatrixXd ShapeEvaluation::ComputeFullSpecificityWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();

  if (N <= 1) {
    return Eigen::VectorXd();
  }

  unsigned int dps = num_particles_ar.size();
  std::cout << " dps = " << dps << " in within evaluate " << std::endl;

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in Generalization Between" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);
  
  
  Eigen::MatrixXd within_spec_all;
  within_spec_all.resize(N-1, dps);

  for(unsigned int k = 0; k < dps; k++){
    unsigned int D = within_objectives[k].rows();
    unsigned int numParticles = D / VDimension;
    Eigen::VectorXd specificities(N-1);

    // PCA calculations
    Eigen::MatrixXd ptsModels = within_objectives[k];


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
    within_spec_all.col(k) = specificities;
  }
  return within_spec_all;
}


//---------------------------------------------------------------------------
Eigen::VectorXd ShapeEvaluation::ComputeFullSpecificityBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.cols();

  if (N <= 1) {
    return Eigen::VectorXd();
  }
  unsigned int dps = num_particles_ar.size();
  std::cout << " dps = " << dps << " in within evaluate " << std::endl;

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;
  std::cout << "Before  - DoMultiLevelModeling in Generalization Between" << std::endl;
  ShapeEvaluation::DoMultiLevelModeling(particleSystem, num_particles_ar, within_objectives, between_objective_all);

  unsigned int D = between_objective_all.rows();
  Eigen::MatrixXd ptsModels = between_objective_all;
  const int numParticles = D / VDimension;

  Eigen::VectorXd specificities(N-1);

  // PCA calculations
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

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void ShapeEvaluation::DoMultiLevelModeling(const Eigen::MatrixXd &shape_matrix, const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives, Eigen::MatrixXd& between_objectives)
{
  std::cout << "Start - DoMultiLevelModeling" << std::endl;

  unsigned int dps = num_particles.size();

  std::cout << "dps is " << dps << std::endl;

  unsigned int N  = shape_matrix.cols(); // num_samples
  unsigned int M =  shape_matrix.rows() / 3; // Total particles M 

  std::cout << "size  is " << M << " X " << N << std::endl;

  Eigen::MatrixXd super_matrix;
  super_matrix.resize(M, 3*N);

  std::cout << "constant matrix Built" << std::endl;

  for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < M; j++){
            super_matrix(j, i*3) = shape_matrix(j*3, i);
            super_matrix(j, i*3 + 1) = shape_matrix(j*3 + 1, i);
            super_matrix(j, i*3 + 2) = shape_matrix(j*3 + 2, i);
        }
  }

  // 2. Compute Grand Mean
  Eigen::MatrixXd grand_mean(3, N); // --> 3 X N;
  Eigen::MatrixXd  ones_M = Eigen::MatrixXd::Constant(M, 1, 1.0); // M X 1
  Eigen::MatrixXd  grand_mean_temp = (ones_M.transpose() * super_matrix) * (1.0/((double)M)); // --> 1 X 3N

  grand_mean_temp.transposeInPlace(); //3N X 1
  for(unsigned int i = 0; i < N; i++){
        grand_mean(0, i) = grand_mean_temp(i*3, 0); 
        grand_mean(1, i) = grand_mean_temp(i*3 + 1, 0); 
        grand_mean(2, i) = grand_mean_temp(i*3 + 2, 0); 
  }

  std::cout << "Before  - ComputeWithinTerms" << std::endl;

  ShapeEvaluation::ComputeWithinTerms(super_matrix, num_particles, within_objectives);
  
  std::cout << "After  - ComputeWithinTerms" << std::endl;
  
  ShapeEvaluation::ComputeBetweenTerms(super_matrix, num_particles, between_objectives);
  
  std::cout << "After  - ComputeBetweenTerms" << std::endl;
  
}

//---------------------------------------------------------------------------
void ShapeEvaluation::ComputeWithinTerms(Eigen::MatrixXd &super_matrix, const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives)
{
  within_objectives.clear();
  unsigned int N = (int)(super_matrix.cols() / VDimension);
  unsigned int dps = num_particles.size();
  for(unsigned int k = 0; k < dps; k++)
  {
    Eigen::MatrixXd centering_matrix(num_particles[k], num_particles[k]); // Build centering matrix for within
    centering_matrix.setIdentity();
    Eigen::MatrixXd ones_m_k = Eigen::MatrixXd::Constant(num_particles[k], 1, 1.0);
    centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)num_particles[k]));
    unsigned int row = 0;
    for (unsigned int x = 0; x < k; x++)
    {
      row += num_particles[x];
    }
    Eigen::MatrixXd  z_k = super_matrix.block(row, 0, num_particles[k], VDimension * N);                                                              // k * num_particles[k]
                            // Get shape matrix of the kth organ
    Eigen::MatrixXd z_within_k = centering_matrix.transpose() * z_k; // m  X 3N

    // Compute Svd obejctive --> 3m X N
    Eigen::MatrixXd z_within_k_objective(VDimension * num_particles[k], N);
    for (unsigned int i = 0; i < N; i++)
    {
      for (unsigned int j = 0; j < num_particles[k]; j++)
      {
        unsigned int r = j * VDimension;
        z_within_k_objective(j * VDimension, i) = z_within_k(j, i * VDimension);
        z_within_k_objective(j * VDimension + 1, i) =  z_within_k(j, i * VDimension + 1);
        z_within_k_objective(j * VDimension + 2, i) =  z_within_k(j, i * VDimension + 2);
      }
    }
    within_objectives.push_back(z_within_k_objective);
  }
}

//---------------------------------------------------------------------------
void ShapeEvaluation::ComputeBetweenTerms(Eigen::MatrixXd &super_matrix, const std::vector<int>& num_particles, Eigen::MatrixXd& between_objectives)
{
  std::cout << "starting between terms" << std::endl;
  unsigned int dps = num_particles.size();
  unsigned int M =  std::accumulate(num_particles.begin(), num_particles.end(), 0);
  unsigned int N = (int)(super_matrix.cols() / VDimension);
  //PART A: Compute deviations of COM of each organ from COM of all the organs
  Eigen::MatrixXd between_centering_matrix(M, M);// Build centering matrix for between
  between_centering_matrix.setIdentity();
  Eigen::MatrixXd ones_m = Eigen::MatrixXd::Constant(M, 1, 1.0);
  between_centering_matrix = between_centering_matrix - (ones_m * ones_m.transpose()) * (1.0 / ((double)M));
  std::cout << " M = " << M << " N = " << N << std::endl;
  std::cout << "between_centering_matrix done Size = " << between_centering_matrix.rows() << " X " << between_centering_matrix.cols() <<  std::endl;
  
  Eigen::MatrixXd z_centered = between_centering_matrix * super_matrix; // M * 3N
  std::cout << "CENTERING DONE size = " << z_centered.rows() << " X " << z_centered.cols() << std::endl;

  Eigen::MatrixXd z_between(dps, VDimension * N);
  for (unsigned int k = 0; k < dps; k++)
  {
    Eigen::MatrixXd z_centered_k(num_particles[k], VDimension * N); // m X 3N
    unsigned int row = 0;
    for (unsigned int x = 0; x < k; x++)
    {
      row += num_particles[x];
    }                                         // k * num_particles[k]
    z_centered_k = z_centered.block(row, 0, num_particles[k], VDimension * N); // extract sub-matrix for kth organ from centred matrix and Center that sub-matrix(for between)
    std::cout << "z_centred_k extract done k = " << k << std::endl;
    Eigen::MatrixXd ones_m_k = Eigen::MatrixXd::Constant(num_particles[k], 1, 1.0);
    Eigen::MatrixXd z_temp = (ones_m_k.transpose() * z_centered_k) * (1.0 / ((double)num_particles[k])); // --> 1 X 3N
    std::cout << "z_temp extract done k = " << k << std::endl;
    std::cout << "z_temp extract done k = " << k << std::endl;
    std::cout << "size = " << z_temp.rows() << " X " << z_temp.cols() <<  std::endl;


    z_between.block(k, 0, 1, VDimension*N) = z_temp;
  }

  // tranform K X 3N --> 3K X N
  Eigen::MatrixXd z_between_objective_temp(VDimension * dps, N); // ---> 3K X N
  for (unsigned int k = 0; k < dps; k++)
  {
    for (unsigned int i = 0; i < N; i++)
    {
      z_between_objective_temp(k * VDimension, i) = z_between(k, i * VDimension);
      z_between_objective_temp(k * VDimension + 1, i) = z_between(k, i * VDimension + 1);
      z_between_objective_temp(k * VDimension + 2, i) = z_between(k, i * VDimension + 2);
    }
  }
  Eigen::MatrixXd z_between_objective = z_between_objective_temp; //---> 3K X N
  std::cout << " between objective set 1| size = " << z_between_objective.rows() << " X "  << z_between_objective.cols() << std::endl;
  between_objectives = (z_between_objective);
  std::cout << " between objective set 2| size = " << between_objectives.rows() << " X "  << between_objectives.cols() << std::endl;
  std::cout << " between objective set 2" << std::endl;

}


} // shapeworks

