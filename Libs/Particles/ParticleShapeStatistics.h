#pragma once

#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include <Eigen/Eigen>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>

#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"
#include "Shapeworks.h"
#include "ParticleSystem.h"

/**
 * \class ParticleShapeStatistics
 * This class computes various statistics for a set of correspondence positions
 * and group ids.
 */

namespace shapeworks {
class ParticleShapeStatistics
{
public:

  constexpr static int VDimension = 3;

  ParticleShapeStatistics() {}
  ~ParticleShapeStatistics() {}

  int DoPCA(std::vector<std::vector<Point>> global_pts, int domainsPerShape = 1);

  int DoPCA(ParticleSystem particleSystem, int domainsPerShape = 1);

  void MCADecomposition(); 
  void ComputeBetweenParams(int b);
  void ComputeWithinParams(int w);
  void ComputeMCAModeStats();
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Loads a set of point files and pre-computes some statistics. */
  int ImportPoints(std::vector<vnl_vector<double>> points, std::vector<int> group_ids);
  int ImportPointsForMca(std::vector<vnl_vector<double>> points, unsigned int dps);
  /** Loads a set of point files and pre-computes some statistics. */
  int ReadPointFiles(const std::string &s);

  /** Reloads a set of point files and recomputes some statistics. */
  int ReloadPointFiles();

  /** Writes a text file in comma-separated format.  Suitable for reading into
      excel or R or Matlab for analysis. */
  int WriteCSVFile(const std::string &s);
  int WriteCSVFile2(const std::string &s);

  /** Computes PCA modes from the set of correspondence mode positions.
      Requires that ReadPointFiles be called first. */
  int ComputeModes();

  /** Computes the principal component loadings, or projections onto the
      principal componenent axes for each of the samples.  ComputeModes must be
      called first. */
  int PrincipalComponentProjections();

  /** Computes fishers linear discriminant line for best group separation. */
  int FisherLinearDiscriminant(unsigned int numModes);

  /** Returns the sample size. */
  const int SampleSize() { return m_numSamples; }
  const int Group1SampleSize() { return m_numSamples1; }
  const int Group2SampleSize() { return m_numSamples2; }

  /** Returns the number of dimensions (this is number of points times Dimension) */
  const int NumberOfDimensions() { return m_numDimensions; }
  const int DomainNumberOfDimensions() { return m_domainNumDimensions; }

  /** Returns the group ids */
  const int GroupID(unsigned int i) { return m_groupIDs[i]; }
  const std::vector<int> &GroupID() { return m_groupIDs;}

  /** Returns the eigenvectors/values. */
  const vnl_matrix<double> &Eigenvectors() { return m_eigenvectors; }
  const std::vector<double> &Eigenvalues() { return m_eigenvalues; }


  const vnl_matrix<double> &BetweenEigenvectors() { return m_betweenEigenvectors; }
  const std::vector<double> &BetweenEigenvalues() { return m_betweenEigenvalues; }

  const vnl_matrix<double> &WithinEigenvectors() { return m_withinEigenvectors; }
  const std::vector<double> &WithinEigenvalues() { return m_withinEigenvalues; }


  /** Returns the mean shape. */
  const vnl_vector<double> &Mean() { return m_mean; }
  const vnl_matrix<double> &GrandMean() { return m_grand_mean; }
  const vnl_vector<double> &WithinMean() { return m_mean_within; }
  const vnl_vector<double> &BetweenMean() { return m_mean_between; }
  const vnl_vector<double> &MCAMean() { return m_mcaMean; }
  const vnl_vector<double> &Group1Mean() { return m_mean1; }
  const vnl_vector<double> &Group2Mean() { return m_mean2; }

  // Returns group2 - group1 mean
  const vnl_vector<double> &NormalizedGroupDifference() { return m_groupdiffnorm; }
  const vnl_vector<double> &GroupDifference() { return m_groupdiff; }

 /** Returns the median shape for the set of shapes with Group ID equal to the
      integer argument.  For example, ComputeMedianShape(0) returns the median
      shape for the set of shapes with m_groupIDs == 0. The median shape is
      defined as the shape with the minimum sum of Euclidean L1 norms to all
      other shapes in that group.  Arguments passed to this function are set to
      the index number of the median shape for Group A and Group B,
      respectively.*/
  int ComputeMedianShape(const int ID);

  /** Returns the euclidean L1 norm between shape a and b */
  double L1Norm(unsigned int a, unsigned int b);

  /** Returns the component loadings */
  Eigen::MatrixXd &PCALoadings() { return m_principals; }

  /** Returns the Fisher linear discriminant */
  const vnl_vector<double> &FishersLDA() { return m_fishersLD; }

  /** Returns the shape matrix*/
  const vnl_matrix<double> &ShapeMatrix() { return m_shapes; }

  /** Returns the shape with the mean subtracted */
  const vnl_matrix<double> &RecenteredShape() { return m_pointsMinusMean; }

  std::vector<double> PercentVarByMode() { return m_percentVarByMode; }

  /** Computes a simple linear regression of the first list of values with
      respect to the second y=a + bx. Returns the estimated parameters a & b.
       Returns 0 on success and -1 on fail.*/
  int SimpleLinearRegression(const std::vector<double> &y,
                             const std::vector<double> &x,
                             double &a, double &b) const;

  Eigen::VectorXd get_compactness(std::function<void(float)> progress_callback);
  Eigen::VectorXd get_specificity(std::function<void (float)> progress_callback);
  Eigen::VectorXd get_generalization(std::function<void (float)> progress_callback);

  Eigen::MatrixXd get_group1_matrix();
  Eigen::MatrixXd get_group2_matrix();
private:

  unsigned int m_numSamples1;
  unsigned int m_numSamples2;
  unsigned int m_numSamples;
  unsigned int m_domainsPerShape;
  unsigned int m_dps;
  unsigned int m_numDimensions;
  unsigned int m_domainNumDimensions;
  std::vector<int> m_groupIDs;

  vnl_matrix<double> m_pooled_covariance;
  vnl_matrix<double> m_eigenvectors;
  std::vector<double> m_eigenvalues;
  // For MCA
  vnl_matrix<double> m_betweenEigenvectors;
  vnl_matrix<double> m_withinEigenvectors;

  vnl_matrix<double> m_between_component_scores;
  std::vector<vnl_matrix<double>> m_within_component_scores;
  std::vector<vnl_matrix<double>> m_within_loading_matrix;
  vnl_matrix<double> m_between_loading_matrix;


  vnl_matrix<double> m_pointsMinusMean_for_between;
  vnl_matrix<double> m_pointsMinusMean_for_within;
  vnl_vector<double> m_mean_within;
  vnl_vector<double> m_mean_between;
  
  std::vector<double> m_betweenEigenvalues;
  std::vector<double> m_withinEigenvalues;
  vnl_vector<double> m_mean;
  // For MCA
  vnl_matrix<double> m_grand_mean;
  vnl_vector<double> m_mcaMean;
  vnl_vector<double> m_mean1;
  vnl_vector<double> m_mean2;
  vnl_matrix<double> m_pointsMinusMean;
  vnl_matrix<double> m_shapes;
  vnl_matrix<double> m_super_matrix;
  std::vector<vnl_matrix<double>> m_shapes_mca;
  vnl_matrix<double> m_projectedPMM1;
  vnl_matrix<double> m_projectedPMM2;
  vnl_vector<double> m_projectedMean1;
  vnl_vector<double> m_projectedMean2;
  std::vector<double> m_fishersProjection;
  std::vector<double> m_percentVarByMode;
  vnl_vector<double> m_fishersLD;
  Eigen::MatrixXd m_principals;

  vnl_vector<double> m_groupdiff;
  vnl_vector<double> m_groupdiffnorm;

  // used to keep the points' files that needs to be reloaded when new updates come in.
  std::vector<std::string> m_pointsfiles;

  Eigen::MatrixXd m_Matrix;

  Eigen::MatrixXd m_group_1_matrix;
  Eigen::MatrixXd m_group_2_matrix;
};

} // shapeworks
