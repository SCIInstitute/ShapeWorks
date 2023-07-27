---
title: shapeworks::FreeFormConstraint

---

# shapeworks::FreeFormConstraint



 [More...](#detailed-description)


`#include <FreeFormConstraint.h>`

Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-freeformconstraint)**() |
| void | **[setMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setmesh)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh)<br>Sets the mesh that defines the FFC.  |
| std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > | **[getMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getmesh)**()<br>Returns the mesh that defines the FFC.  |
| bool | **[readyForOptimize](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-readyforoptimize)**() const<br>Prepares for optimize.  |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override<br>Returns if pt in Eigen format is violated by the constraint.  |
| virtual void | **[print](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-print)**() const override<br>Prints the constraint neatly.  |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override<br>Returns the gradient of the constraint.  |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override<br>Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary.  |
| void | **[setDefinition](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setdefinition)**(vtkSmartPointer< vtkPolyData > polyData)<br>Set polydata where per-vertex free form constraint definition exists.  |
| vtkSmartPointer< vtkPolyData > | **[getDefinition](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getdefinition)**()<br>Get polydata where per-vertex free form constraint definition exists.  |
| void | **[applyToPolyData](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-applytopolydata)**(vtkSmartPointer< vtkPolyData > polyData)<br>Apply the free form constraint to a polydata.  |
| std::vector< std::vector< Eigen::Vector3d > > & | **[boundaries](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-boundaries)**() |
| Eigen::Vector3d | **[getQueryPoint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getquerypoint)**() |
| void | **[setQueryPoint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setquerypoint)**(Eigen::Vector3d queryPoint) |
| void | **[computeBoundaries](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-computeboundaries)**() |
| void | **[setInoutPolyData](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setinoutpolydata)**(vtkSmartPointer< vtkPolyData > polyData)<br>Set the in/out polydata.  |
| vtkSmartPointer< vtkPolyData > | **[getInoutPolyData](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getinoutpolydata)**()<br>Get the in/out polydata.  |
| void | **[createInoutPolyData](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-createinoutpolydata)**()<br>Create the in/out poly data from the definition poly data.  |
| bool | **[isSet](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isset)**()<br>Return if this FFC is set or not.  |
| void | **[setPainted](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setpainted)**(bool painted)<br>Set if this FFC has paint on it or not.  |
| void | **[reset](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-reset)**()<br>Reset to initial state.  |
| void | **[computeGradientFields](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-computegradientfields)**(std::shared_ptr< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh)<br>Compute gradient fields for a mesh.  |
| void | **[convertLegacyFFC](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-convertlegacyffc)**(vtkSmartPointer< vtkPolyData > polyData)<br>Convert legacy FFC to new format.  |

## Additional inherited members

**Public Functions inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[setMus](../Classes/classshapeworks_1_1Constraint.md#function-setmus)**(std::vector< double > inmu)<br>Initializes mu.  |
| std::vector< double > | **[getMus](../Classes/classshapeworks_1_1Constraint.md#function-getmus)**()<br>Gets mu.  |
| void | **[updateMu](../Classes/classshapeworks_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C, size_t index)<br>Updates the value of mu according to the augmented lagrangian update.  |
| Eigen::Vector3d | **[lagragianGradient](../Classes/classshapeworks_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C, size_t index) const<br>Computes the lagrangian gradient based on lagrangian inequality equations. NOTE: Not actually lagrangian. We are using quadratic penalty and not lagrangian because it works better.  |

**Protected Functions inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classshapeworks_1_1Constraint.md#function-sgn)**(double val)<br>Returns the sign of the double.  |

**Protected Attributes inherited from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| std::vector< double > | **[mus_](../Classes/classshapeworks_1_1Constraint.md#variable-mus-)** <br>Mu is the lagrangian momentum term.  |


## Detailed Description

```cpp
class shapeworks::FreeFormConstraint;
```


Encapsulate functionality related to free-form constraints (FFC). Inherits from [Constraint](../Classes/classshapeworks_1_1Constraint.md). See [Constraint.h] for more information 

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

Sets the mesh that defines the FFC. 

### function getMesh

```cpp
inline std::shared_ptr< shapeworks::Mesh > getMesh()
```

Returns the mesh that defines the FFC. 

### function readyForOptimize

```cpp
bool readyForOptimize() const
```

Prepares for optimize. 

### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const override
```

Returns if pt in Eigen format is violated by the constraint. 

**Reimplements**: [shapeworks::Constraint::isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)


### function print

```cpp
inline virtual void print() const override
```

Prints the constraint neatly. 

**Reimplements**: [shapeworks::Constraint::print](../Classes/classshapeworks_1_1Constraint.md#function-print)


### function constraintGradient

```cpp
inline virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const override
```

Returns the gradient of the constraint. 

**Reimplements**: [shapeworks::Constraint::constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)


### function constraintEval

```cpp
inline virtual double constraintEval(
    const Eigen::Vector3d & pt
) const override
```

Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary. 

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


**Deprecated**: 

Access the set of boundaries 

### function getQueryPoint

```cpp
inline Eigen::Vector3d getQueryPoint()
```


**Deprecated**: 

Get query (inside) point 

### function setQueryPoint

```cpp
inline void setQueryPoint(
    Eigen::Vector3d queryPoint
)
```


**Deprecated**: 

Set query (inside) point 

### function computeBoundaries

```cpp
void computeBoundaries()
```


**Deprecated**: 

Compute boundaries from definition polydata with ffc_paint scalars 

### function setInoutPolyData

```cpp
inline void setInoutPolyData(
    vtkSmartPointer< vtkPolyData > polyData
)
```

Set the in/out polydata. 

### function getInoutPolyData

```cpp
inline vtkSmartPointer< vtkPolyData > getInoutPolyData()
```

Get the in/out polydata. 

### function createInoutPolyData

```cpp
void createInoutPolyData()
```

Create the in/out poly data from the definition poly data. 

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

### function computeGradientFields

```cpp
void computeGradientFields(
    std::shared_ptr< Mesh > mesh
)
```

Compute gradient fields for a mesh. 

### function convertLegacyFFC

```cpp
void convertLegacyFFC(
    vtkSmartPointer< vtkPolyData > polyData
)
```

Convert legacy FFC to new format. 

-------------------------------

Updated on 2023-07-27 at 15:31:32 +0000