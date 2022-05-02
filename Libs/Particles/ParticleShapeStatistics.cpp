
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
