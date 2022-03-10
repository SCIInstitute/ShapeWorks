---
title: Libs/Optimize/ParticleSystem/itkParticlePointIndexPair.h

---

# Libs/Optimize/ParticleSystem/itkParticlePointIndexPair.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[itk::ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePointIndexPair.h,v $
  Date:      $Date: 2011/03/23 22:40:10 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticlePointIndexPair_h
#define __itkParticlePointIndexPair_h

#include "itkPoint.h"
namespace itk {
template <unsigned int VDimension>
struct ParticlePointIndexPair
{
  typedef itk::Point<double, VDimension> PointType;
  ParticlePointIndexPair() {}
  ParticlePointIndexPair(const PointType &p, unsigned int i=0)
  {
    Point = p;
    Index = i;
  }
  ParticlePointIndexPair(const ParticlePointIndexPair &o)
  { this->operator=(o); }
  const ParticlePointIndexPair &operator=(const ParticlePointIndexPair &o)
  {
    Point = o.Point;
    Index = o.Index;
    return *this;
  }
  PointType Point;
  unsigned int Index;
};

}
#endif
```


-------------------------------

Updated on 2022-03-10 at 23:35:01 +0000
