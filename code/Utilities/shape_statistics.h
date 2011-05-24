/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: shape_statistics.h,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
/// WARNING: THIS CLASS IS OBSOLETE AND NOT USED.  NEEDS TO BE REMOVED

#ifndef _shape_statistics_h
#define _shape_statistics_h

#include <iostream>
#include "CS6350.h"
#include <vector>
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "param.h"
#include "vnl/vnl_matrix.h"
#include "itkParticlePositionReader.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "itkParticlePositionWriter.h"

/**
 * \class shape_statistics
 * This class computes various statistics for a set of correspondence positions
 * and group ids.
 */
class shape_statistics
{
public:
  shape_statistics() {}
  ~shape_statistics() {}

  /** Loads a set of point files and pre-computes some statistics. */
  int ReadPointFiles(param::parameterFile &);

  /** Writes a text file in comma-separated format.  Suitable for reading into
      excel or R or Matlab for analysis. */
  int WriteCSVFile(const char *s);
  int WriteCSVFile(const std::string &s)
  { return this->WriteCSVFile(s.c_str()); }

  /** Computes PCA modes from the set of correspondence mode positions.
      Requires that ReadPointFiles be called first. */
  int ComputeModes();

  /** Computes the principal component loadings, or projections onto the
      principal componenent axes for each of the samples.  ComputeModes must be
      called first. */
  int PrincipalComponentProjections();

  /** Computes fishers linear discriminant line for best group separation. */
  int FisherLinearDiscriminant(int);

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
  int m_top95;
  vnl_matrix<double> m_principals;
  
protected:

  
};

#endif
