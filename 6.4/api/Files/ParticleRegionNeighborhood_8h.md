---
title: Libs/Optimize/Neighborhood/ParticleRegionNeighborhood.h

---

# Libs/Optimize/Neighborhood/ParticleRegionNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**  |




## Source code

```cpp
#pragma once

#include "ParticleNeighborhood.h"
#include "PowerOfTwoPointTree.h"

namespace shapeworks {
class ParticleRegionNeighborhood : public ParticleNeighborhood {
 public:
  constexpr static unsigned int VDimension = 3;

  typedef ParticleRegionNeighborhood Self;
  typedef ParticleNeighborhood Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;
  using DomainType = ParticleDomain;

  itkNewMacro(Self);

  itkTypeMacro(ParticleRegionNeighborhood, ParticleNeighborhood);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;

  typedef typename Superclass::PointVectorType PointVectorType;

  typedef PowerOfTwoPointTree PointTreeType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, double) const;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetDomain(DomainType::Pointer p) override;

  itkSetMacro(TreeLevels, unsigned int);
  itkGetMacro(TreeLevels, unsigned int);

  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    os << indent << "m_TreeLevels = " << m_TreeLevels << std::endl;
    m_Tree->PrintSelf(os, indent);
    Superclass::PrintSelf(os, indent);
  }

  virtual void AddPosition(const PointType& p, unsigned int idx, int threadId = 0);
  virtual void SetPosition(const PointType& p, unsigned int idx, int threadId = 0);
  virtual void RemovePosition(unsigned int idx, int threadId = 0);

 protected:
  ParticleRegionNeighborhood() : m_TreeLevels(3) {
    m_Tree = PointTreeType::New();
    m_IteratorMap = IteratorMapType::New();
  }
  virtual ~ParticleRegionNeighborhood(){};

  struct IteratorNodePair {
    typedef typename PointTreeType::NodePointerType NodePointerType;
    typedef typename PointTreeType::PointListType::iterator IteratorType;

    IteratorNodePair() {}
    ~IteratorNodePair() {}
    IteratorNodePair(const IteratorType& p, const NodePointerType& n) {
      Iterator = p;
      NodePointer = n;
    }

    IteratorNodePair(const IteratorNodePair& o) { this->operator=(o); }
    const IteratorNodePair& operator=(const IteratorNodePair& o) {
      Iterator = o.Iterator;
      NodePointer = o.NodePointer;
      return *this;
    }

    IteratorType Iterator;
    NodePointerType NodePointer;
  };

  typedef GenericContainer<IteratorNodePair> IteratorMapType;

 protected:
  typename PointTreeType::Pointer m_Tree;
  typename IteratorMapType::Pointer m_IteratorMap;
  unsigned int m_TreeLevels;

 private:
  ParticleRegionNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);              // purposely not implemented
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2023-04-03 at 19:48:11 +0000
