/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: MetricSpace.hxx,v $
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
  File: MetricSpace.hxx
  Author: Tom Fletcher

  MetricSpace is an abstract class for defining distances between objects. It's
  only member function is Distance().

=============================================================================*/

#ifndef __MetricSpace_hxx
#define __MetricSpace_hxx

#include <math.h>

template<class PointType, class DistanceType=double>
class MetricSpace
{
public:
  virtual DistanceType distance(const PointType & p1, const PointType & p2) = 0;
};


// L2MetricSpace defines the L^2 metric for vector/scalar types.
template<class PointType, class DistanceType=double>
class L2MetricSpace : public MetricSpace<PointType, DistanceType>
{
public:
  virtual DistanceType Distance(const PointType & p1, const PointType & p2);
};

template<class PointType, class DistanceType>
DistanceType
L2MetricSpace<PointType, DistanceType>::Distance(const PointType & p1,
                                                 const PointType & p2)
{
  return sqrt((p1 - p2) * (p1 - p2));
}

#endif
