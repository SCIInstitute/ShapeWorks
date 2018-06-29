/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: RiemannianGeometry.hxx,v $
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
  File: RiemannianGeometry.hxx
  Author: Tom Fletcher

  RiemannianGeometry is an abstract class for defining the geometry on a
  Riemannian manifold. The geometry is defined via a Riemannian metric, and this
  leads to geodesics and distances. Note that the geometry does not need to be
  curved, in other words, a linear geometry is also possible in this framework.

=============================================================================*/

#ifndef __RiemannianGeometry_hxx
#define __RiemannianGeometry_hxx

#include "MetricSpace.hxx"

template<class PointType, class TangentType, class ScalarType = double>
class RiemannianGeometry : public MetricSpace<PointType, ScalarType>
{
public:

  RiemannianGeometry() {}

  // Defines the Riemannian metric at a base point for two tangent vectors.
  virtual ScalarType innerProduct(const PointType & base, const TangentType & v,
                                  const TangentType & w) = 0;
  virtual ScalarType norm(const PointType & base, const TangentType & v);
  virtual ScalarType normSquared(const PointType & base, const TangentType & v);

  // ExpMap gives the Riemannian exponential map (i.e., geodesic segment
  // starting at base with initial velocity v)
  virtual PointType expMap(const PointType & base, const TangentType & v) = 0;

  // LogMap is the inverse of the exponential map. It returns the initial
  // veloctiy vector for the geodesic segment between base and p.
  virtual TangentType logMap(const PointType & base, const PointType & p) = 0;

  // Geodesic distance between tensors a and b.
  virtual ScalarType distance(const PointType & a, const PointType & b);
  virtual ScalarType distanceSquared(const PointType & a, const PointType & b);

  virtual int getDimension() = 0;
  virtual TangentType & getZeroVector() { return zeroVector; }

protected:
  // This should be set in the constructor of a derived class
  TangentType zeroVector;
};

template<class PointType, class TangentType, class ScalarType>
ScalarType
RiemannianGeometry<PointType, TangentType, ScalarType>::
norm(const PointType & base, const TangentType & v)
{
  return sqrt(normSquared(base, v));
}

template<class PointType, class TangentType, class ScalarType>
ScalarType
RiemannianGeometry<PointType, TangentType, ScalarType>::
normSquared(const PointType & base, const TangentType & v)
{
  return innerProduct(base, v, v);
}

template<class PointType, class TangentType, class ScalarType>
ScalarType
RiemannianGeometry<PointType, TangentType, ScalarType>::
distance(const PointType & a, const PointType & b)
{
  return norm(a, logMap(a, b));
}

template<class PointType, class TangentType, class ScalarType>
ScalarType
RiemannianGeometry<PointType, TangentType, ScalarType>::
distanceSquared(const PointType & a, const PointType & b)
{
  return normSquared(a, logMap(a, b));
}

#endif
