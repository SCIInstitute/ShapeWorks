---
title: shapeworks::PlaneConstraint

---

# shapeworks::PlaneConstraint



 [More...](#detailed-description)


`#include <PlaneConstraint.h>`

Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1PlaneConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override<br>Returns if pt in Eigen format is violated by the constraint.  |
| virtual void | **[print](../Classes/classshapeworks_1_1PlaneConstraint.md#function-print)**() const override<br>Prints the constraint neatly.  |
| Eigen::Vector3d | **[getPlaneNormal](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getplanenormal)**()<br>Get plane normal.  |
| void | **[setPlaneNormal](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanenormal)**(const Eigen::Vector3d & inPlane)<br>Set plane normal.  |
| Eigen::Vector3d | **[getPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getplanepoint)**()<br>Get plane center point.  |
| void | **[setPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanepoint)**(const vnl_vector< double > & point)<br>Set plane center point.  |
| void | **[setPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanepoint)**(const Eigen::Vector3d & p)<br>Set plane center point.  |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override<br>Returns the gradient of the constraint.  |
| std::vector< Eigen::Vector3d > & | **[points](../Classes/classshapeworks_1_1PlaneConstraint.md#function-points)**() |
| double | **[getOffset](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getoffset)**() |
| void | **[setOffset](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setoffset)**(double offset) |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override<br>Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary.  |
| void | **[updatePlaneFromPoints](../Classes/classshapeworks_1_1PlaneConstraint.md#function-updateplanefrompoints)**() |
| vtkSmartPointer< vtkPlane > | **[getVTKPlane](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getvtkplane)**()<br>Return this plane as a vtkPlane.  |

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
class shapeworks::PlaneConstraint;
```


Encapsulate functionality related to cutting-plane constraints. Inherits from [Constraint](../Classes/classshapeworks_1_1Constraint.md). See [Constraint.h] for more information 

## Public Functions Documentation

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
virtual void print() const override
```

Prints the constraint neatly. 

**Reimplements**: [shapeworks::Constraint::print](../Classes/classshapeworks_1_1Constraint.md#function-print)


### function getPlaneNormal

```cpp
inline Eigen::Vector3d getPlaneNormal()
```

Get plane normal. 

### function setPlaneNormal

```cpp
inline void setPlaneNormal(
    const Eigen::Vector3d & inPlane
)
```

Set plane normal. 

### function getPlanePoint

```cpp
inline Eigen::Vector3d getPlanePoint()
```

Get plane center point. 

### function setPlanePoint

```cpp
inline void setPlanePoint(
    const vnl_vector< double > & point
)
```

Set plane center point. 

### function setPlanePoint

```cpp
inline void setPlanePoint(
    const Eigen::Vector3d & p
)
```

Set plane center point. 

### function constraintGradient

```cpp
inline virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const override
```

Returns the gradient of the constraint. 

**Reimplements**: [shapeworks::Constraint::constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)


### function points

```cpp
inline std::vector< Eigen::Vector3d > & points()
```


### function getOffset

```cpp
double getOffset()
```


### function setOffset

```cpp
void setOffset(
    double offset
)
```


### function constraintEval

```cpp
virtual double constraintEval(
    const Eigen::Vector3d & pt
) const override
```

Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary. 

**Reimplements**: [shapeworks::Constraint::constraintEval](../Classes/classshapeworks_1_1Constraint.md#function-constrainteval)


### function updatePlaneFromPoints

```cpp
void updatePlaneFromPoints()
```


### function getVTKPlane

```cpp
vtkSmartPointer< vtkPlane > getVTKPlane()
```

Return this plane as a vtkPlane. 

-------------------------------

Updated on 2024-07-14 at 16:33:03 +0000