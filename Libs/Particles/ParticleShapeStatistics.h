#pragma once

#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
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

/**
 * \class ParticleShapeStatistics
 * This class computes various statistics for a set of correspondence positions
 * and group ids.
 */
class ParticleShapeStatistics
{
public:

  constexpr static int VDimension = 3;

  ParticleShapeStatistics() {}
  ~ParticleShapeStatistics() {}

  typedef typename itk::ParticlePositionReader<3>::PointType PointType;

  int DoPCA(std::vector< std::vector<PointType> > global_pts, int domainsPerShape = 1);

 /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Loads a set of point files and pre-computes some statistics. */
  int ImportPoints( std::vector<vnl_vector<double> > points, std::vector<int> group_ids );

  /** Loads a set of point files and pre-computes some statistics. */
  int ReadPointFiles(const char *fname);

  /** Reloads a set of point files and recomputes some statistics. */
  int ReloadPointFiles( );

  /** Writes a text file in comma-separated format.  Suitable for reading into
      excel or R or Matlab for analysis. */
  int WriteCSVFile(const char *s);
  int WriteCSVFile(const std::string &s)
  { return this->WriteCSVFile(s.c_str()); }
  int WriteCSVFile2(const char *s);
  int WriteCSVFile2(const std::string &s)
  { return this->WriteCSVFile2(s.c_str()); }

  /** Computes PCA modes from the set of correspondence mode positions.
      Requires that ReadPointFiles be called first. */
  int ComputeModes();

  /** Computes the principal component loadings, or projections onto the
      principal componenent axes for each of the samples.  ComputeModes must be
      called first. */
  int PrincipalComponentProjections();

  /** Computes fishers linear discriminant line for best group separation. */
  int FisherLinearDiscriminant(unsigned int);

  /** Returns the sample size. */
  inline  int SampleSize() const
  { return m_numSamples; }
  inline int Group1SampleSize() const
  { return m_numSamples1; }
  inline int Group2SampleSize() const
  { return m_numSamples2; }

  /** Returns the number of dimensions (this is number of points times Dimension) */
  inline int NumberOfDimensions() const
  { return m_numDimensions; }

  /** Returns the group ids */
  inline int GroupID(unsigned int i ) const
  { return m_groupIDs[i]; }
  const std::vector<int> &GroupID() const
  { return m_groupIDs;}

  /** Returns the eigenvectors/values. */
  const vnl_matrix<double> &Eigenvectors() const
  { return m_eigenvectors; }
  const vnl_vector<double> &Eigenvalues() const
  { return m_eigenvalues; }

  /** Returns the mean shape. */
  const vnl_vector<double> &Mean() const
  { return m_mean; }
  const vnl_vector<double> &Group1Mean() const
  { return m_mean1; }
  const vnl_vector<double> &Group2Mean() const
  { return m_mean2; }

  // Returns group2 - group1 mean
  const vnl_vector<double> &NormalizedGroupDifference() const
  { return m_groupdiffnorm;}
  const vnl_vector<double> &GroupDifference() const
  { return m_groupdiff;}

 /** Returns the median shape for the set of shapes with Group ID equal to the
      integer argument.  For example, ComputeMedianShape(0) returns the median
      shape for the set of shapes with m_groupIDs == 0. The median shape is
      defined as the shape with the minimum sum of Euclidean L1 norms to all
      other shapes in that group.  Arguments passed to this function are set to
      the index number of the median shape for Group A and Group B,
      respectively.*/
  int ComputeMedianShape(const int);

    /** Returns the euclidean L1 norm between shape a and b */
  double L1Norm(unsigned int a, unsigned int b);

  /** Returns the component loadings */
  const vnl_matrix<double> &PCALoadings() const
  { return m_principals; }

  /** Returns the Fisher linear discriminant */
  const vnl_vector<double> &FishersLDA() const
  { return m_fishersLD; }

  /** Returns the shape matrix*/
  const vnl_matrix<double> &ShapeMatrix() const
  {return m_shapes; }

  /** Returns the shape with the mean subtracted */
  const vnl_matrix<double> &RecenteredShape() const
  { return m_pointsMinusMean; }

  std::vector<double> PercentVarByMode ()
  {return m_percentVarByMode;}

  /** Computes a simple linear regression of the first list of values with
      respect to the second y=a + bx. Returns the estimated parameters a & b.
       Returns 0 on success and -1 on fail.*/
  int SimpleLinearRegression(const std::vector<double> &y,
                              const std::vector<double> &x,
                              double &a, double &b) const;


  double get_compactness(const int num_modes);
  double get_specificity(const int num_modes);
  double get_generalization(const int num_modes);

private:

  void compute_evaluation(int num_modes);

  unsigned int m_numSamples1;
  unsigned int m_numSamples2;
  unsigned int m_numSamples;
  unsigned int m_domainsPerShape;
  unsigned int m_numDimensions;
  std::vector<int> m_groupIDs;

  vnl_matrix<double> m_pooled_covariance;
  vnl_matrix<double> m_eigenvectors;
  vnl_vector<double> m_eigenvalues;
  vnl_vector<double> m_mean;
  vnl_vector<double> m_mean1;
  vnl_vector<double> m_mean2;
  vnl_matrix<double> m_pointsMinusMean;
  vnl_matrix<double> m_shapes;
  vnl_matrix<double> m_projectedPMM1;
  vnl_matrix<double> m_projectedPMM2;
  vnl_vector<double> m_projectedMean1;
  vnl_vector<double> m_projectedMean2;
  std::vector<double> m_fishersProjection;
  std::vector<double> m_percentVarByMode;
  vnl_vector<double> m_fishersLD;
  vnl_matrix<double> m_principals;

  vnl_vector<double> m_groupdiff;
  vnl_vector<double> m_groupdiffnorm;

  // used to keep the points' files that needs to be reloaded when new updates come in.
  std::vector< std::string > m_pointsfiles;

  Eigen::MatrixXd m_Matrix;
  double compactness_ = 0;
  double specificity_ = 0;
  double generalization_ = 0;
  bool evaluation_ready_ = false;
  int evaluation_modes_ = 0;

};

