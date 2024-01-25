---
title: shapeworks::PowerOfTwoPointTree

---

# shapeworks::PowerOfTwoPointTree



 [More...](#detailed-description)


`#include <PowerOfTwoPointTree.h>`

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md) | **[Self](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-constweakpointer)**  |
| typedef [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) | **[NodeType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodetype)**  |
| typedef NodeType::Pointer | **[NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype)**  |
| typedef [PowerOfTwoPointTreeNode::PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype)**  |
| typedef [NodeType::PointListType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype) | **[PointListType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointlisttype)**  |
| typedef std::vector< typename PointListType::const_iterator > | **[PointIteratorListType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointiteratorlisttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itktypemacro)**([PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md) , DataObject ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itkstaticconstmacro)**(BranchesPerNode , int , ([powstruct](../Classes/structshapeworks_1_1powstruct.md)< 2, VDimension >::c) ) |
| | **[itkGetMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itkgetmacro)**(Depth , unsigned int ) |
| void | **[ConstructTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-constructtree)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , unsigned int ) |
| PointIteratorListType | **[FindPointsInRegion](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-findpointsinregion)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & ) const |
| unsigned int | **[FindPointsInRegion](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-findpointsinregion)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , PointIteratorListType & ) const |
| [NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) | **[GetNode](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-getnode)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & ) |
| const [NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) | **[GetNode](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-getnode)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & ) const |
| | **[itkGetObjectMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itkgetobjectmacro)**(Root , [NodeType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodetype) ) |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-itksetobjectmacro)**(Root , [NodeType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodetype) ) |
| PointListType::iterator | **[AddPoint](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-addpoint)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , unsigned int , [NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) & ) |
| PointListType::iterator | **[AddPoint](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-addpoint)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & p, unsigned int i) |
| bool | **[Overlap](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-overlap)**(const [NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & ) const |
| bool | **[RegionContains](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-regioncontains)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & p, const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & lowerbound, const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & upperbound) const |
| void | **[PrintSelf](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-poweroftwopointtree)**() |
| virtual | **[~PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-~poweroftwopointtree)**() |
| void | **[BranchNode](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-branchnode)**([NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) & , unsigned int ) |
| void | **[FindOneNodeInRegion](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-findonenodeinregion)**(const [NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-pointtype) & , PointIteratorListType & ) const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#variable-vdimension)**  |

## Detailed Description

```cpp
class shapeworks::PowerOfTwoPointTree;
```


A tree data container, templated over node type, whose nodes are associated with bounding boxes in a rectangular domain, and that has 2^D branches at each node, where D is the dimensionality of the domain. In 2D this is a quad-tree, and in 3D this is an octree, etc. The tree is constructed by specifying a region and a tree depth, then calling [ConstructTree()](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#function-constructtree). This class was designed for use as a quad/octree binning structure for ParticleNeighborhoodr.h classes. 

## Public Types Documentation

### typedef Self

```cpp
typedef PowerOfTwoPointTree shapeworks::PowerOfTwoPointTree::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::PowerOfTwoPointTree::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::PowerOfTwoPointTree::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::PowerOfTwoPointTree::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::PowerOfTwoPointTree::ConstWeakPointer;
```


### typedef NodeType

```cpp
typedef PowerOfTwoPointTreeNode shapeworks::PowerOfTwoPointTree::NodeType;
```


Shorthand for the object pointed to by each node. 


### typedef NodePointerType

```cpp
typedef NodeType::Pointer shapeworks::PowerOfTwoPointTree::NodePointerType;
```


The real node type, which is a actually pointer to what we are calling NodeTypes. 


### typedef PointType

```cpp
typedef PowerOfTwoPointTreeNode::PointType shapeworks::PowerOfTwoPointTree::PointType;
```


Point type used by nodes for upper and lower bounds. 


### typedef PointListType

```cpp
typedef NodeType::PointListType shapeworks::PowerOfTwoPointTree::PointListType;
```


Types defined by the NodeType. 


### typedef PointIteratorListType

```cpp
typedef std::vector<typename PointListType::const_iterator> shapeworks::PowerOfTwoPointTree::PointIteratorListType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkTypeMacro

```cpp
itkTypeMacro(
    PowerOfTwoPointTree ,
    DataObject 
)
```


Run-time type information (and related methods). 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Dimensionality of the domain. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    BranchesPerNode ,
    int ,
    (powstruct< 2, VDimension >::c) 
)
```


Number of children per node. 


### function itkGetMacro

```cpp
itkGetMacro(
    Depth ,
    unsigned int 
)
```


Set/Get the depth of the tree. This is the number of levels in the tree. 


### function ConstructTree

```cpp
void ConstructTree(
    const PointType & ,
    const PointType & ,
    unsigned int 
)
```


Construct the tree to the specified depth. The bounding box of the root node is specified with the lower bound and upper bound points respectively. 


### function FindPointsInRegion

```cpp
PointIteratorListType FindPointsInRegion(
    const PointType & ,
    const PointType & 
) const
```


Return a list of PointListType iterators (effectively pointers to points, see [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md)) to points and their associated indicies that are stored in this tree and are contained within the specified bounding box region. The bounding box is specified with two points, in this order: a lower bound followed by an upper bound. 


### function FindPointsInRegion

```cpp
unsigned int FindPointsInRegion(
    const PointType & ,
    const PointType & ,
    PointIteratorListType & 
) const
```


### function GetNode

```cpp
NodePointerType GetNode(
    const PointType & 
)
```


Return the node associated with the domain region that contains the given point. 


### function GetNode

```cpp
const NodePointerType GetNode(
    const PointType & 
) const
```


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    Root ,
    NodeType 
)
```


Set/Get the root node of the tree. 


### function itkSetObjectMacro

```cpp
itkSetObjectMacro(
    Root ,
    NodeType 
)
```


### function AddPoint

```cpp
PointListType::iterator AddPoint(
    const PointType & ,
    unsigned int ,
    NodePointerType & 
)
```


Associates a point and, optionally, an index with the appropriate leaf node. This method starts at the root of the tree and uses the Contains method to query branches at each node, following the first branch it finds whose bounding box contains this point. When it reaches a leaf node, the point is added to that node's list, along with the specified index (if any). This method returns an iterator pointing to the new list element (e.g., for quick deletion or reference of the point) and, optionally, will set a given smart pointer to point to the leaf node. If the specified point is not contained within the domain, then this method will throw an exception. 


### function AddPoint

```cpp
inline PointListType::iterator AddPoint(
    const PointType & p,
    unsigned int i
)
```


### function Overlap

```cpp
bool Overlap(
    const NodePointerType & ,
    const PointType & ,
    const PointType & 
) const
```


Returns true if the specified node region overlaps the given region and false otherwise. 


### function RegionContains

```cpp
inline bool RegionContains(
    const PointType & p,
    const PointType & lowerbound,
    const PointType & upperbound
) const
```


### function PrintSelf

```cpp
void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


## Protected Functions Documentation

### function PowerOfTwoPointTree

```cpp
inline PowerOfTwoPointTree()
```


### function ~PowerOfTwoPointTree

```cpp
inline virtual ~PowerOfTwoPointTree()
```


### function BranchNode

```cpp
void BranchNode(
    NodePointerType & ,
    unsigned int 
)
```


Add the appropriate number of empty child nodes to a given node. The second parameter is the level in the tree. 


### function FindOneNodeInRegion

```cpp
void FindOneNodeInRegion(
    const NodePointerType & ,
    const PointType & ,
    const PointType & ,
    PointIteratorListType & 
) const
```


Find one of the nodes that overlaps the specified region and appends all of its points to the specified list. The method is used by FindPointsInRegion and is called recursively. 


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


-------------------------------

Updated on 2024-01-25 at 03:19:25 +0000