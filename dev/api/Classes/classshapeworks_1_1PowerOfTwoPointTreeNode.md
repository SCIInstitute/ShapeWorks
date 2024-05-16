---
title: shapeworks::PowerOfTwoPointTreeNode

---

# shapeworks::PowerOfTwoPointTreeNode



 [More...](#detailed-description)


`#include <PowerOfTwoPointTree.h>`

Inherits from itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) | **[Self](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-constpointer)**  |
| typedef itk::LightObject | **[Superclass](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-superclass)**  |
| typedef itk::Point< double, VDimension > | **[PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype)**  |
| typedef std::list< [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) > | **[PointListType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-itktypemacro)**([PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) , LightObject ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-itkstaticconstmacro)**(BranchesPerNode , int , ([powstruct](../Classes/structshapeworks_1_1powstruct.md)< 2, VDimension >::c) ) |
| bool | **[operator==](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-operator==)**(const [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) & o) const |
| bool | **[IsLeaf](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-isleaf)**() const |
| PointListType::iterator | **[InsertElement](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-insertelement)**(const [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) & elem) |
| bool | **[Contains](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-contains)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) const |
| void | **[SetLowerBound](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-setlowerbound)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) |
| const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getlowerbound)**() const |
| void | **[SetUpperBound](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-setupperbound)**(const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) |
| const [PointType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getupperbound)**() const |
| Self::Pointer * | **[GetBranches](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getbranches)**() |
| const Self::Pointer * | **[GetBranches](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getbranches)**() const |
| Self::Pointer & | **[GetBranch](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getbranch)**(unsigned int i) |
| const Self::Pointer & | **[GetBranch](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getbranch)**(unsigned int i) const |
| void | **[SetBranch](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-setbranch)**(unsigned int b, typename Self::Pointer n) |
| void | **[PrintSelf](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| const [PointListType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype) & | **[GetList](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getlist)**() const |
| [PointListType](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype) & | **[GetList](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-getlist)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-poweroftwopointtreenode)**() |
| | **[~PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-~poweroftwopointtreenode)**() |
| | **[PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-poweroftwopointtreenode)**(const [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) & o) |
| const [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) & | **[operator=](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#function-operator=)**(const [PowerOfTwoPointTreeNode](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md) & o) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1PowerOfTwoPointTreeNode.md#variable-vdimension)**  |

## Detailed Description

```cpp
class shapeworks::PowerOfTwoPointTreeNode;
```


A node object for use in the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). This class contains a bounding box, a set of child (branch) node pointers, and a linked list that may be filled with Points and Indices (unsigned ints). The single template parameter is the dimensionality of the Points. This class was designed for use as a binning structure for ParticleNeighborhoodr.h classes. 

## Public Types Documentation

### typedef Self

```cpp
typedef PowerOfTwoPointTreeNode shapeworks::PowerOfTwoPointTreeNode::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::PowerOfTwoPointTreeNode::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::PowerOfTwoPointTreeNode::ConstPointer;
```


### typedef Superclass

```cpp
typedef itk::LightObject shapeworks::PowerOfTwoPointTreeNode::Superclass;
```


### typedef PointType

```cpp
typedef itk::Point<double, VDimension> shapeworks::PowerOfTwoPointTreeNode::PointType;
```


Point type stored in the leaf nodes. 


### typedef PointListType

```cpp
typedef std::list<ParticlePointIndexPair> shapeworks::PowerOfTwoPointTreeNode::PointListType;
```


List type for storing lists of points+indices. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    PowerOfTwoPointTreeNode ,
    LightObject 
)
```


### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    BranchesPerNode ,
    int ,
    (powstruct< 2, VDimension >::c) 
)
```


The number of branches at each node of the tree, which is 2^VDimension, e.g. 4 in 2D, 8 in 3D, etc. 


### function operator==

```cpp
inline bool operator==(
    const PowerOfTwoPointTreeNode & o
) const
```


Equivalence operator, ignores value type. 


### function IsLeaf

```cpp
inline bool IsLeaf() const
```


Returns true if this is a leaf node (has no branches) and false otherwise. The assumption is that a non-leaf node cannot have any empty branches. Each level of the tree must contain BranchesPerNode divisions. This assumption speeds up certain types of processing. 


### function InsertElement

```cpp
inline PointListType::iterator InsertElement(
    const ParticlePointIndexPair & elem
)
```


Insert a PointIndexPair into the list associated with this node. 


### function Contains

```cpp
inline bool Contains(
    const PointType & p
) const
```


Returns true if the bounding box in this node contains the given point and false otherwise. 


### function SetLowerBound

```cpp
inline void SetLowerBound(
    const PointType & p
)
```


Set/Get the lower and upper bounds of the region described by this node. 


### function GetLowerBound

```cpp
inline const PointType & GetLowerBound() const
```


### function SetUpperBound

```cpp
inline void SetUpperBound(
    const PointType & p
)
```


### function GetUpperBound

```cpp
inline const PointType & GetUpperBound() const
```


### function GetBranches

```cpp
inline Self::Pointer * GetBranches()
```


Return the data structure holding branch (child) nodes. 


### function GetBranches

```cpp
inline const Self::Pointer * GetBranches() const
```


### function GetBranch

```cpp
inline Self::Pointer & GetBranch(
    unsigned int i
)
```


Return a specific branch 


### function GetBranch

```cpp
inline const Self::Pointer & GetBranch(
    unsigned int i
) const
```


### function SetBranch

```cpp
inline void SetBranch(
    unsigned int b,
    typename Self::Pointer n
)
```


Set the node for a specific branch of this node. 


### function PrintSelf

```cpp
void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


Standard ITK PrintSelf method. 


### function GetList

```cpp
inline const PointListType & GetList() const
```


Get the list of elements which contain points and associated indices. 


### function GetList

```cpp
inline PointListType & GetList()
```


## Protected Functions Documentation

### function PowerOfTwoPointTreeNode

```cpp
inline PowerOfTwoPointTreeNode()
```


### function ~PowerOfTwoPointTreeNode

```cpp
inline ~PowerOfTwoPointTreeNode()
```


### function PowerOfTwoPointTreeNode

```cpp
PowerOfTwoPointTreeNode(
    const PowerOfTwoPointTreeNode & o
)
```


### function operator=

```cpp
const PowerOfTwoPointTreeNode & operator=(
    const PowerOfTwoPointTreeNode & o
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


-------------------------------

Updated on 2024-05-16 at 20:36:01 +0000