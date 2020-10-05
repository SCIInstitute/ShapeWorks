
#include "ParticleShapeStatistics.h"
#include <Libs/Particles/ShapeEvaluation.h>
#include "tinyxml.h"

int ParticleShapeStatistics
::SimpleLinearRegression(const std::vector<double>& y,
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

int ParticleShapeStatistics::ImportPoints(
  std::vector<vnl_vector<double >> points, std::vector<int> group_ids)
{
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

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);
  m_shapes.set_size(m_numDimensions, m_numSamples);
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
  //this->m_Matrix.resize(m_numSamples, num_points * 3);
  this->m_Matrix.resize(num_points * 3, m_numSamples);
  for (unsigned int i = 0; i < m_numSamples; i++) {
    unsigned int q = points[i].size();
    for (unsigned int j = 0; j < q; j++) {
      this->m_Matrix(j, i) = points[i][j];
    }
  }

  return 0;
}

int ParticleShapeStatistics::ReadPointFiles(const char* fname)
{
  TiXmlDocument doc(fname);
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
} // end ReadPointFiles



int ParticleShapeStatistics::DoPCA(std::vector<std::vector<PointType> > global_pts,
                                   int domainsPerShape)
{
  this->m_domainsPerShape = domainsPerShape;

  // Assumes all the same size.
  m_numSamples = global_pts.size() / m_domainsPerShape;
  m_numDimensions = global_pts[0].size() * VDimension * m_domainsPerShape;

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);
  m_shapes.set_size(m_numDimensions, m_numSamples);
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
      std::vector<PointType> curDomain = global_pts[i * m_domainsPerShape + k];
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
} // end DoPCA


/** Reloads a set of point files and recomputes some statistics. */

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
  // COMPUTE MODES
  vnl_matrix<double> A = m_pointsMinusMean.transpose()
                         * m_pointsMinusMean * (1.0 / ((double) (m_numSamples - 1)));
  vnl_symmetric_eigensystem<double> symEigen(A);

  m_eigenvectors = m_pointsMinusMean * symEigen.V;
  m_eigenvalues.set_size(m_numSamples);

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

    m_eigenvalues(i) = symEigen.D(i, i);
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
}  // end ComputeModes();


int ParticleShapeStatistics::PrincipalComponentProjections()
{
  // Now print the projection of each shape
  m_principals.set_size(m_numSamples, m_numSamples);

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

int ParticleShapeStatistics::WriteCSVFile2(const char* fn)
{
  // Write csv file
  std::ofstream outfile;
  outfile.open(fn);

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

int ParticleShapeStatistics::WriteCSVFile(const char* fn)
{
  // Write csv file
  std::ofstream outfile;
  outfile.open(fn);

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

double ParticleShapeStatistics::get_compactness(const int num_modes)
{
  this->compute_evaluation(num_modes);
  return this->compactness_;
}

double ParticleShapeStatistics::get_specificity(const int num_modes)
{
  this->compute_evaluation(num_modes);
  return this->specificity_;
}

double ParticleShapeStatistics::get_generalization(const int num_modes)
{
  this->compute_evaluation(num_modes);
  return this->generalization_;
}

void ParticleShapeStatistics::compute_evaluation(int num_modes)
{
  if (!this->evaluation_ready_ || num_modes != this->evaluation_modes_) {
    auto ps = shapeworks::ParticleSystem(this->m_Matrix);
    this->compactness_ = shapeworks::ShapeEvaluation<3>::ComputeCompactness(ps, num_modes);
    this->specificity_ = shapeworks::ShapeEvaluation<3>::ComputeSpecificity(ps, num_modes);
    this->generalization_ = shapeworks::ShapeEvaluation<3>::ComputeGeneralization(ps, num_modes);
    this->evaluation_ready_ = true;
    this->evaluation_modes_ = num_modes;
  }

}

