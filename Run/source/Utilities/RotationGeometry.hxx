/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: RotationGeometry.hxx,v $
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
  File: RotationGeometry.hxx
  Author: Tom Fletcher

  RotationGeometry defines a linear geometry on the space of diffusion
  tensors. The inner product is the standard Frobenius inner product between
  matrices.

=============================================================================*/

#ifndef __RotationGeometry_hxx
#define __RotationGeometry_hxx

#include "vnl/vnl_quaternion.h"
#include "RiemannianGeometry.hxx"

template<class ScalarType>
class RotationGeometry :
  public RiemannianGeometry<vnl_quaternion<ScalarType>,
                            vnl_vector_fixed<ScalarType, 3>,
                            ScalarType>
{
public:
  typedef vnl_quaternion<ScalarType> PointType;
  typedef vnl_vector_fixed<ScalarType, 3> TangentType;
  typedef RiemannianGeometry<PointType, TangentType, ScalarType> SuperClass;

  RotationGeometry() : SuperClass() { this->zeroVector.fill(0.0); }

  virtual ScalarType innerProduct(const PointType & base,
                                  const TangentType & v,
                                  const TangentType & w)
  {    return (v[0]*w[0] + v[1]*w[1] + v[2]*w[2]);  }

  virtual PointType expMap(const PointType & base, const TangentType & t)
  {
    ScalarType w = t.magnitude();
    TangentType axis = t / (w + 1.0e-12);
    PointType p(axis, w) ;  // reconstitute a quaternion from the tangent t
    return (p * base);
  }
  virtual TangentType logMap(const PointType & base, const PointType & p)
  {
    PointType R = p * base.inverse(); // relative angle
    return ( R.axis() * R.angle() );     // vectorized quaternion
  }

  virtual int getDimension() { return 3; }
};


#endif
