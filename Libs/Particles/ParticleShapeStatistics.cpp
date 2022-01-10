
#include "ParticleShapeStatistics.h"
#include <Libs/Particles/ShapeEvaluation.h>
#include "tinyxml.h"

namespace shapeworks{

int ParticleShapeStatistics::SimpleLinearRegression(const std::vector<double>& y,
                         const std::vector<double>& x,
                         double& a, double& b) const
{
  if (x.size() != y.size()) return -1;

  //  std::cout << "y = ";
  //  for (unsigned int i = 0; i < y.size(); i++)
  //    {
  //    std::cout << y[i] << "\t";
  //    }
  //  std::cout << std::endl;

  //  std::cout << "x = ";
  //  for (unsigned int i = 0; i < y.size(); i++)
  //    {
  //    std::cout << x[i] << "\t";
  //    }
  //  std::cout << std::endl;

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

int ParticleShapeStatistics::ImportPoints(std::vector<vnl_vector<double>> points, std::vector<int> group_ids)
{
  // debug
  std::cout << "importing points now" << std::endl;
  this->m_groupIDs = group_ids;
  this->m_domainsPerShape = 1;

  int num_points = points[0].size() / 3;

  // Read the point files.  Assumes all the same size.
  m_numSamples1 = 0;
  m_numSamples2 = 0;
  m_numSamples = points.size() / m_domainsPerShape;
  m_numDimensions = num_points * VDimension * m_domainsPerShape;
  m_domainNumDimensions = points[0].size();


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

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);
  m_pointsMinusMean.fill(0);
  m_shapes.set_size(m_numDimensions, m_numSamples);
  m_shapes_mca.clear();
  m_mean.set_size(m_numDimensions);
  m_mean.fill(0);

  m_mean1.set_size(m_numDimensions);
  m_mean1.fill(0);
  m_mean2.set_size(m_numDimensions);
  m_mean2.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      unsigned int q = points[i].size();
      vnl_matrix<double> domain_shape;
      domain_shape.set_size(q, m_numSamples);

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
        domain_shape(j, i) = points[i][j];
      }
      m_shapes_mca.push_back(domain_shape);
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

  //debug
  std::cout << "Import points done" << std::endl;
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
  typename itk::ParticlePositionReader<VDimension>::Pointer reader1 = itk::ParticlePositionReader<VDimension>::New();
  reader1->SetFileName(pointsfiles[0].c_str());
  reader1->Update();
  m_numSamples1 = 0;
  m_numSamples2 = 0;
  m_numSamples = pointsfiles.size() / m_domainsPerShape;
  m_numDimensions = reader1->GetOutput().size() * VDimension * m_domainsPerShape;
  m_domainNumDimensions = reader1->GetOutput().size() * VDimension;

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

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);
  m_shapes.set_size(m_numDimensions, m_numSamples);
  m_shapes_mca.clear();
  m_mean.set_size(m_numDimensions);
  m_mean.fill(0);

  m_mean1.set_size(m_numDimensions);
  m_mean1.fill(0);
  m_mean2.set_size(m_numDimensions);
  m_mean2.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < m_numSamples; i++) {
    for (unsigned int k = 0; k < m_domainsPerShape; k++) {
      // read file
      typename itk::ParticlePositionReader<VDimension>::Pointer reader
        = itk::ParticlePositionReader<VDimension>::New();
      reader->SetFileName(pointsfiles[i * m_domainsPerShape + k].c_str());
      reader->Update();
      unsigned int q = reader->GetOutput().size();
      vnl_matrix<double> domain_shape;
      domain_shape.set_size(m_domainNumDimensions, m_numSamples);

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

        domain_shape(j * VDimension + 0, i) = reader->GetOutput()[j][0];
        domain_shape(j * VDimension + 1, i) = reader->GetOutput()[j][1];
        domain_shape(j * VDimension + 2, i) = reader->GetOutput()[j][2];
      }
      // m_shapes_mca[k] = domain_shape;
      m_shapes_mca.push_back(domain_shape);
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

int ParticleShapeStatistics::DoPCA(std::vector<std::vector<Point>> global_pts, int domainsPerShape)
{
  this->m_domainsPerShape = domainsPerShape;

  // Assumes all the same size.
  m_numSamples = global_pts.size() / m_domainsPerShape;
  m_numDimensions = global_pts[0].size() * VDimension * m_domainsPerShape;
  m_domainNumDimensions = global_pts[0].size() * VDimension;

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);
  m_shapes.set_size(m_numDimensions, m_numSamples);
  m_shapes_mca.clear();
  m_mean.set_size(m_numDimensions);
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
      vnl_matrix<double> domain_shape;
      domain_shape.set_size(m_domainNumDimensions, m_numSamples);

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

        domain_shape(j * VDimension + 0, i) = curDomain[j][0];
        domain_shape(j * VDimension + 1, i) = curDomain[j][1];
        domain_shape(j * VDimension + 2, i) = curDomain[j][2];
      }
      m_shapes_mca.push_back(domain_shape);
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
      typename itk::ParticlePositionReader<VDimension>::Pointer reader
        = itk::ParticlePositionReader<VDimension>::New();
      reader->SetFileName(m_pointsfiles[i * m_domainsPerShape + k].c_str());
      reader->Update();
      unsigned int q = reader->GetOutput().size();
      vnl_matrix<double> domain_shape;
      domain_shape.set_size(m_domainNumDimensions, m_numSamples);

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

        domain_shape(j * VDimension + 0, i) = reader->GetOutput()[j][0];
        domain_shape(j * VDimension + 1, i) = reader->GetOutput()[j][1];
        domain_shape(j * VDimension + 2, i) = reader->GetOutput()[j][2];
      }
      m_shapes_mca[k] = domain_shape;
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
  vnl_matrix<double> A = m_pointsMinusMean.transpose()
                         * m_pointsMinusMean * (1.0 / ((double) (m_numSamples - 1)));
  vnl_symmetric_eigensystem<double> symEigen(A);

  m_eigenvectors = m_pointsMinusMean * symEigen.V;
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

    m_eigenvalues[i] = symEigen.D(i, i);
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

void ParticleShapeStatistics::MCADecomposition()
{
  // Super matrix
  //grand mean 
  unsigned int m = m_shapes.rows();
  vnl_matrix<double> ones_m;
  ones_m.set_size(1, m);
  ones_m.fill(1.0);
  grand_mean = ones_m * m_shapes;
  grand_mean = grand_mean * (double)(1.0/m) ;
  grand_mean = grand_mean.transpose(); // 1 X N

  ComputeBetweenParams(1);
  ComputeWithinParams(1);
}

void ParticleShapeStatistics::ComputeBetweenParams(int between_components)
{
  //centering matrix
  unsigned int m = m_shapes.rows(); // M
  unsigned int n = m_numSamples; // N
  unsigned int m_k = (int)(m/m_domainsPerShape);
  vnl_matrix<double> centering_matrix;
  centering_matrix.set_size(m, m);
  centering_matrix.fill(1.0);
  vnl_matrix<double> ones_m;
  vnl_matrix<double> ones_m_sub;
  ones_m.set_size(m, 1);
  ones_m_sub.set_size(m_k, 1);
  ones_m.fill(1.0);
  ones_m_sub.fill(1.0);
  centering_matrix = centering_matrix - (ones_m * ones_m.transpose()) * (double)(1.0/m);

  vnl_matrix<double> z_sub_total = centering_matrix * m_shapes;
  vnl_matrix<double> z_centred_res;
  z_centred_res.set_size(m_domainsPerShape, n);
  z_centred_res.fill(0.0);

  for(unsigned int i = 0; i < m_domainsPerShape; i++)
  {
    vnl_matrix<double> z_c_temp;
    z_c_temp.set_size(m_k * VDimension, n);
    z_sub_total.extract(z_c_temp, i * m_domainsPerShape, 0);
    vnl_matrix<double> z_centred_k;
    z_centred_k.set_size(1, n);
    z_centred_k = (ones_m_sub.transpose() * z_c_temp) * (double)(1.0/m_k);
    z_centred_res.update(z_centred_k, i, 0);
  } 
  vnl_matrix<double> w;
  w.set_size(m_domainsPerShape, m_domainsPerShape);
  w.fill_diagonal(sqrt(m_k));

  vnl_matrix<double> between_objective_matrix = w * z_centred_res; // K X N

  // do svd for between
  vnl_svd<double> between_svd(between_objective_matrix);

  // compute CA loading matrix and component scores
  // TODO: check for particular components extraction
  vnl_matrix<double> w_inv = w * m_k;
  between_component_scores = w_inv * between_svd.U();
  between_loading_matrix = between_svd.V() * between_svd.W();
}

void ParticleShapeStatistics::ComputeWithinParams(int within_components)
{
  unsigned int m = m_shapes.rows(); // M
  unsigned int n = m_numSamples; // N
  unsigned int m_k = (int)(m/m_domainsPerShape);
  within_loading_matrix.clear();
  within_component_scores.clear();

  for(int i = 0; i < m_domainsPerShape; i++)
  {
    // compute centering matrix
    vnl_matrix<double> centering_matrix;
    centering_matrix.set_size(m_k, m_k);
    centering_matrix.fill(1.0);
    vnl_matrix<double> ones_m_k;
    ones_m_k.set_size(m_k, 1);
    ones_m_k.fill(1.0);
    centering_matrix = centering_matrix - (1.0/m_k) * (ones_m_k * ones_m_k.transpose());

    vnl_matrix<double> within_objective_matrix = centering_matrix.transpose() * m_shapes_mca[i];
    vnl_svd<double> within_svd(within_objective_matrix);

    vnl_matrix<double> within_component_scores_k = within_svd.U();
    vnl_matrix<double> within_loading_matrix_k = within_svd.V() * within_svd.W();

    within_component_scores.push_back(within_component_scores_k);
    within_loading_matrix.push_back(within_loading_matrix_k);
  }
}

void ParticleShapeStatistics::ComputeMCAModeStats()
{
  // reconstruct in terms of MCA decomposition and apply pca on that
  unsigned int m = m_shapes.rows(); // M
  unsigned int n = m_numSamples; // N
  unsigned int m_k = (int)(m/m_domainsPerShape);
  vnl_matrix<double> ones_m_k;
  ones_m_k.set_size(m_k, 1);
  ones_m_k.fill(1.0);
  vnl_matrix<double> mean_part = ones_m_k * grand_mean.transpose();
  std::vector<vnl_matrix<double>> z_within;
  std::vector<vnl_matrix<double>> z_between;

  // A. For Within
  for(unsigned int k = 0; k < m_domainsPerShape; k++)
  {
    vnl_matrix<double> z_k =  within_component_scores[k] * within_loading_matrix[k].transpose(); // M_k X N
    z_k += mean_part;
    z_within.push_back(z_k);
  }

  for(unsigned int k = 0; k < m_domainsPerShape; k++)
  {
    vnl_matrix<double> z_k =  ones_m_k * between_component_scores.get_n_rows(k, 1).transpose() * between_loading_matrix.transpose();
    z_k += mean_part;
    z_between.push_back(z_k);
  }

  // make overall matix now for pca on top of this
  vnl_matrix<double> pointsMinusMean_for_between;
  pointsMinusMean_for_between.set_size(m_numDimensions, m_numSamples);
  pointsMinusMean_for_between.fill(0);
  vnl_vector<double> mean_between;
  mean_between.set_size(m_numDimensions);
  mean_between.fill(0);

  for(unsigned int k = 0; k < m_domainsPerShape; k++)
  {
    pointsMinusMean_for_between.update(z_between[k], k * m_domainsPerShape, 0);
  }

  for (unsigned int i = 0; i < m_numSamples; i++)
  {
    mean_between += pointsMinusMean_for_between.get_column(i);
  }

  mean_between = mean_between * (double)(1.0/m_numSamples);
  for (unsigned int j = 0; j < m_numDimensions; j++) {
    for (unsigned int i = 0; i < m_numSamples; i++) {
      pointsMinusMean_for_between(j, i) -= mean_between(j);
    }
  }


}
int ParticleShapeStatistics::PrincipalComponentProjections()
{
  // Now print the projection of each shape
  m_principals.resize(m_numSamples, m_numSamples);

  for (unsigned int n = 0; n < m_numSamples; n++) {
    for (unsigned int s = 0; s < m_numSamples; s++) {
      double p = dot_product<double>(m_eigenvectors.get_column((m_numSamples - 1) - n),
                                     m_pointsMinusMean.get_column(s));

      m_principals(s, n) = p; // each row is a sample, columns index PC

    }
  }

  return 0;
}

int ParticleShapeStatistics::FisherLinearDiscriminant(unsigned int numModes)
{
  m_projectedMean1.set_size(numModes);
  m_projectedMean2.set_size(numModes);
  m_projectedMean1.fill(0.0);
  m_projectedMean2.fill(0.0);

  m_projectedPMM1.set_size(numModes, m_numSamples1);
  m_projectedPMM2.set_size(numModes, m_numSamples2);

  unsigned int s1 = 0;
  unsigned int s2 = 0;
  for (unsigned int n = 0; n < numModes; n++) {
    s1 = 0;
    s2 = 0;
    for (unsigned int s = 0; s < m_numSamples; s++) {
      double p = dot_product<double>(m_eigenvectors.get_column((m_numSamples - 1) - n),
                                     m_pointsMinusMean.get_column(s));

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

  vnl_matrix<double> cov1 = (m_projectedPMM1 * m_projectedPMM1.transpose())
                            / ((double) (m_numSamples1) - 1.0);
  vnl_matrix<double> cov2 = (m_projectedPMM2 * m_projectedPMM2.transpose())
                            / ((double) (m_numSamples2) - 1.0);

  vnl_vector<double> mdiff = m_projectedMean1 - m_projectedMean2;
  vnl_matrix<double> covsuminv = vnl_matrix_inverse<double>(cov1 + cov2);

  // w is fishers linear discriminant (normal to the hyperplane)
  vnl_vector<double> w = covsuminv * mdiff;

  // Normalize to distance between means
  double mag = mdiff.magnitude();
  m_fishersLD = (w * mag) / sqrt(dot_product<double>(w, w));

  vnl_vector<double> wext(m_numSamples);
  for (unsigned int i = 0; i < m_numSamples; i++) {
    if (i >= numModes) wext[i] = 0.0;
    else wext[i] = m_fishersLD[i];// * m_eigenvalues[(m_numSamples - 1) - i];
  }

  // Rotate the LD back into the full dimensional space
  // Rearrange the eigenvectors:
  vnl_matrix<double> tmpeigs = m_eigenvectors;
  tmpeigs.fliplr();

  vnl_vector<double> bigLD = wext.post_multiply(tmpeigs.transpose());

  // Create a file of vectors in the VDimensionD space from bigLD that KWMeshvisu can
  // read

  // Open the output file.
  std::ofstream out("LinearDiscriminantsVectors.txt");

  out << "NUMBER_OF_POINTS = " << m_numDimensions << std::endl;
  out << "DIMENSION = " << VDimension << std::endl;
  out << "TYPE = Vector" << std::endl;

  // Write points.
  for (unsigned int i = 0; i < m_numDimensions;) {
    for (unsigned int j = 0; j < VDimension; j++) {
      out << -bigLD[i] << " ";
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


} // shapeworks
