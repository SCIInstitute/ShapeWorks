/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMPointTree_h
#define __itkPSMPointTree_h

#include "itkPSMPointIndexPair.h"
#include "itkLightObject.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkTreeContainer.h"
#include "itkPoint.h"
#include <list>

namespace itk
{

/** Compute pow(a,b)=c at compile time.  */
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

/**
 * \class PSMPointTreeNode
 *
 * A node object for use in the PSMPointTree.  This class contains a
 * bounding box, a set of child (branch) node pointers, and a linked list that
 * may be filled with Points and Indices (unsigned ints).  The single template
 * parameter is the dimensionality of the Points.  This class was designed for
 * use as a binning structure for itkPSMNeighborhood classes.
 *
 */
template <unsigned int VDimension>
class PSMPointTreeNode : public LightObject
{
public:
  /** Standard class typedefs. */
  typedef PSMPointTreeNode   Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( PSMPointTreeNode, LightObject);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** The number of branches at each node of the tree, which is 2^VDimension,
      e.g. 4 in 2D, 8 in 3D, etc. */
  itkStaticConstMacro(BranchesPerNode, int, (powstruct<2, VDimension>::c));

  /** Point type stored in the leaf nodes. */
  typedef Point<double, VDimension> PointType;

  /** List type for storing lists of points+indices. */
  typedef std::list<PSMPointIndexPair<VDimension> > PointListType;
  
  /** Equivalence operator, ignores value type. */
  bool operator==(const PSMPointTreeNode &o) const
  {
    if  ((m_LowerBound == o.m_LowerBound) && (m_UpperBound == o.m_UpperBound) ) return true;
    else return false;
  }

  /** Returns true if this is a leaf node (has no branches) and false
      otherwise. The assumption is that a non-leaf node cannot have any empty
      branches.  Each level of the tree must contain BranchesPerNode
      divisions.  This assumption speeds up certain types of processing.*/
  bool IsLeaf() const
  {
    if (m_Branches[0].GetPointer() != 0) return false;
    else return true;
  }

  /** Insert a PointIndexPair into the list associated with this node. */
  typename PointListType::iterator InsertElement(const PSMPointIndexPair<VDimension> &elem)
  {
    return m_List.insert(m_List.end(), elem);
  }

  /** Returns true if the bounding box in this node contains the given point
      and false otherwise. */
  bool Contains( const PointType &p ) const
  {
    for (unsigned int i = 0; i < VDimension; i++)
      {
      if (p[i] < m_LowerBound[i] || p[i] >= m_UpperBound[i]) return false;
      }
    return true;
  }
  
  /** Set/Get the lower and upper bounds of the region described by this node. */
  void SetLowerBound( const PointType &p)
  { m_LowerBound = p; }
  const PointType &GetLowerBound() const
  { return  m_LowerBound; }
  void SetUpperBound( const PointType &p)
  { m_UpperBound = p; }
  const PointType &GetUpperBound() const
  { return  m_UpperBound; }

  /** Return the data structure holding branch (child) nodes. */
  typename Self::Pointer *GetBranches()
  { return m_Branches; }
  const typename Self::Pointer *GetBranches() const
  { return m_Branches; }

  /** Return a specific branch */
  typename Self::Pointer &GetBranch(unsigned int i)
  { return m_Branches[i]; }
  const  typename Self::Pointer &GetBranch(unsigned int i) const
  { return m_Branches[i]; }
  
  /** Set the node for a specific branch of this node. */
  void SetBranch(unsigned int b, typename Self::Pointer n)
  { this->GetBranch(b) = n; }
  
  /** Standard ITK PrintSelf method. */  
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Get the list of elements which contain points and associated indices. */
  const PointListType &GetList() const
  { return m_List; }
  PointListType &GetList()
  { return m_List; }
  
protected:
  PSMPointTreeNode()  {  }
  ~PSMPointTreeNode() {}
  
  PSMPointTreeNode(const PSMPointTreeNode &o); // purposely not implemented
  const PSMPointTreeNode & operator=(const PSMPointTreeNode &o);
  
private:
  PointType m_LowerBound;
  PointType m_UpperBound;
  PointListType m_List;
  
  typename Self::Pointer m_Branches[powstruct<2,VDimension>::c];
};

/** \class PSMPointTree
 *
 *  A tree data container, templated over node type, whose nodes are
 *  associated with bounding boxes in a rectangular domain, and that
 *  has 2^D branches at each node, where D is the dimensionality of
 *  the domain.  In 2D this is a quad-tree, and in 3D this is an
 *  octree, etc.  The tree is constructed by specifying a region and a
 *  tree depth, then calling ConstructTree().  This class was designed
 *  for use as a multidimensional quad/octree binning structure for
 *  itkPSMNeighborhood classes.
 */
 template <unsigned int VDimension>
 class ITK_EXPORT PSMPointTree : public DataObject
{
public:
  /** Standard class typedefs */
  typedef PSMPointTree Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

    /** Shorthand for the object pointed to by each node.   */
  typedef PSMPointTreeNode<VDimension> NodeType;

  /** The real node type, which is  a actually pointer  to what we are calling NodeTypes. */
  typedef typename NodeType::Pointer NodePointerType;
  
  /** Point type used by nodes for upper and lower bounds. */
  typedef typename PSMPointTreeNode<VDimension>::PointType PointType;

  /** Types defined by the NodeType. */
  typedef typename NodeType::PointListType PointListType;
  typedef std::vector<typename PointListType::const_iterator> PointIteratorListType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMPointTree, DataObject);

  /** Dimensionality of the domain. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Number of children per node. */
  itkStaticConstMacro(BranchesPerNode, int, (powstruct<2, VDimension>::c));
  
  /** Set/Get the depth of the tree.  This is the number of levels in the
      tree. */
  itkGetMacro(Depth, unsigned int);

  /** Construct the tree to the specified depth.  The bounding box of the root
      node is specified with the lower bound and upper bound points
      respectively. */
  void ConstructTree(const PointType &, const PointType &, unsigned int);

  /** Return a list of PointListType iterators (effectively pointers to points,
      see PSMPointTreeNode) to points and their associated indicies
      that are stored in this tree and are contained within the specified
      bounding box region. The bounding box is specified with two points, in
      this order: a lower bound followed by an upper bound.  */
  PointIteratorListType FindPointsInRegion(const PointType &, const PointType &) const;
  unsigned int FindPointsInRegion(const PointType &, const PointType &, PointIteratorListType &) const;

  /** Return the node associated with the domain region that contains the given
      point. */
  NodePointerType GetNode(const PointType &);
  const NodePointerType GetNode(const PointType &) const;

  /** Set/Get the root node of the tree. */
  itkGetObjectMacro(Root, NodeType);
  itkSetObjectMacro(Root, NodeType);

  /** Associates a point and, optionally, an index with the appropriate leaf
      node.  This method starts at the root of the tree and uses the Contains
      method to query branches at each node, following the first branch it
      finds whose bounding box contains this point.  When it reaches a leaf
      node, the point is added to that node's list, along with the specified
      index (if any). This method returns an iterator pointing to the new list
      element (e.g., for quick deletion or reference of the point) and,
      optionally, will set a given smart pointer to point to the leaf node.  If
      the specified point is not contained within the domain, then this method
      will throw an exception.*/
  typename PointListType::iterator AddPoint(const PointType &,
                                            unsigned int, NodePointerType &);
  typename PointListType::iterator AddPoint(const PointType &p,
                                            unsigned int i)
  {
    NodePointerType node = NodeType::New();
    return this->AddPoint(p, i, node);
  }

  /** Returns true if the specified node region overlaps the given region and
      false otherwise. */
  bool Overlap(const NodePointerType &, const PointType &,  const PointType &) const;

  /** */
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
  PSMPointTree()
  {
    m_Depth = 0;
  }
  virtual ~PSMPointTree() {}

  /** Add the appropriate number of empty child nodes to a given node.  The
      second parameter is the level in the tree.*/
  void BranchNode(NodePointerType &, unsigned int);

/** Find one of the nodes that overlaps the specified region and appends all of
    its points to the specified list.  The method is used by FindPointsInRegion
    and is called recursively. */
  void FindOneNodeInRegion(const NodePointerType&, const PointType &, const PointType &,
                           PointIteratorListType &) const;
  
private:
  PSMPointTree(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  NodePointerType m_Root;
  
  unsigned int m_Depth;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMPointTree.hxx"
#endif

#endif
