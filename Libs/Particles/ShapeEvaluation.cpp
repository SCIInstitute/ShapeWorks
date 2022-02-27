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
Eigen::VectorXd ShapeEvaluation::ComputeFullCompactnessInWithinSubspace(const Eigen::MatrixXd &WithinMatrix, std::function<void(float)> progress_callback)
{
  const int N = WithinMatrix.cols();
  const int D = WithinMatrix.rows();
  const int num_modes = N-1; // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  Eigen::MatrixXd Y = WithinMatrix;

  // Compute Within subspace here
  
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
Eigen::VectorXd ShapeEvaluation::ComputeFullCompactnessInBetweenSubspace(const Eigen::MatrixXd &BetweenMatrix, std::function<void(float)> progress_callback)
{
  const int N = BetweenMatrix.cols();
  const int D = BetweenMatrix.rows();
  const int num_modes = N-1; // the number of modes is one less than the number of samples

  if (num_modes < 1) {
    return Eigen::VectorXd();
  }
  Eigen::MatrixXd Y = BetweenMatrix;

  // Compute Within subspace here
  
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


Eigen::VectorXd ShapeEvaluation::ComputeFullGeneralizationMultiLevel(const ParticleSystem &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  std::cout << "Start - ComputeFullGeneralizationMultiLevel " << std::endl;
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  const Eigen::MatrixXd &P = particleSystem.Particles();
  unsigned int dps = num_particles_ar.size();
  if (N <= 1)
  {
    return Eigen::VectorXd();
  }
  Eigen::VectorXd generalizations(N - 1);
  Eigen::MatrixXd distMatrix = Eigen::MatrixXd::Zero(N-1, N-1); // To compute the variance and mean of generalization for a mode
  Eigen::VectorXd totalDists = Eigen::VectorXd::Zero(N - 1);

  for (int leave = 0; leave < N; leave++)
  {
    if (progress_callback)
    {
      progress_callback(static_cast<float>(leave) / static_cast<float>(N));
    }

    // Do Multi-level Modeling for the given Particle System
    std::vector<Eigen::MatrixXd> within_objectives;
    Eigen::MatrixXd between_objective_all;
    std::cout << "Before  - DoMultiLevelModeling " << std::endl;

    ShapeEvaluation::DoMultiLevelModeling(particleSystem.Particles(), num_particles_ar, within_objectives, between_objective_all);
    std::cout << "After  - DoMultiLevelModeling " << std::endl;

    // Now Build Between part
    Eigen::MatrixXd beteween_objective(dps * VDimension, N - 1);
    beteween_objective.leftCols(leave) = between_objective_all.leftCols(leave);
    beteween_objective.rightCols(N - leave - 1) = between_objective_all.rightCols(N - leave - 1);
    const Eigen::VectorXd between_mean = beteween_objective.rowwise().mean();
    beteween_objective.colwise() -= between_mean;
    const Eigen::VectorXd test_between_part = between_objective_all.col(leave);
    Eigen::JacobiSVD<Eigen::MatrixXd> between_svd(Y, Eigen::ComputeFullU);
    std::vector<Eigen::VectorXd> between_rec_modes;
    for (int mode = 1; mode < N; mode++)
    {
      const auto between_epsi = between_svd.matrixU().block(0, 0, dps * VDimension, mode);
      const auto between_betas = between_epsi.transpose() * (test_between_part - between_mean);
      const Eigen::VectorXd between_rec = between_epsi * between_betas + between_mean;
      between_rec_modes.push_back(between_mean);
    }

    // Now for each organ compute, generalization by adding between and within part for each
    for (unsigned int k = 0; k < dps; k++)
    {
      unsigned int row = 0;
      unsigned int m = VDimension * num_particles_ar[k];
      for (unsigned int x = 0; x < k; x++)
      {
        row += num_particles_ar[x];
      }  
      Eigen::MatrixXd organ_k = P.block(row * VDimension, 0, num_particles_ar[k] * VDimension, N);
      Eigen::MatrixXd organ_k_Y(m, N-1);
      organ_k_Y.leftCols(leave) = organ_k.leftCols(leave);
      organ_k_Y.rightCols(N - leave - 1) = organ_k.rightCols(N - leave - 1);
      const Eigen::VectorXd mu = organ_k_Y.rowwise().mean();
      organ_k_Y.colwise() -= mu;
      const Eigen::VectorXd organ_k_test = organ_k.col(leave);


      Eigen::MatrixXd within_objective_k(m, N - 1);
      within_objective_k.leftCols(leave) = within_objectives[k].leftCols(leave);
      within_objective_k.rightCols(N - leave - 1) = within_objectives[k].rightCols(N - leave - 1);
      const Eigen::VectorXd within_mean = within_objective_k.rowwise().mean();
      within_objective_k.colwise() -= within_mean;
      const Eigen::VectorXd test_within_part_k = within_objectives[k].col(leave);
      Eigen::JacobiSVD<Eigen::MatrixXd> within_k_svd(within_objective_k, Eigen::ComputeFullU);
      for (int mode = 1; mode < N; mode++)
      {
        const auto within_epsi = within_k_svd.matrixU().block(0, 0, m, mode);
        const auto within_betas = within_epsi.transpose() * (test_within_part_k - within_mean);
        const Eigen::VectorXd within_rec = within_epsi * within_betas + within_mean;

        Eigen::VectorXd between_rec_k = Eigen::VectorXd::Zero(m);
        Eigen::VectorXd between_part_k_ext = between_rec_modes[mode].segment(VDimension * k, VDimension);
        const int numParticles = m / VDimension;
        for (unsigned int i = 0; i < numParticles; i++){
          between_rec_k(i * VDimension) = between_part_k_ext(0);
          between_rec_k(i * VDimension + 1) = between_part_k_ext(1);
          between_rec_k(i * VDimension + 2) = between_part_k_ext(2);

        }
        const Eigen::VectorXd organ_k_rec = within_rec + between_rec_k + mu;

        const Eigen::Map<const RowMajorMatrix> organ_k_test_reshaped(organ_k_test.data(), numParticles, VDimension);
        const Eigen::Map<const RowMajorMatrix> organ_K_rec_reshaped(organ_k_rec.data(), numParticles, VDimension);
        const double dist = (organ_K_rec_reshaped - organ_k_test_reshaped).rowwise().norm().sum() / numParticles;
        totalDists(mode - 1) += dist;
        distMatrix(leave, mode - 1) += dist;
      }
    }// end for all organs
    
  }// end for leave out loop
  generalizations = totalDists / (N * dps);
  distMatrix /= dps;
  return generalizations;
}

//---------------------------------------------------------------------------
double ShapeEvaluation::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes,
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
Eigen::VectorXd ShapeEvaluation::ComputeFullSpecificityMultiLevel(const ParticleSystem &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback)
{
  const int N = particleSystem.N();
  const int D = particleSystem.D();
  // const int numParticles = D / VDimension;
  unsigned int dps = num_particles_ar.size();


  Eigen::VectorXd specificities = Eigen::VectorXd::Zero(N-1);
  Eigen::VectorXd specificitiesMatrix = Eigen::VectorXd::Zero(1000, N-1);


  // PCA calculations
  const Eigen::MatrixXd &ptsModels = particleSystem.Particles();
  const int nTrain = ptsModels.cols();

  std::vector<Eigen::MatrixXd> within_objectives;
  Eigen::MatrixXd between_objective_all;



  //--*************************




  ///****************
  ShapeEvaluation::DoMultiLevelModeling(particleSystem.Particles(), num_particles_ar, within_objectives, between_objective_all);
  const Eigen::VectorXd between_mean = between_objective_all.rowwise().mean();
  between_objective_all.colwise() -= between_mean;
  Eigen::JacobiSVD<Eigen::MatrixXd> svd_between(between_objective_all, Eigen::ComputeFullU);
  const auto allEigenValues_between = svd_between.singularValues();

  for(unsigned int k = 0; k < dps; k++)
  {  
    unsigned int row = 0;
    unsigned int m = num_particles_ar[k] * VDimension;
    for (unsigned int x = 0; x < k; x++)
    {
      row += num_particles_ar[x];
    }  
    Eigen::MatrixXd organ_k_points = ptsModels.block(row * VDimension, 0, num_particles_ar[k] * VDimension, nTrain);
    const Eigen::VectorXd mu_k = organ_k_points.rowwise().mean();
    organ_k_points.colwise() -= mu_k;
    
    Eigen::MatrixXd within_objective_k = within_objectives[k];
    const Eigen::VectorXd within_mean = within_objective_k.rowwise().mean();
    within_objective_k.colwise() -= within_mean;
    Eigen::JacobiSVD<Eigen::MatrixXd> svd_within(within_objective_k, Eigen::ComputeFullU);
    const auto allEigenValues_within = svd_within.singularValues();

    for (int nModes=1;nModes<N;nModes++)
    {
      if (progress_callback) {
        progress_callback(static_cast<float>(nModes) / static_cast<float>(N));
      }
      const int nSamples = 1000;
      const auto eigenValues_within = allEigenValues_within.head(nModes);
      const auto epsi_within = svd_within.matrixU().block(0, 0, m, nModes); // m * nModes
      const auto eigenValues_between = allEigenValues_between.head(nModes);
      const auto epsi_between = svd_between.matrixU().block(0, 0, dps * VDimension, nModes); // Kd * nModes

      Eigen::MatrixXd samplingBetas_for_within(nModes, nSamples);
      Eigen::MatrixXd samplingBetas_for_between(nModes, nSamples); // nModes X 1000
      MultiVariateNormalRandom sampling_between{eigenValues_between.asDiagonal()};
      MultiVariateNormalRandom sampling_within{eigenValues_within.asDiagonal()};

      //Populate sampling betas
      for (int i = 0; i < nSamples; i++) {
        samplingBetas_for_between.col(i) = sampling_between();
        samplingBetas_for_within.col(i) = sampling_within();
      }

      Eigen::MatrixXd samplingPoints_within_k = (epsi_within * samplingBetas_for_within).colwise() + within_mean; //m X nModes  X  nModes * 1000 ---> m X 1000
      Eigen::MatrixXd samplingPoints_between_all = (epsi_between * samplingBetas_for_between).colwise() + between_mean; //Kd X nModes  X  nModes * 1000 ---> Kd X 1000
      Eigen::MatrixXd samplingPoints_between_k(m, nSamples);
      for (unsigned int x = 0; x < num_particles_ar[k]; x++){
        samplingPoints_between_k.block(VDimension*x, 0, VDimension, nSamples) = samplingPoints_between_all.block(k*VDimension, 0, VDimension, nSamples);
      }
      Eigen::MatrixXd samplingPoints_net_k = (samplingPoints_between_k + samplingPoints_within_k).colwise() + mu_k;
      Eigen::VectorXd distanceToClosestTrainingSample(nSamples);
      for (int i = 0; i < nSamples; i++)
      {
        Eigen::VectorXd pts_m_k = samplingPoints_net_k.col(i);
        Eigen::MatrixXd ptsDistance_vec_k = organ_k_points.colwise() - pts_m_k;
        Eigen::MatrixXd ptsDistance_k(Eigen::MatrixXd::Constant(1, nTrain, 0.0));
        for (int j = 0; j < nTrain; j++) {
          Eigen::Map<const RowMajorMatrix> ptsDistance_vec_reshaped_k(ptsDistance_vec_k.col(j).data(), num_particles_ar[k],
                                                                    VDimension);
          ptsDistance_k(j) = (ptsDistance_vec_reshaped_k).rowwise().norm().sum();
        }
        int closestIdx, _r;
        distanceToClosestTrainingSample(i) = ptsDistance_k.minCoeff(&_r, &closestIdx);

      }
      specificitiesMatrix.col(nModes-1) = distanceToClosestTrainingSample;
      double meanSpecificity = distanceToClosestTrainingSample.mean();
      const double specificity = meanSpecificity / num_particles_ar[k];
      specificities(nModes-1) += specificity; // For each mode add specificty for each organ
    }
  }
  specificities /= dps;
  specificitiesMatrix /= dps;
  return specificities;
}



//---------------------------------------------------------------------------
void ShapeEvaluation::DoMultiLevelModeling(const Eigen::MatrixXd &shape_matrix, const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives, Eigen::MatrixXd& between_objectives)
{
  std::cout << "Start - DoMultiLevelModeling" << std::endl;
  unsigned int dps = num_particles.size();
  std::cout << "dps is " << dps << std::endl;
  unsigned int N  = shape_matrix.cols(); // num_samples
  unsigned int M =  shape_matrix.rows() / 3; // Total particles
  std::cout << "size  is " << M << " X " << N << std::endl;

  Eigen::MatrixXd super_matrix;
  super_matrix.resize(M, 3*N);

  std::cout << "constant matrix Built" << std::endl;

  for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < M; j++){
        std::cout << "Inside init" << std::endl;
        std::cout << "val is i  = " << i << " j = "<< j << std::endl;
            super_matrix(j, i*3) = shape_matrix(j*3, i);
            super_matrix(j, i*3 + 1) = shape_matrix(j*3 + 1, i);
            super_matrix(j, i*3 + 2) = shape_matrix(j*3 + 2, i);
        }
  }
  std::cout << "super matrix Built" << std::endl;

  // 2. Compute Grand Mean
  Eigen::MatrixXd grand_mean(3, N); // --> 3 X N;
  Eigen::MatrixXd  ones_M = Eigen::MatrixXd::Constant(M, 1, 1.0); // M X 1
  Eigen::MatrixXd  grand_mean_temp = (ones_M.transpose() * super_matrix) * (1.0/((double)M)); // --> 1 X 3N
  std::cout << "grand mean transpose done" << std::endl;

  grand_mean_temp = grand_mean_temp.transpose(); // --> 3N X 1
  for(unsigned int i = 0; i < N; i++){
        grand_mean(0, i) = grand_mean_temp(i*3, 0); 
        grand_mean(1, i) = grand_mean_temp(i*3 + 1, 0); 
        grand_mean(2, i) = grand_mean_temp(i*3 + 2, 0); 
  }
  std::cout << "grand mean all done" << std::endl;


  // 3. Compute Within Parameters
  std::cout << "Before  - ComputeWithinTerms" << std::endl;
  ShapeEvaluation::ComputeWithinTerms(super_matrix, num_particles, within_objectives);
  std::cout << "After  - ComputeWithinTerms" << std::endl;
  ShapeEvaluation::ComputeBetweenTerms(super_matrix, num_particles, between_objectives);
  std::cout << "After  - ComputeBetweenTerms" << std::endl;

}

void ShapeEvaluation::ComputeWithinTerms(Eigen::MatrixXd &super_matrix, const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives)
{
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

void ShapeEvaluation::ComputeBetweenTerms(Eigen::MatrixXd &super_matrix, const std::vector<int>& num_particles, Eigen::MatrixXd between_objectives)
{

  unsigned int dps = num_particles.size();
  unsigned int M =  std::accumulate(num_particles.begin(), num_particles.end(), 0);
  unsigned int N = super_matrix.cols();
  //PART A: Compute deviations of COM of each organ from COM of all the organs
  Eigen::MatrixXd between_centering_matrix(M, M);// Build centering matrix for between
  between_centering_matrix.setIdentity();
  Eigen::MatrixXd ones_m = Eigen::MatrixXd::Constant(M, 1, 1.0);
  between_centering_matrix = between_centering_matrix - (ones_m * ones_m.transpose()) * (1.0 / ((double)M));
  Eigen::MatrixXd z_centered = between_centering_matrix * super_matrix; // M * 3N
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
    Eigen::MatrixXd ones_m_k = Eigen::MatrixXd::Constant(num_particles[k], 1, 1.0);
    Eigen::MatrixXd z_temp = (ones_m_k.transpose() * z_centered_k) * (1.0 / ((double)num_particles[k])); // --> 1 X 3N
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
  between_objectives = (z_between_objective);
}


} // shapeworks

