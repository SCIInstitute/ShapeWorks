---
title: itk::PowerOfTwoPointTreeNode

---

# itk::PowerOfTwoPointTreeNode



 [More...](#detailed-description)


`#include <itkPowerOfTwoPointTree.h>`

Inherits from LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) | **[Self](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-constpointer)**  |
| typedef LightObject | **[Superclass](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-superclass)**  |
| typedef Point< double, VDimension > | **[PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype)**  |
| typedef std::list< [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md)< VDimension > > | **[PointListType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-itktypemacro)**([PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) , LightObject ) |
| | **[itkNewMacro](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-itknewmacro)**([Self](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-itkstaticconstmacro)**(BranchesPerNode , int , ([powstruct](../Classes/structitk_1_1powstruct.md)< 2, VDimension >::c) ) |
| bool | **[operator==](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-operator==)**(const [PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) & o) const |
| bool | **[IsLeaf](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-isleaf)**() const |
| PointListType::iterator | **[InsertElement](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-insertelement)**(const [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md)< VDimension > & elem) |
| bool | **[Contains](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-contains)**(const [PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) const |
| void | **[SetLowerBound](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-setlowerbound)**(const [PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) |
| const [PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getlowerbound)**() const |
| void | **[SetUpperBound](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-setupperbound)**(const [PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & p) |
| const [PointType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getupperbound)**() const |
| Self::Pointer * | **[GetBranches](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getbranches)**() |
| const Self::Pointer * | **[GetBranches](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getbranches)**() const |
| Self::Pointer & | **[GetBranch](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getbranch)**(unsigned int i) |
| const Self::Pointer & | **[GetBranch](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getbranch)**(unsigned int i) const |
| void | **[SetBranch](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-setbranch)**(unsigned int b, typename Self::Pointer n) |
| void | **[PrintSelf](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-printself)**(std::ostream & os, Indent indent) const |
| const [PointListType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype) & | **[GetList](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getlist)**() const |
| [PointListType](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#typedef-pointlisttype) & | **[GetList](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-getlist)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-poweroftwopointtreenode)**() |
| | **[~PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-~poweroftwopointtreenode)**() |
| | **[PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-poweroftwopointtreenode)**(const [PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) & o) |
| const [PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) & | **[operator=](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md#function-operator=)**(const [PowerOfTwoPointTreeNode](../Classes/classitk_1_1PowerOfTwoPointTreeNode.md) & o) |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::PowerOfTwoPointTreeNode;
```


A node object for use in the [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md). This class contains a bounding box, a set of child (branch) node pointers, and a linked list that may be filled with Points and Indices (unsigned ints). The single template parameter is the dimensionality of the Points. This class was designed for use as a binning structure for itkParticleNeighborhood classes. 

## Public Types Documentation

### typedef Self

```cpp
typedef PowerOfTwoPointTreeNode itk::PowerOfTwoPointTreeNode< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::PowerOfTwoPointTreeNode< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::PowerOfTwoPointTreeNode< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef LightObject itk::PowerOfTwoPointTreeNode< VDimension >::Superclass;
```


### typedef PointType

```cpp
typedef Point<double, VDimension> itk::PowerOfTwoPointTreeNode< VDimension >::PointType;
```


Point type stored in the leaf nodes. 


### typedef PointListType

```cpp
typedef std::list<ParticlePointIndexPair<VDimension> > itk::PowerOfTwoPointTreeNode< VDimension >::PointListType;
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
    const ParticlePointIndexPair< VDimension > & elem
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
    Indent indent
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


-------------------------------

Updated on 2022-03-03 at 07:50:35 +0000