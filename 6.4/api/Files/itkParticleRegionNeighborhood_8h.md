---
title: Libs/Optimize/itkParticleRegionNeighborhood.h

---

# Libs/Optimize/itkParticleRegionNeighborhood.h



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
#pragma once

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
  using DomainType = shapeworks::ParticleDomain;

  itkNewMacro(Self);

  itkTypeMacro(ParticleRegionNeighborhood, ParticleNeighborhood);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;

  typedef typename Superclass::PointVectorType PointVectorType;

  typedef PowerOfTwoPointTree<VDimension> PointTreeType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, double) const;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetDomain(DomainType::Pointer p) override;

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


#include "itkParticleRegionNeighborhood.txx"
```


-------------------------------

Updated on 2022-12-10 at 09:04:10 +0000
