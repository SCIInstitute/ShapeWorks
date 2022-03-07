---
title: shapeworks::PlaneConstraint

---

# shapeworks::PlaneConstraint





Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1PlaneConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override |
| virtual void | **[print](../Classes/classshapeworks_1_1PlaneConstraint.md#function-print)**() const override |
| Eigen::Vector3d | **[getPlaneNormal](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getplanenormal)**()<br>Get plane normal.  |
| void | **[setPlaneNormal](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanenormal)**(const Eigen::Vector3d & inPlane)<br>Set plane normal.  |
| Eigen::Vector3d | **[getPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getplanepoint)**()<br>Get plane center point.  |
| void | **[setPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanepoint)**(const vnl_vector< double > & point)<br>Set plane center point.  |
| void | **[setPlanePoint](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setplanepoint)**(const Eigen::Vector3d & p)<br>Set plane center point.  |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override |
| std::vector< Eigen::Vector3d > & | **[points](../Classes/classshapeworks_1_1PlaneConstraint.md#function-points)**() |
| double | **[getOffset](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getoffset)**() |
| void | **[setOffset](../Classes/classshapeworks_1_1PlaneConstraint.md#function-setoffset)**(double offset) |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override |
| void | **[updatePlaneFromPoints](../Classes/classshapeworks_1_1PlaneConstraint.md#function-updateplanefrompoints)**() |
| vtkSmartPointer< vtkPlane > | **[getVTKPlane](../Classes/classshapeworks_1_1PlaneConstraint.md#function-getvtkplane)**()<br>Return this plane as a vtkPlane.  |

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


## Public Functions Documentation

### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)


### function print

```cpp
virtual void print() const override
```


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

Updated on 2022-03-07 at 00:21:27 +0000