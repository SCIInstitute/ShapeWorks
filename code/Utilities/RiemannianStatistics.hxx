/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: RiemannianStatistics.hxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
/*=============================================================================
  File: RiemannianStatistics.hxx
  Author: Tom Fletcher

  RiemannianStatistics computes statistics of data on a Riemannian manifold,
  including mean, covariance, and principal geodesic analysis (PGA). (PGA is the
  generalization of PCA to manifold geometries). The class takes a pointer to a
  RiemannianGeometry class, which defines how the statistics are computed. In
  other words, the statistics may be linear or nonlinear based on the definition
  of the Riemannian metric. For details on the statistics computations, see the
  references

  Fletcher, P.T., Joshi, S., Lu, C., Pizer, S.M. Principal Geodesic Analysis for
  the Study of Nonlinear Statistics of Shape, IEEE Transactions on Medical
  Imaging, vol. 23, no. 8, pp. 995-1005, IEEE, Aug. 2004.

  P.T. Fletcher, S. Joshi. Principal Geodesic Analysis on Symmetric Spaces:
  Statistics of Diffusion Tensors. CVAMIA Workshop part of ECCV 2004,
  LNCS 3117, Springer-Verlag, pp. 87-98, 2004.
  http://www.cs.unc.edu/~fletcher/papers/FletcherCVAMIA04_DTStats.pdf

=============================================================================*/

#ifndef __RiemannianStatistics_hxx
#define __RiemannianStatisitcs_hxx

#include <vector>
#include <vnl/vnl_sym_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include "RiemannianGeometry.hxx"

template<class PointType, class TangentType, class ScalarType = double>
class RiemannianStatistics
{
public:
  typedef RiemannianGeometry<PointType, TangentType, ScalarType>
    RiemannianGeometryType;

  typedef std::vector<PointType> PointListType;
  typedef std::vector<ScalarType> ScalarListType;

  typedef vnl_sym_matrix<ScalarType> CovarianceType;
  typedef vnl_symmetric_eigensystem<ScalarType> PGAType;
  typedef vnl_diag_matrix<ScalarType> PGAVariancesType;
  typedef vnl_matrix<ScalarType> PGAVectorsMatrixType;

  RiemannianStatistics(RiemannianGeometryType * _riemannianGeometry,
                   const ScalarType & _stepSize = 1.0)
  {
    riemannianGeometry = _riemannianGeometry;
    stepSize = _stepSize;
  }

  RiemannianGeometryType * getRiemannianGeometry()
  {
    return riemannianGeometry;
  }

  void computeMean(const PointListType & pointList, PointType & mean) const;

  void computeWeightedAve(const ScalarListType & weightList,
                          const PointListType & pointList,
                          PointType & weightedAve) const;

  void computeMeanAndCovariance(const PointListType & pointList,
                                PointType & mean,
                                CovarianceType & covariance) const;

  void computeMeanAndPGA(const PointListType & pointList,
                         PointType & mean,
                         PGAVariancesType & pgaVariances,
                         PGAVectorsMatrixType & pgaVectorsMatrix) const;

  // Generates random point from isotropic Gaussian density.
  PointType randomGaussianPoint(const PointType & mean, ScalarType sigma) const;

private:
  RiemannianGeometryType * riemannianGeometry;
  ScalarType stepSize;

  //  static const ScalarType EPSILON = 1.0e-12;
};

#include "RiemannianStatistics.txx"

#endif
