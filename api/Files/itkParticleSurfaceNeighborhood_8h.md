---
title: Libs/Optimize/ParticleSystem/itkParticleSurfaceNeighborhood.h

---

# Libs/Optimize/ParticleSystem/itkParticleSurfaceNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSurfaceNeighborhood.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSurfaceNeighborhood_h
#define __itkParticleSurfaceNeighborhood_h

#include "itkParticleRegionNeighborhood.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{
template <class TImage>
class ITK_EXPORT ParticleSurfaceNeighborhood : public ParticleRegionNeighborhood<TImage::ImageDimension>
{
public:
  typedef TImage ImageType;
  typedef ParticleSurfaceNeighborhood Self;
  typedef ParticleRegionNeighborhood<TImage::ImageDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  typedef typename ImageType::PixelType NumericType;
  typedef typename Superclass::PointTreeType PointTreeType;

  typedef  vnl_vector_fixed<NumericType, TImage::ImageDimension> GradientVectorType;

  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);

  itkNewMacro(Self);

  itkTypeMacro(ParticleSurfaceNeighborhood, ParticleRegionNeighborhood);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef ParticleImplicitSurfaceDomain<typename TImage::PixelType> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &,
          std::vector<double>&, double) const override;
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &, double) const override;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetWeightingEnabled(bool is_enabled)
  {
    m_WeightingEnabled = is_enabled;
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }


protected:
  ParticleSurfaceNeighborhood() : m_FlatCutoff(0.30)  {  }
  virtual ~ParticleSurfaceNeighborhood() {};

private:
  ParticleSurfaceNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  double m_FlatCutoff;
  bool m_WeightingEnabled{true};

};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleSurfaceNeighborhood+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleSurfaceNeighborhood.txx"
#endif

#include "itkParticleSurfaceNeighborhood.txx"

#endif
```


-------------------------------

Updated on 2021-12-13 at 17:35:59 +0000
