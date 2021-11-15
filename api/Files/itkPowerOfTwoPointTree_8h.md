---
title: Libs/Optimize/ParticleSystem/itkPowerOfTwoPointTree.h

---

# Libs/Optimize/ParticleSystem/itkPowerOfTwoPointTree.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[itk::powstruct](../Classes/structitk_1_1powstruct.md)**  |
| struct | **[itk::powstruct< a, 0 >](../Classes/structitk_1_1powstruct_3_01a_00_010_01_4.md)**  |
| class | **[itk::PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md)**  |
| class | **[itk::PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md)**  |




## Source code

```cpp
#pragma once

#include "itkParticlePointIndexPair.h"
#include "itkLightObject.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkTreeContainer.h"
#include "itkPoint.h"
#include <list>

namespace itk
{

template <int a, int b>
struct powstruct
{
  static const int c = a * powstruct<a,b-1>::c;
};
template<int a>
struct powstruct<a,0>
{
  static const int c = 1;
};

template <unsigned int VDimension>
class PowerOfTwoPointTreeNode : public LightObject
{
public:
  typedef PowerOfTwoPointTreeNode   Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( PowerOfTwoPointTreeNode, LightObject);

  itkNewMacro(Self);

  itkStaticConstMacro(BranchesPerNode, int, (powstruct<2, VDimension>::c));

  typedef Point<double, VDimension> PointType;

  typedef std::list<ParticlePointIndexPair<VDimension> > PointListType;
  
  bool operator==(const PowerOfTwoPointTreeNode &o) const
  {
    if  ((m_LowerBound == o.m_LowerBound) && (m_UpperBound == o.m_UpperBound) ) return true;
    else return false;
  }

  bool IsLeaf() const
  {
    if (m_Branches[0].GetPointer() != 0) return false;
    else return true;
  }

  typename PointListType::iterator InsertElement(const ParticlePointIndexPair<VDimension> &elem)
  {
    return m_List.insert(m_List.end(), elem);
  }

  bool Contains( const PointType &p ) const
  {
    for (unsigned int i = 0; i < VDimension; i++)
      {
      if (p[i] < m_LowerBound[i] || p[i] > m_UpperBound[i]) return false;
      }
    return true;
  }
  
  void SetLowerBound( const PointType &p)
  { m_LowerBound = p; }
  const PointType &GetLowerBound() const
  { return  m_LowerBound; }
  void SetUpperBound( const PointType &p)
  { m_UpperBound = p; }
  const PointType &GetUpperBound() const
  { return  m_UpperBound; }

  typename Self::Pointer *GetBranches()
  { return m_Branches; }
  const typename Self::Pointer *GetBranches() const
  { return m_Branches; }

  typename Self::Pointer &GetBranch(unsigned int i)
  { return m_Branches[i]; }
  const  typename Self::Pointer &GetBranch(unsigned int i) const
  { return m_Branches[i]; }
  
  void SetBranch(unsigned int b, typename Self::Pointer n)
  { this->GetBranch(b) = n; }
  
  void PrintSelf(std::ostream& os, Indent indent) const;

  const PointListType &GetList() const
  { return m_List; }
  PointListType &GetList()
  { return m_List; }
  
protected:
  PowerOfTwoPointTreeNode()  {  }
  ~PowerOfTwoPointTreeNode() {}
  
  PowerOfTwoPointTreeNode(const PowerOfTwoPointTreeNode &o); // purposely not implemented
  const PowerOfTwoPointTreeNode & operator=(const PowerOfTwoPointTreeNode &o);
  
private:
  PointType m_LowerBound;
  PointType m_UpperBound;
  PointListType m_List;
  
  typename Self::Pointer m_Branches[powstruct<2,VDimension>::c];
};

 template <unsigned int VDimension>
 class ITK_EXPORT PowerOfTwoPointTree : public DataObject
{
public:
  typedef PowerOfTwoPointTree Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef PowerOfTwoPointTreeNode<VDimension> NodeType;

  typedef typename NodeType::Pointer NodePointerType;
  
  typedef typename PowerOfTwoPointTreeNode<VDimension>::PointType PointType;

  typedef typename NodeType::PointListType PointListType;
  typedef std::vector<typename PointListType::const_iterator> PointIteratorListType;

  itkNewMacro(Self);

  itkTypeMacro(PowerOfTwoPointTree, DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  itkStaticConstMacro(BranchesPerNode, int, (powstruct<2, VDimension>::c));
  
  itkGetMacro(Depth, unsigned int);

  void ConstructTree(const PointType &, const PointType &, unsigned int);

  PointIteratorListType FindPointsInRegion(const PointType &, const PointType &) const;
  unsigned int FindPointsInRegion(const PointType &, const PointType &, PointIteratorListType &) const;

  NodePointerType GetNode(const PointType &);
  const NodePointerType GetNode(const PointType &) const;

  itkGetObjectMacro(Root, NodeType);
  itkSetObjectMacro(Root, NodeType);

  typename PointListType::iterator AddPoint(const PointType &,
                                            unsigned int, NodePointerType &);
  typename PointListType::iterator AddPoint(const PointType &p,
                                            unsigned int i)
  {
    NodePointerType node = NodeType::New();
    return this->AddPoint(p, i, node);
  }

  bool Overlap(const NodePointerType &, const PointType &,  const PointType &) const;

  inline bool RegionContains(const PointType &p, const PointType &lowerbound,
                             const PointType &upperbound) const
  {
    for (unsigned int i = 0; i < VDimension; i++)
      {
      if (p[i] < lowerbound[i] || p[i] >= upperbound[i]) return false;
      }
    return true;
  }
  
  void PrintSelf(std::ostream& os, Indent indent) const;
protected:
  PowerOfTwoPointTree()
  {
    m_Depth = 0;
  }
  virtual ~PowerOfTwoPointTree() {}

  void BranchNode(NodePointerType &, unsigned int);

  void FindOneNodeInRegion(const NodePointerType&, const PointType &, const PointType &,
                           PointIteratorListType &) const;
  
private:
  PowerOfTwoPointTree(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  NodePointerType m_Root;
  
  unsigned int m_Depth;
};

} // end namespace itk

#include "itkPowerOfTwoPointTree.txx"
```


-------------------------------

Updated on 2021-11-15 at 01:30:08 +0000
