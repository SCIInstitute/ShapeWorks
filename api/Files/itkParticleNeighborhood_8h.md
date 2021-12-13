---
title: Libs/Optimize/ParticleSystem/itkParticleNeighborhood.h

---

# Libs/Optimize/ParticleSystem/itkParticleNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleNeighborhood.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleNeighborhood_h
#define __itkParticleNeighborhood_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkParticleDomain.h"
#include "itkParticleContainer.h"
#include "itkParticlePointIndexPair.h"
#include <vector>

namespace itk
{
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleNeighborhood : public DataObject
{
public:
  typedef ParticleNeighborhood Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleNeighborhood, DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef Point<double, VDimension> PointType;

  typedef ParticleDomain DomainType;
  
  typedef ParticleContainer<PointType> PointContainerType;

  typedef std::vector<ParticlePointIndexPair<VDimension> > PointVectorType;

  itkSetObjectMacro(PointContainer, PointContainerType);
  itkGetConstObjectMacro(PointContainer, PointContainerType);

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, double) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &,
                                                 double) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &, std::vector<double> &,
                                                 double) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual unsigned int  FindNeighborhoodPoints(const PointType &, int idx, double, PointVectorType &) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
    return 0;
  }

  itkSetObjectMacro(Domain, DomainType);
  itkGetConstObjectMacro(Domain, DomainType);

  virtual void AddPosition(const PointType &p, unsigned int idx, int threadId = 0) {}
  virtual void SetPosition(const PointType &p, unsigned int idx, int threadId = 0) {}
  virtual void RemovePosition(unsigned int idx, int threadId = 0) {}
  
protected:
  ParticleNeighborhood() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleNeighborhood() {};

private:
  ParticleNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename PointContainerType::Pointer m_PointContainer;
  typename DomainType::Pointer m_Domain;
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleNeighborhood+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleNeighborhood.txx"
#endif

#endif
```


-------------------------------

Updated on 2021-12-13 at 17:35:59 +0000
