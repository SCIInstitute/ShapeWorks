---
title: Libs/Optimize/ParticleSystem/itkParticleRegionNeighborhood.h

---

# Libs/Optimize/ParticleSystem/itkParticleRegionNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleRegionNeighborhood.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleRegionNeighborhood_h
#define __itkParticleRegionNeighborhood_h

#include "itkParticleNeighborhood.h"
#include "itkPowerOfTwoPointTree.h"

namespace itk
{
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleRegionNeighborhood : public ParticleNeighborhood<VDimension>
{
public:
  typedef ParticleRegionNeighborhood Self;
  typedef ParticleNeighborhood<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleRegionNeighborhood, ParticleNeighborhood);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef typename Superclass::DomainType DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  typedef PowerOfTwoPointTree<VDimension> PointTreeType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, double) const;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  virtual void SetDomain( DomainType *p);

  itkSetMacro(TreeLevels, unsigned int);
  itkGetMacro(TreeLevels, unsigned int);

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    os << indent << "m_TreeLevels = " << m_TreeLevels << std::endl;
    m_Tree->PrintSelf(os, indent);
    Superclass::PrintSelf(os, indent);
  }

  virtual void AddPosition(const PointType &p, unsigned int idx, int threadId = 0);
  virtual void SetPosition(const PointType &p, unsigned int idx, int threadId = 0);
  virtual void RemovePosition(unsigned int idx, int threadId = 0);

protected:
  ParticleRegionNeighborhood() : m_TreeLevels(3)
  {
    m_Tree = PointTreeType::New();
    m_IteratorMap = IteratorMapType::New();
  }
  virtual ~ParticleRegionNeighborhood() {};

  struct IteratorNodePair
  {
    typedef typename PointTreeType::NodePointerType NodePointerType;
    typedef typename PointTreeType::PointListType::iterator IteratorType;

    IteratorNodePair() {}
    ~IteratorNodePair() {}
    IteratorNodePair(const IteratorType &p, const NodePointerType &n)
    {
      Iterator = p;
      NodePointer = n;
    }
   
    IteratorNodePair(const IteratorNodePair &o)
    { this->operator=(o); }
    const IteratorNodePair &operator=(const IteratorNodePair &o)
    {
      Iterator = o.Iterator;
      NodePointer = o.NodePointer;
      return *this;
    }

    IteratorType Iterator;
    NodePointerType NodePointer;
 };
  
  typedef ParticleContainer<IteratorNodePair> IteratorMapType;

protected:
  typename PointTreeType::Pointer m_Tree;
  typename IteratorMapType::Pointer m_IteratorMap;
  unsigned int m_TreeLevels;

 
private:
  ParticleRegionNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleRegionNeighborhood+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleRegionNeighborhood.txx"
#endif

#include "itkParticleRegionNeighborhood.txx"

#endif
```


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000
