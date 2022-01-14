#pragma once

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

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Loads a set of point files and pre-computes some statistics. */
  int ImportPoints(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids);

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

  /** Returns the group ids */
  const int GroupID(unsigned int i) { return m_groupIDs[i]; }
  const std::vector<int> &GroupID() { return m_groupIDs;}

  /** Returns the eigenvectors/values. */
  const Eigen::MatrixXd &Eigenvectors() { return m_eigenvectors; }
  const std::vector<double> &Eigenvalues() { return m_eigenvalues; }

  /** Returns the mean shape. */
  const Eigen::VectorXd &Mean() { return m_mean; }
  const Eigen::VectorXd &Group1Mean() { return m_mean1; }
  const Eigen::VectorXd &Group2Mean() { return m_mean2; }

  // Returns group2 - group1 mean
  const Eigen::VectorXd &NormalizedGroupDifference() { return m_groupdiffnorm; }
  const Eigen::VectorXd &GroupDifference() { return m_groupdiff; }

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
  const Eigen::VectorXd &FishersLDA() { return m_fishersLD; }

  /** Returns the shape matrix*/
  const Eigen::MatrixXd &ShapeMatrix() { return m_shapes; }

  /** Returns the shape with the mean subtracted */
  const Eigen::MatrixXd &RecenteredShape() { return m_pointsMinusMean; }

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
  unsigned int m_numDimensions;
  std::vector<int> m_groupIDs;

  Eigen::MatrixXd m_eigenvectors;
  std::vector<double> m_eigenvalues;
  Eigen::VectorXd m_mean;
  Eigen::VectorXd m_mean1;
  Eigen::VectorXd m_mean2;
  Eigen::MatrixXd m_pointsMinusMean;
  Eigen::MatrixXd m_shapes;
  Eigen::MatrixXd m_projectedPMM1;
  Eigen::MatrixXd m_projectedPMM2;
  Eigen::VectorXd m_projectedMean1;
  Eigen::VectorXd m_projectedMean2;
  std::vector<double> m_fishersProjection;
  std::vector<double> m_percentVarByMode;
  Eigen::VectorXd m_fishersLD;
  Eigen::MatrixXd m_principals;

  Eigen::VectorXd m_groupdiff;
  Eigen::VectorXd m_groupdiffnorm;

  // used to keep the points' files that needs to be reloaded when new updates come in.
  std::vector<std::string> m_pointsfiles;

  Eigen::MatrixXd m_Matrix;

  Eigen::MatrixXd m_group_1_matrix;
  Eigen::MatrixXd m_group_2_matrix;
};

} // shapeworks
