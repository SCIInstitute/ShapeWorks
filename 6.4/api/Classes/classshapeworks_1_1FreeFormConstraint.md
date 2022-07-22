---
title: shapeworks::FreeFormConstraint
summary: FreeFormConstraint. 

---

# shapeworks::FreeFormConstraint



[FreeFormConstraint]().  [More...](#detailed-description)


`#include <FreeFormConstraint.h>`

Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-freeformconstraint)**() |
| void | **[setMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setmesh)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh) |
| std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > | **[getMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getmesh)**() |
| bool | **[readyForOptimize](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-readyforoptimize)**() const |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override |
| virtual void | **[print](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-print)**() const override |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override |
| void | **[setDefinition](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setdefinition)**(vtkSmartPointer< vtkPolyData > polyData)<br>Set polydata where per-vertex free form constraint definition exists.  |
| vtkSmartPointer< vtkPolyData > | **[getDefinition](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getdefinition)**()<br>Get polydata where per-vertex free form constraint definition exists.  |
| void | **[applyToPolyData](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-applytopolydata)**(vtkSmartPointer< vtkPolyData > polyData)<br>Apply the free form constraint to a polydata.  |
| std::vector< std::vector< Eigen::Vector3d > > & | **[boundaries](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-boundaries)**()<br>Access the set of boundaries.  |
| Eigen::Vector3d | **[getQueryPoint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getquerypoint)**()<br>Get query (inside) point.  |
| void | **[setQueryPoint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setquerypoint)**(Eigen::Vector3d queryPoint)<br>Set query (inside) point.  |
| void | **[computeBoundaries](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-computeboundaries)**()<br>Compute boundaries from definition polydata with ffc_paint scalars.  |
| bool | **[isSet](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isset)**()<br>Return if this FFC is set or not.  |
| void | **[setPainted](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setpainted)**(bool painted)<br>Set if this FFC has paint on it or not.  |
| void | **[reset](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-reset)**()<br>Reset to initial state.  |

## Additional inherited members

**Public Functions inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[setZ](../Classes/classshapeworks_1_1Constraint.md#function-setz)**(double inz) |
| double | **[getZ](../Classes/classshapeworks_1_1Constraint.md#function-getz)**() |
| void | **[setMu](../Classes/classshapeworks_1_1Constraint.md#function-setmu)**(double inmu) |
| double | **[getMu](../Classes/classshapeworks_1_1Constraint.md#function-getmu)**() |
| void | **[setLambda](../Classes/classshapeworks_1_1Constraint.md#function-setlambda)**(double inLambda) |
| double | **[getLambda](../Classes/classshapeworks_1_1Constraint.md#function-getlambda)**() |
| void | **[updateZ](../Classes/classshapeworks_1_1Constraint.md#function-updatez)**(const Eigen::Vector3d & pt, double C) |
| void | **[updateMu](../Classes/classshapeworks_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C) |
| Eigen::Vector3d | **[lagragianGradient](../Classes/classshapeworks_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C) const |

**Protected Functions inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classshapeworks_1_1Constraint.md#function-sgn)**(double val) |

**Protected Attributes inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| double | **[mu_](../Classes/classshapeworks_1_1Constraint.md#variable-mu-)**  |
| double | **[z_](../Classes/classshapeworks_1_1Constraint.md#variable-z-)**  |
| double | **[lambda_](../Classes/classshapeworks_1_1Constraint.md#variable-lambda-)**  |


## Detailed Description

```cpp
class shapeworks::FreeFormConstraint;
```

[FreeFormConstraint](). 

Encapsulate functionality related to free form constraints 

## Public Functions Documentation

### function FreeFormConstraint

```cpp
inline FreeFormConstraint()
```


### function setMesh

```cpp
inline void setMesh(
    std::shared_ptr< shapeworks::Mesh > mesh
)
```


### function getMesh

```cpp
inline std::shared_ptr< shapeworks::Mesh > getMesh()
```


### function readyForOptimize

```cpp
bool readyForOptimize() const
```


### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)


### function print

```cpp
inline virtual void print() const override
```


**Reimplements**: [shapeworks::Constraint::print](../Classes/classshapeworks_1_1Constraint.md#function-print)


### function constraintGradient

```cpp
inline virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)


### function constraintEval

```cpp
inline virtual double constraintEval(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::constraintEval](../Classes/classshapeworks_1_1Constraint.md#function-constrainteval)


### function setDefinition

```cpp
void setDefinition(
    vtkSmartPointer< vtkPolyData > polyData
)
```

Set polydata where per-vertex free form constraint definition exists. 

### function getDefinition

```cpp
inline vtkSmartPointer< vtkPolyData > getDefinition()
```

Get polydata where per-vertex free form constraint definition exists. 

### function applyToPolyData

```cpp
void applyToPolyData(
    vtkSmartPointer< vtkPolyData > polyData
)
```

Apply the free form constraint to a polydata. 

### function boundaries

```cpp
std::vector< std::vector< Eigen::Vector3d > > & boundaries()
```

Access the set of boundaries. 

### function getQueryPoint

```cpp
inline Eigen::Vector3d getQueryPoint()
```

Get query (inside) point. 

### function setQueryPoint

```cpp
inline void setQueryPoint(
    Eigen::Vector3d queryPoint
)
```

Set query (inside) point. 

### function computeBoundaries

```cpp
void computeBoundaries()
```

Compute boundaries from definition polydata with ffc_paint scalars. 

### function isSet

```cpp
bool isSet()
```

Return if this FFC is set or not. 

### function setPainted

```cpp
void setPainted(
    bool painted
)
```

Set if this FFC has paint on it or not. 

### function reset

```cpp
void reset()
```

Reset to initial state. 

-------------------------------

Updated on 2022-07-22 at 16:39:12 +0000