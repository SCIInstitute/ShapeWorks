
#include "ParticleShapeStatistics.h"
#include "ShapeEvaluation.h"
#include <Libs/Project/Project.h>


#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <tinyxml.h>

namespace shapeworks{

int ParticleShapeStatistics::SimpleLinearRegression(const std::vector<double>& y,
                         const std::vector<double>& x,
                         double& a, double& b) const
{
  if (x.size() != y.size()) return -1;

  double xmean = 0.0;
  double ymean = 0.0;
  double cross = 0.0;
  double xvar = 0.0;
  double n = static_cast<double>(y.size());

  for (unsigned int i = 0; i < y.size(); i++) {
    xmean += x[i];
    ymean += y[i];
  }
  xmean /= n;
  ymean /= n;

  for (unsigned int i = 0; i < y.size(); i++) {
    double xm = x[i] - xmean;
    cross += xm * (y[i] - ymean);
    xvar += xm * xm;
  }

  b = cross / xvar;
  a = ymean - (b * xmean);

  return 0;
}

int ParticleShapeStatistics::ComputeMedianShape(const int ID)
{
  int ret = -1;
  double min_L1 = 1.0e300;
  //  std::cout << "ID == " << ID << std::endl;
  // Compile list of indices for groupIDs == ID
  std::vector<unsigned int> set;
  for (unsigned int i = 0; i < m_groupIDs.size(); i++) {

    if (m_groupIDs[i] == ID || ID == -32) // -32 means use both groups
    {
      //      std::cout << i << " -> " << m_groupIDs[i] << " =? " << ID << std::endl;
      set.push_back(i);
    }
  }

  // Find min sum L1 norms
  for (unsigned int i = 0; i < set.size(); i++) {
    double sum = 0.0;
    //    std::cout << "set[" << i << "] = " << set[i] << std::endl;

    for (unsigned int j = 0; j < set.size(); j++) {
      if (i != j) sum += this->L1Norm(set[i], set[j]);
      // std::cout << set[j] << "\t" << this->L1Norm(set[i],set[j]) << std::endl;
    }
    //    std::cout << sum << std::endl;
    if (sum < min_L1) {
      min_L1 = sum;
      ret = static_cast<int>(set[i]);
    }
  }
  //  std::cout << "min_L1 = " << min_L1 << std::endl;
  //  std::cout << "index = " << ret << std::endl;
  return ret; // if there has been some error ret == -1
}

double ParticleShapeStatistics::L1Norm(unsigned int a, unsigned int b)
{
  double norm = 0.0;
  for (unsigned int i = 0; i < m_shapes.rows(); i++) {
    norm += fabs(m_shapes(i, a) - m_shapes(i, b));
  }
  return norm;
}

int ParticleShapeStatistics::ImportPoints(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids)
{
  // local copy of points
  points_ = points;

  this->m_groupIDs = group_ids;
  this->m_domainsPerShape = 1;

  int num_points = points[0].size() / 3;

  // Read the point files.  Assumes all the same size.
  m_numSamples1 = 0;
  m_numSamples2 = 0;
  m_numSamples = points.size() / m_domainsPerShape;
  m_numDimensions = num_points * VDimension * m_domainsPerShape;

  // If there are no group IDs, make up some bogus ones
  if (m_groupIDs.size() != m_numSamples) {
    if (m_groupIDs.size() > 0) {
      std::cerr << "Group ID list does not match shape list in size." << std::endl;
      return 1;
    }

    m_groupIDs.resize(m_numSamples);
    for (unsigned int k = 0; k < m_numSamples / 2; k++) {
      m_groupIDs[k] = 1;
      m_numSamples1++;
    }
    for (unsigned int k = m_numSamples / 2; k < m_numSamples; k++) {
      m_groupIDs[k] = 2;
      m_numSamples2++;
    }
  }
  else {
    for (int i = 0; i < m_groupIDs.size(); i++) {
      if (m_groupIDs[i] == 1) {
        m_numSamples1++;
      }
      else {
        m_numSamples2++;
      }
    }
  }

  //std::cerr << "m_numSamples1 = " << m_numSamples1 << "\n";
  //std::cerr << "m_numSamples2 = " << m_numSamples2 << "\n";

  m_pointsMinusMean.resize(m_numDimensions, m_numSamples);
  m_pointsMinusMean.fill(0);
  m_shapes.resize(m_numDimensions, m_numSamples);
  m_mean.resize(m_numDimensions);
  m_mean.fill(0);

  m_mean1.resize(m_numDimensions);
  m_mean1.fill(0);
  m_mean2.resize(m_numDimensions);
  m_mean2.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      unsigned int q = points[i].size();
      for (unsigned int j = 0; j < q; j++) {
        m_pointsMinusMean(q * k * VDimension + j, i) = points[i][j];

        m_mean(q * k * VDimension + j) += points[i][j];

        if (m_groupIDs[i] == 1) {
          m_mean1(q * k * VDimension + j) += points[i][j];
        }
        else {
          m_mean2(q * k * VDimension + j) += points[i][j];
        }

        m_shapes(q * k * VDimension + j, i) = points[i][j];
      }
    }
  }

  for (unsigned int i = 0; i < m_numDimensions; i++) {
    m_mean(i) /= (double) m_numSamples;
    m_mean1(i) /= (double) m_numSamples1;
    m_mean2(i) /= (double) m_numSamples2;
  }

  for (unsigned int j = 0; j < m_numDimensions; j++) {
    for (unsigned int i = 0; i < m_numSamples; i++) {
      m_pointsMinusMean(j, i) -= m_mean(j);
    }
  }

  m_groupdiff = m_mean2 - m_mean1;

  // copy to Eigen matrix
  this->m_Matrix.resize(num_points * 3, m_numSamples);
  this->m_group_1_matrix.resize(num_points * 3, m_numSamples1);
  this->m_group_2_matrix.resize(num_points * 3, m_numSamples2);
  int group1_idx = 0;
  int group2_idx = 0;
  for (unsigned int i = 0; i < m_numSamples; i++) {
    unsigned int q = points[i].size();
    for (unsigned int j = 0; j < q; j++) {
      this->m_Matrix(j, i) = points[i][j];

      if (m_groupIDs[i] == 1) {
        this->m_group_1_matrix(j,group1_idx) = points[i][j];
      }
      else
      {
        this->m_group_2_matrix(j,group2_idx) = points[i][j];
      }
    }
    if (m_groupIDs[i] == 1) {
      group1_idx++;
    } else {
      group2_idx++;
    }
  }

  return 0;
}

void ParticleShapeStatistics::SetNumberOfParticlesAr(std::vector<int> num_particles_ar)
{
  this->m_num_particles_ar = num_particles_ar;
  for(int i = 0; i < num_particles_ar.size(); i++){
    std::cout << num_particles_ar[i] << " ";
  }
  std::cout << std::endl <<  "Number of Particles Array Set for Multi-level Analysis" << std::endl;
}


int ParticleShapeStatistics::ImportPointsAndComputeMlpca(std::vector<Eigen::VectorXd> points, unsigned int dps)
{
  // debug
  std::cout << "importing points now for MCA dps value " << dps <<" "<< m_domainsPerShape << std::endl;
  unsigned int num_points = (int)(points[0].size() / (3 * dps));
  m_dps = dps;
  m_numPoints = num_points;
  m_N = points.size();
  std::cout << "num_points = " << num_points << "num_samples m_N  = " << m_N << std::endl;
  std::cout << "points[0].size() = " << points[0].size() << std::endl;


  unsigned int n = m_N * VDimension; 
  // std::cout << " n " << n << std::endl;
  // unsigned int m = num_points * dps;
  unsigned int m = 0;
  for(unsigned int idx = 0; idx < dps; idx++) { m += (this->m_num_particles_ar[idx]); }
  std::cout << "m " << m << std::endl;

  m_super_matrix.resize(m, n);
  m_shapes_mca.clear();
  std::cout << "cleared" <<std::endl;

  for(unsigned int i = 0; i < points.size(); i++){
    // unsigned int p = num_points * m_dps; // or = m_super_matrix.rows()
    unsigned int p = m_super_matrix.rows();
    // std::cout << "While building shape matrix, p = " << p << " and m_super_matrix.rows() = "<< m_super_matrix.rows() << std::endl;
    for(unsigned int j= 0; j < p; j++){
      m_super_matrix(j, i * VDimension) = points[i][j * VDimension];
      m_super_matrix(j, i * VDimension + 1) = points[i][j * VDimension + 1];
      m_super_matrix(j, i * VDimension + 2) = points[i][j * VDimension + 2];
    }
  }
  
  std::cout << " Super matrix constructed " << std::endl;

  // Step 2. Compute within covariance matrix

  // 2.a Compute centered matrix for each domain
  
  Eigen::MatrixXd z_within_centred;
  z_within_centred.resize(m, n);
  z_within_centred.fill(0.0);

  for(unsigned int k = 0; k < m_dps; k++){
    // extract shape matrix of kth domain
    std::cout << "start k = " << k << std::endl;
    Eigen::MatrixXd z_k;
    // z_k.set_size(num_points, n);
    // m_super_matrix.extract(z_k, k * num_points, 0);

    z_k.resize(this->m_num_particles_ar[k], n);
    unsigned int row = 0;
    for(unsigned int idx = 0; idx < k; idx++){ row += this->m_num_particles_ar[idx]; }
    std::cout << "row = " << row << std::endl;
    z_k = m_super_matrix.block(row, 0, z_k.rows(), z_k.cols());
    std::cout << "extract done for k = " << k << std::endl;

    // m_shapes_mca.push_back(z_k); // keep track of shape matriz of each domain
    // compute column-wise mean(of each sample)
    Eigen::MatrixXd mean_k;
    mean_k.resize(n, 1);
    mean_k.fill(0.0);
    for (unsigned int j = 0; j < n; j++){
      Eigen::MatrixXd z_k_i = z_k.col(j);
      mean_k(j, 0) = z_k_i.mean();
    }

    Eigen::MatrixXd ones_m_k;
    // ones_m_k.set_size(num_points, 1);
    ones_m_k.resize(this->m_num_particles_ar[k], 1);
    ones_m_k.fill(1.0);

    Eigen::MatrixXd z_k_centred = z_k - (ones_m_k * mean_k.transpose());

    // z_within_centred.update(z_k_centred, k * num_points, 0);
    // z_within_centred.update(z_k_centred, row, 0);
    z_within_centred.block(row, 0, z_k_centred.rows(), z_k_centred.cols()) = z_k_centred;
    std::cout << "update done " << std::endl;

  }

  Eigen::MatrixXd z_within_objective;
  // unsigned int M = num_points * VDimension * m_dps;
  unsigned int M = 0;
  for(unsigned int idx = 0; idx < dps; idx++) { M += (this->m_num_particles_ar[idx] * VDimension); }
  z_within_objective.resize(M, m_N);
  this->m_MatrixWithin.resize(M, m_N);
  // 2.a Compute within objective matrix

  for(unsigned int i = 0; i < m_N; i++){
    // unsigned int p = num_points * m_dps;
    unsigned int p = m;
    for(unsigned int j = 0; j < p; j++){
      z_within_objective(j * VDimension, i) = z_within_centred(j, i * VDimension);
      z_within_objective(j * VDimension + 1, i) = z_within_centred(j, i * VDimension + 1);
      z_within_objective(j * VDimension + 2, i) = z_within_centred(j, i * VDimension + 2);

      //Copy to Eigen matrix for Evaluation Computation
      this->m_MatrixWithin(j * VDimension, i) = z_within_centred(j, i * VDimension);
      this->m_MatrixWithin(j * VDimension + 1, i) = z_within_centred(j, i * VDimension + 1);
      this->m_MatrixWithin(j * VDimension + 2, i) = z_within_centred(j, i * VDimension + 2);
    }
  }

  std::cout << "Within objective computed " << std::endl;

  // 2..b compute within covariance matrix
  m_pointsMinusMean_for_within.resize(M, m_N);
  m_pointsMinusMean_for_within.fill(0.0);
  m_mean_within.resize(M);
  m_mean_within.fill(0.0);

  for(unsigned int i = 0; i < M; i++){
    Eigen::MatrixXd p_i = z_within_objective.row(i);
    m_mean_within(i) = p_i.mean();
    for(unsigned int j = 0; j < m_N; j++){
      m_pointsMinusMean_for_within(i, j) = z_within_objective(i, j) - m_mean_within(i);
    }
  }

  std::cout << "Within Covariance Matrix computed " << std::endl;

  // Step 3.Compute Between Covariance matrix

  Eigen::MatrixXd z_between_centred;
  z_between_centred.resize(m, n);

  for (unsigned int i = 0; i < n; i++){
    Eigen::MatrixXd p_i = m_super_matrix.col(i);
    double col_mean = p_i.mean();
    for (unsigned int j = 0; j < m; j++){
      z_between_centred(j, i) = m_super_matrix(j, i) - col_mean;
    }
  }
  std::cout << "between means done before covariance" << std::endl;

  Eigen::MatrixXd z_between;
  z_between.resize(m_dps, n);
  z_between.fill(0.0);
  for(unsigned int k = 0; k < m_dps ; k++){
    Eigen::MatrixXd z_between_k_mean;
    z_between_k_mean.resize(1, n);;
    for(unsigned int i = 0; i < n; i++){
      Eigen::MatrixXd mean_temp_vec;
      // mean_temp_vec.set_size(num_points, 1);
      mean_temp_vec.resize(this->m_num_particles_ar[k], 1);
      // z_between_centred.extract(mean_temp_vec, k * num_points, i);
      unsigned int row = 0;
      for(unsigned int idx = 0; idx < k; idx++){ row += this->m_num_particles_ar[idx]; }
      // z_between_centred.extract(mean_temp_vec, row, i);
      mean_temp_vec = z_between_centred.block(row, i, mean_temp_vec.rows(), mean_temp_vec.cols());
      z_between_k_mean(0, i) = mean_temp_vec.mean();
    }
    // z_between.update(z_between_k_mean, k, 0);
    z_between.block(k, 0, z_between_k_mean.rows(), z_between_k_mean.cols()) = z_between_k_mean;

  }
  std::cout << "z_between formed" << std::endl;
  Eigen::MatrixXd z_between_objective;
  z_between_objective.resize(m_dps * VDimension, m_N);
  this->m_MatrixBetween.resize(m_dps * VDimension, m_N);
  for(unsigned int k = 0; k < m_dps; k++){
    for(unsigned int i = 0; i < m_N; i++){
      z_between_objective(k * VDimension, i) = z_between(k, i * VDimension);
      z_between_objective(k * VDimension + 1, i) = z_between(k, i * VDimension + 1);
      z_between_objective(k * VDimension + 2, i) = z_between(k, i * VDimension + 2);

      //Copy to eigen matrix for Evaluation computation
      this->m_MatrixBetween(k * VDimension, i) = z_between(k, i * VDimension);
      this->m_MatrixBetween(k * VDimension + 1, i) = z_between(k, i * VDimension + 1);
      this->m_MatrixBetween(k * VDimension + 2, i) = z_between(k, i * VDimension + 2);
    }
  }
  std::cout << "z_between objective done" << std::endl;
  

  //3.b between covariance matrix

  m_pointsMinusMean_for_between.resize(m_dps * VDimension, m_N);
  m_pointsMinusMean_for_between.fill(0.0);
  m_mean_between.resize(m_dps * VDimension);
  m_mean_between.fill(0.0);

  for(unsigned int i = 0; i < m_dps * VDimension; i++){
    Eigen::MatrixXd d_i = z_between_objective.row(i);
    m_mean_between(i) = d_i.mean();
    for(unsigned int j = 0; j < m_N; j++){
      m_pointsMinusMean_for_between(i, j) = z_between_objective(i, j) - m_mean_between(i);
    }
  }

 std::cout << "Between Covariance Matrix computed " << std::endl;
 std::cout << "Multi Level base part done" << std::endl;
//  std::cout << "writing within compactness" << std::endl;
//  std::string fn = "/home/sci/nawazish.khan/Desktop/result/within_compactness.txt";
//  std::ofstream outfile;
//  outfile.open(fn.c_str());

//   Eigen::MatrixXd WithinMatrix = m_MatrixWithin;

//   const int N = WithinMatrix.cols();
//   const int D = WithinMatrix.rows();
//   const int num_modes = N-1; // the number of modes is one less than the number of samples

//   if (num_modes < 1) {
//     std::cout << "no modes" << std::endl;
//   }
//   Eigen::MatrixXd Y = WithinMatrix;

//   // Compute Within subspace here
  
//   const Eigen::VectorXd mu = Y.rowwise().mean();
//   Y.colwise() -= mu;

//   Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
//   const auto S = svd.singularValues().array().pow(2) / (N * D);

//   // Compute cumulative sum
//   Eigen::VectorXd cumsum(num_modes);
//   cumsum(0) = S(0);
//   for (int i = 1; i < num_modes; i++) {
//     cumsum(i) = cumsum(i-1) + S(i);
//   }
//   cumsum /= S.sum();
//   outfile << cumsum << "\n";
//   outfile.close();


//  std::cout << "gen ml file opened" << std::endl;
//  std::cout << "writing between  part compactness" << std::endl;
//   std::string fn1 = "/home/sci/nawazish.khan/Desktop/result/between_compactness.txt";
//  std::ofstream outfile1;
//  outfile1.open(fn1.c_str());

//  Eigen::MatrixXd BetweenMatrix = m_MatrixBetween;

//    const int N1 = BetweenMatrix.cols();
//   const int D1 = BetweenMatrix.rows();
//   const int num_modes1 = N1-1; // the number of modes is one less than the number of samples

//   if (num_modes1 < 1) {
//     std::cout << "no modes" << std::endl;
//   }
//   Eigen::MatrixXd Y1 = BetweenMatrix;

//   // Compute Within subspace here
  
//   const Eigen::VectorXd mu1 = Y1.rowwise().mean();
//   Y1.colwise() -= mu1;
//   std::cout << "before svd done of Y" << std::endl;


//   Eigen::JacobiSVD<Eigen::MatrixXd> svd1(Y1);
//   std::cout << "svd done of Y" << std::endl;
//   const auto S1 = svd1.singularValues().array().pow(2) / (N1 * D1);

//   // Compute cumulative sum
//   Eigen::VectorXd cumsum1(num_modes1);
//   cumsum1(0) = S1(0);
//   for (int i = 1; i < num_modes1; i++) {
 
//     cumsum1(i) = cumsum1(i-1) + S1(i);
//   }
//   cumsum1 /= S1.sum();
//   outfile1 << cumsum1 << "\n";
//   outfile1.close();
//   std::cout << "done " << std::endl;

}


int ParticleShapeStatistics::ComputeWithinModesForMca()
{
  std::cout << "Computing stats for within modes of variation " << std::endl;
  unsigned int m = m_pointsMinusMean_for_within.rows();
  std::cout << "m_N = " << m_N << "m_numSamples = " << m_numSamples << " m = " << m << " m_numDimensions = " << m_numDimensions <<  std::endl;

  Eigen::MatrixXd A = m_pointsMinusMean_for_within.transpose()
                          * m_pointsMinusMean_for_within * (1.0 / ((double) (m_N - 1)));
  
  vnl_matrix<double> vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);
  std::cout << "Eigen decomp done" << std::endl;

  Eigen::MatrixXd within_eigenSymEigenV = Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());
  Eigen::VectorXd within_eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  m_withinEigenvectors = m_pointsMinusMean_for_within * within_eigenSymEigenV;
  m_withinEigenvalues.resize(m_N);

  for (unsigned int i = 0; i < m_N; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < m; j++) {
      total += m_withinEigenvectors(j, i) * m_withinEigenvectors(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < m; j++) {
      m_withinEigenvectors(j, i) = m_withinEigenvectors(j, i) / (total + 1.0e-15);
    }

    m_withinEigenvalues[i] = within_eigenSymEigenD(i);
  }
  std::cout << " Within stats done" << std::endl;
  return 0;
}

int ParticleShapeStatistics::ComputeBetweenModesForMca()
{
  std::cout << "Computing stats for between modes of variation " << std::endl;
  unsigned int m = m_pointsMinusMean_for_between.rows();
  std::cout << "m_N = " << m_N << "m_numSamples = " << m_numSamples << " m = " << m << " m_numDimensions = " << m_numDimensions <<  std::endl;

  Eigen::MatrixXd A = m_pointsMinusMean_for_between.transpose()
                         * m_pointsMinusMean_for_between * (1.0 / ((double) (m_N - 1)));
  vnl_matrix<double> vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);
  std::cout << "Eigen decomp done" << std::endl;

  Eigen::MatrixXd between_eigenSymEigenV = Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());
  Eigen::VectorXd between_eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  m_betweenEigenvectors = m_pointsMinusMean_for_between * between_eigenSymEigenV;
  m_betweenEigenvalues.resize(m_N);

  for (unsigned int i = 0; i < m_N; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < m; j++) {
      total += m_betweenEigenvectors(j, i) * m_betweenEigenvectors(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < m; j++) {
      m_betweenEigenvectors(j, i) = m_betweenEigenvectors(j, i) / (total + 1.0e-15);
    }

    m_betweenEigenvalues[i] = between_eigenSymEigenD(i);
  }

  std::cout << " Between stats done " << std::endl;
  return 0;
}



int ParticleShapeStatistics::ReadPointFiles(const std::string &s)
{
  TiXmlDocument doc(s.c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) std::cerr << "invalid parameter file..." << std::endl;
  TiXmlHandle docHandle(&doc);
  TiXmlElement* elem;
  std::stringstream inputsBuffer;

  // Collect point file names and group id's
  std::vector<std::string> pointsfiles;
  std::string ptFileName;
  elem = docHandle.FirstChild("point_files").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> ptFileName) {
      pointsfiles.push_back(ptFileName);

      m_pointsfiles.push_back(ptFileName); // Keep the points' files to reload.
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  this->m_domainsPerShape = 1;
  elem = docHandle.FirstChild("domains_per_shape").Element();
  if (elem) this->m_domainsPerShape = atoi(elem->GetText());

  // Read the point files.  Assumes all the same size.
  itk::ParticlePositionReader::Pointer reader1 = itk::ParticlePositionReader::New();
  reader1->SetFileName(pointsfiles[0].c_str());
  reader1->Update();
  m_numSamples1 = 0;
  m_numSamples2 = 0;
  m_numSamples = pointsfiles.size() / m_domainsPerShape;
  m_numDimensions = reader1->GetOutput().size() * VDimension * m_domainsPerShape;

  // Read the group ids
  int tmpID;
  elem = docHandle.FirstChild("group_ids").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    for (unsigned int shapeCount = 0; shapeCount < m_numSamples; shapeCount++) {
      inputsBuffer >> tmpID;
      m_groupIDs.push_back(tmpID);
      if (tmpID == 1) m_numSamples1++;
      else m_numSamples2++;
    }
  }

  std::cerr << "group id size = " << m_groupIDs.size() << "\n";
  std::cerr << "numSamples = " << m_numSamples << "\n";

  // If there are no group IDs, make up some bogus ones
  if (m_groupIDs.size() != m_numSamples) {
    if (m_groupIDs.size() > 0) {
      std::cerr << "Group ID list does not match shape list in size." << std::endl;
      return 1;
    }

    m_groupIDs.resize(m_numSamples);
    for (unsigned int k = 0; k < m_numSamples / 2; k++) {
      m_groupIDs[k] = 1;
      m_numSamples1++;
    }
    for (unsigned int k = m_numSamples / 2; k < m_numSamples; k++) {
      m_groupIDs[k] = 2;
      m_numSamples2++;
    }
  };

  m_pointsMinusMean.resize(m_numDimensions, m_numSamples);
  m_shapes.resize(m_numDimensions, m_numSamples);
  m_mean.resize(m_numDimensions);
  m_mean.fill(0);

  m_mean1.resize(m_numDimensions);
  m_mean1.fill(0);
  m_mean2.resize(m_numDimensions);
  m_mean2.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      // read file
      itk::ParticlePositionReader::Pointer reader
        = itk::ParticlePositionReader::New();
      reader->SetFileName(pointsfiles[i * m_domainsPerShape + k].c_str());
      reader->Update();
      unsigned int q = reader->GetOutput().size();
      for (unsigned int j = 0; j < q; j++) {
        m_mean(q * k * VDimension + (VDimension * j) + 0) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 0, i)
          = reader->GetOutput()[j][0];
        m_mean(q * k * VDimension + (VDimension * j) + 1) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 1, i)
          = reader->GetOutput()[j][1];
        m_mean(q * k * VDimension + (VDimension * j) + 2) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 2, i)
          = reader->GetOutput()[j][2];

        if (m_groupIDs[i] == 1) {
          m_mean1(q * k * VDimension + (VDimension * j) + 0) += reader->GetOutput()[j][0];
          m_mean1(q * k * VDimension + (VDimension * j) + 1) += reader->GetOutput()[j][1];
          m_mean1(q * k * VDimension + (VDimension * j) + 2) += reader->GetOutput()[j][2];
        }
        else {
          m_mean2(q * k * VDimension + (VDimension * j) + 0) += reader->GetOutput()[j][0];
          m_mean2(q * k * VDimension + (VDimension * j) + 1) += reader->GetOutput()[j][1];
          m_mean2(q * k * VDimension + (VDimension * j) + 2) += reader->GetOutput()[j][2];
        }

        m_shapes(q * k * VDimension + (VDimension * j) + 0, i) = reader->GetOutput()[j][0];
        m_shapes(q * k * VDimension + (VDimension * j) + 1, i) = reader->GetOutput()[j][1];
        m_shapes(q * k * VDimension + (VDimension * j) + 2, i) = reader->GetOutput()[j][2];

      }
    }
  }

  for (unsigned int i = 0; i < m_numDimensions; i++) {
    m_mean(i) /= (double) m_numSamples;
    m_mean1(i) /= (double) m_numSamples1;
    m_mean2(i) /= (double) m_numSamples2;
  }

  for (unsigned int j = 0; j < m_numDimensions; j++) {
    for (unsigned int i = 0; i < m_numSamples; i++) {
      m_pointsMinusMean(j, i) -= m_mean(j);
    }
  }

  m_groupdiff = m_mean2 - m_mean1;

  return 0;
}

//---------------------------------------------------------------------------
ParticleShapeStatistics::ParticleShapeStatistics(std::shared_ptr<Project> project) {

  std::vector<Eigen::VectorXd> points;
  std::vector<int> groups;
  for (auto& s : project->get_subjects()) {
    auto world_files = s->get_world_particle_filenames();
    Eigen::VectorXd particles;
    for (auto& file : world_files) {
      Eigen::VectorXd domain_particles;
      ParticleSystem::ReadParticleFile(file, domain_particles);
      Eigen::VectorXd combined(particles.size() + domain_particles.size());
      combined << particles, domain_particles;
      particles = combined;
    }
    points.push_back(particles);
    groups.push_back(1);
  }
  ImportPoints(points, groups);
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::DoPCA(std::vector<std::vector<Point>> global_pts, int domainsPerShape)
{
  this->m_domainsPerShape = domainsPerShape;

  // Assumes all the same size.
  m_numSamples = global_pts.size() / m_domainsPerShape;
  m_numDimensions = global_pts[0].size() * VDimension * m_domainsPerShape;

  m_pointsMinusMean.resize(m_numDimensions, m_numSamples);
  m_shapes.resize(m_numDimensions, m_numSamples);
  m_mean.resize(m_numDimensions);
  m_mean.fill(0);

  std::cout << "VDimension = " << VDimension << "-------------\n";
  std::cout << "m_numSamples = " << m_numSamples << "-------------\n";
  std::cout << "m_domainsPerShape = " << m_domainsPerShape << "-------------\n";
  std::cout << "global_pts.size() = " << global_pts.size() << "-------------\n";

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      //std::cout << "i*m_domainsPerShape + k = " << i*m_domainsPerShape + k << "-------------\n";
      std::vector<Point> curDomain = global_pts[i * m_domainsPerShape + k];
      unsigned int q = curDomain.size();

      //std::cout << "q = " << q << "-------------\n";
      for (unsigned int j = 0; j < q; j++) {
        m_mean(q * k * VDimension + (VDimension * j) + 0) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 0, i)
          = curDomain[j][0];
        m_mean(q * k * VDimension + (VDimension * j) + 1) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 1, i)
          = curDomain[j][1];
        m_mean(q * k * VDimension + (VDimension * j) + 2) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 2, i)
          = curDomain[j][2];

        m_shapes(q * k * VDimension + (VDimension * j) + 0, i) = curDomain[j][0];
        m_shapes(q * k * VDimension + (VDimension * j) + 1, i) = curDomain[j][1];
        m_shapes(q * k * VDimension + (VDimension * j) + 2, i) = curDomain[j][2];

      }
    }
  }

  for (unsigned int i = 0; i < m_numDimensions; i++) {
    m_mean(i) /= (double) m_numSamples;
  }

  for (unsigned int j = 0; j < m_numDimensions; j++) {
    for (unsigned int i = 0; i < m_numSamples; i++) {
      m_pointsMinusMean(j, i) -= m_mean(j);
    }
  }

  ComputeModes();
  return 0;
}

int ParticleShapeStatistics::DoPCA(ParticleSystem particleSystem, int domainsPerShape)
{
  Eigen::MatrixXd p = particleSystem.Particles();

  std::vector<std::vector<Point>> particlePoints;

  for (int i=0; i<p.cols(); i++) {
    std::vector<Point> particle;
    for (int j=0; j<p.rows()/3; j++) {
      Point point;
      for (int k=0; k<3; k++) {
        point[k] = p.coeff(j*3+k, i);
      }
      particle.push_back(point);
    }
    particlePoints.push_back(particle);
  }

  return DoPCA(particlePoints, domainsPerShape);
}

int ParticleShapeStatistics::ReloadPointFiles()
{

  m_mean.fill(0);
  m_mean1.fill(0);
  m_mean2.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      // read file
      itk::ParticlePositionReader::Pointer reader
        = itk::ParticlePositionReader::New();
      reader->SetFileName(m_pointsfiles[i * m_domainsPerShape + k].c_str());
      reader->Update();
      unsigned int q = reader->GetOutput().size();
      for (unsigned int j = 0; j < q; j++) {
        m_mean(q * k * VDimension + (VDimension * j) + 0) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 0, i)
          = reader->GetOutput()[j][0];
        m_mean(q * k * VDimension + (VDimension * j) + 1) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 1, i)
          = reader->GetOutput()[j][1];
        m_mean(q * k * VDimension + (VDimension * j) + 2) += m_pointsMinusMean(
          q * k * VDimension + (VDimension * j) + 2, i)
          = reader->GetOutput()[j][2];

        if (m_groupIDs[i] == 1) {
          m_mean1(q * k * VDimension + (VDimension * j) + 0) += reader->GetOutput()[j][0];
          m_mean1(q * k * VDimension + (VDimension * j) + 1) += reader->GetOutput()[j][1];
          m_mean1(q * k * VDimension + (VDimension * j) + 2) += reader->GetOutput()[j][2];
        }
        else {
          m_mean2(q * k * VDimension + (VDimension * j) + 0) += reader->GetOutput()[j][0];
          m_mean2(q * k * VDimension + (VDimension * j) + 1) += reader->GetOutput()[j][1];
          m_mean2(q * k * VDimension + (VDimension * j) + 2) += reader->GetOutput()[j][2];
        }

        m_shapes(q * k * VDimension + (VDimension * j) + 0, i) = reader->GetOutput()[j][0];
        m_shapes(q * k * VDimension + (VDimension * j) + 1, i) = reader->GetOutput()[j][1];
        m_shapes(q * k * VDimension + (VDimension * j) + 2, i) = reader->GetOutput()[j][2];

      }
    }
  }

  for (unsigned int i = 0; i < m_numDimensions; i++) {
    m_mean(i) /= (double) m_numSamples;
    m_mean1(i) /= (double) m_numSamples1;
    m_mean2(i) /= (double) m_numSamples2;
  }

  for (unsigned int j = 0; j < m_numDimensions; j++) {
    for (unsigned int i = 0; i < m_numSamples; i++) {
      m_pointsMinusMean(j, i) -= m_mean(j);
    }
  }

  m_groupdiff = m_mean2 - m_mean1;

  return 0;
}

int ParticleShapeStatistics::ComputeModes()
{
  Eigen::MatrixXd A = m_pointsMinusMean.transpose()
                      * m_pointsMinusMean * (1.0 / ((double) (m_numSamples - 1)));

  vnl_matrix<double> vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);

  Eigen::MatrixXd eigenSymEigenV = Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());
  Eigen::VectorXd eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  m_eigenvectors = m_pointsMinusMean * eigenSymEigenV;
  m_eigenvalues.resize(m_numSamples);

  // normalize those eigenvectors
  for (unsigned int i = 0; i < m_numSamples; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < m_numDimensions; j++) {
      total += m_eigenvectors(j, i) * m_eigenvectors(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < m_numDimensions; j++) {
      m_eigenvectors(j, i) = m_eigenvectors(j, i) / (total + 1.0e-15);
    }

    m_eigenvalues[i] = eigenSymEigenD(i);
  }

  float sum = 0.0;
  for (unsigned int n = 0; n < m_numSamples; n++) {
    sum += m_eigenvalues[(m_numSamples - 1) - n];
  }

  float sum2 = 0.0;
  bool found = false;
  for (unsigned int n = 0; n < m_numSamples; n++) {
    sum2 += m_eigenvalues[(m_numSamples - 1) - n];
    m_percentVarByMode.push_back(sum2 / sum);

    if ((sum2 / sum) >= 0.95 && found == false) {
      found = true;
    }
  }

  return 0;
}

int ParticleShapeStatistics::PrincipalComponentProjections()
{
  // Now print the projection of each shape
  m_principals.resize(m_numSamples, m_numSamples);

  for (unsigned int n = 0; n < m_numSamples; n++) {
    for (unsigned int s = 0; s < m_numSamples; s++) {
      double p = m_eigenvectors.col((m_numSamples - 1) - n).dot(m_pointsMinusMean.col(s));
      m_principals(s, n) = p; // each row is a sample, columns index PC

    }
  }

  return 0;
}

int ParticleShapeStatistics::FisherLinearDiscriminant(unsigned int numModes)
{
  m_projectedMean1.resize(numModes);
  m_projectedMean2.resize(numModes);
  m_projectedMean1.fill(0.0);
  m_projectedMean2.fill(0.0);

  m_projectedPMM1.resize(numModes, m_numSamples1);
  m_projectedPMM2.resize(numModes, m_numSamples2);

  unsigned int s1 = 0;
  unsigned int s2 = 0;
  for (unsigned int n = 0; n < numModes; n++) {
    s1 = 0;
    s2 = 0;
    for (unsigned int s = 0; s < m_numSamples; s++) {
      double p = m_eigenvectors.col((m_numSamples - 1) - n).dot(m_pointsMinusMean.col(s));

      if (m_groupIDs[s] == 1) {
        m_projectedPMM1(n, s1) = p;
        m_projectedMean1[n] += p;
        s1++;
      }
      else {
        m_projectedPMM2(n, s2) = p;
        m_projectedMean2[n] += p;
        s2++;
      }
    }
  }

  // Compute means and covariance matrices for each group
  m_projectedMean1 /= static_cast<double>(m_numSamples1);
  m_projectedMean2 /= static_cast<double>(m_numSamples2);

  m_fishersProjection.resize(m_numSamples);
  for (unsigned int i = 0; i < m_numSamples; i++) m_fishersProjection[i] = 0.0;

  for (unsigned int i = 0; i < numModes; i++) // modes
  {
    for (unsigned int j = 0; j < m_numSamples1; j++) // samples
    {
      m_projectedPMM1(i, j) -= m_projectedMean1(i);
    }
    for (unsigned int j = 0; j < m_numSamples2; j++) // samples
    {
      m_projectedPMM2(i, j) -= m_projectedMean2(i);
    }
  }

  Eigen::MatrixXd cov1 = (m_projectedPMM1 * m_projectedPMM1.transpose())
                            / ((double) (m_numSamples1) - 1.0);
  Eigen::MatrixXd cov2 = (m_projectedPMM2 * m_projectedPMM2.transpose())
                            / ((double) (m_numSamples2) - 1.0);

  Eigen::VectorXd mdiff = m_projectedMean1 - m_projectedMean2;
  Eigen::MatrixXd covsuminv = (cov1 + cov2).inverse();

  // w is fishers linear discriminant (normal to the hyperplane)
  Eigen::VectorXd w = covsuminv * mdiff;

  // Normalize to distance between means
  double mag = mdiff.size();
  m_fishersLD = (w * mag) / sqrt(w.dot(w));

  Eigen::VectorXd wext(m_numSamples);
  for (unsigned int i = 0; i < m_numSamples; i++) {
    if (i >= numModes) wext[i] = 0.0;
    else wext[i] = m_fishersLD[i];// * m_eigenvalues[(m_numSamples - 1) - i];
  }

  // Rotate the LD back into the full dimensional space
  // Rearrange the eigenvectors:

  Eigen::MatrixXd tmpeigs = m_eigenvectors;
  tmpeigs.rowwise().reverse();

  Eigen::MatrixXd bigLD = wext * tmpeigs.transpose();

  // Create a file of vectors in the VDimensionD space from bigLD that KWMeshvisu can read

  // Open the output file.
  std::ofstream out("LinearDiscriminantsVectors.txt");

  out << "NUMBER_OF_POINTS = " << m_numDimensions << std::endl;
  out << "DIMENSION = " << VDimension << std::endl;
  out << "TYPE = Vector" << std::endl;

  // Write points.
  for (unsigned int i = 0; i < m_numDimensions;) {
    for (unsigned int j = 0; j < VDimension; j++) {
      out << -bigLD(i) << " ";
      i++;
    }
    out << std::endl;
  }

  out.close();
  return 0;
}

int ParticleShapeStatistics::WriteCSVFile2(const std::string &s)
{
  // Write csv file
  std::ofstream outfile;
  outfile.open(s.c_str());
  if (!outfile.good()) {
    throw std::runtime_error("Unable to open " + s + " for writing");
  }

  outfile << "Group";
  for (unsigned int i = 0; i < m_numSamples; i++) {
    outfile << ",P" << i;
  }
  outfile << std::endl;

  for (unsigned int r = 0; r < m_numSamples; r++) {
    outfile << m_groupIDs[r];
    for (unsigned int c = 0; c < m_numSamples; c++) {
      outfile << "," << m_principals(r, c);
    }
    outfile << std::endl;
  }

  outfile.close();
  return 0;
}

int ParticleShapeStatistics::WriteCSVFile(const std::string &s)
{
  // Write csv file
  std::ofstream outfile;
  outfile.open(s.c_str());

  outfile << "Group,LDA,PV";
  for (unsigned int i = 0; i < m_numSamples; i++) {
    outfile << ",P" << i;
  }
  outfile << std::endl;

  for (unsigned int r = 0; r < m_numSamples; r++) {
    outfile << m_groupIDs[r] << ",";
    outfile << m_fishersProjection[r] << ",";
    outfile << m_percentVarByMode[r];
    for (unsigned int c = 0; c < m_numSamples; c++) {
      outfile << "," << m_principals(r, c);
    }
    outfile << std::endl;
  }

  outfile.close();
  return 0;
}

Eigen::VectorXd ParticleShapeStatistics::get_compactness(std::function<void(float)> progress_callback)
{
  auto ps = shapeworks::ParticleSystem(this->m_Matrix);
  return shapeworks::ShapeEvaluation::ComputeFullCompactness(ps, progress_callback);
}

Eigen::VectorXd ParticleShapeStatistics::get_specificity(std::function<void(float)> progress_callback)
{
  auto ps = shapeworks::ParticleSystem(this->m_Matrix);
  return shapeworks::ShapeEvaluation::ComputeFullSpecificity(ps, progress_callback);
}

Eigen::VectorXd ParticleShapeStatistics::get_generalization(std::function<void(float)> progress_callback)
{
  auto ps = shapeworks::ParticleSystem(this->m_Matrix);
  return shapeworks::ShapeEvaluation::ComputeFullGeneralization(ps, progress_callback);
}

Eigen::MatrixXd ParticleShapeStatistics::get_group1_matrix()
{
  return this->m_group_1_matrix;
}

Eigen::MatrixXd ParticleShapeStatistics::get_group2_matrix()
{
  return this->m_group_2_matrix;
}

void ParticleShapeStatistics::compute_good_bad_points()
{


}

} // shapeworks
