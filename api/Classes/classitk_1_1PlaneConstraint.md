---
title: itk::PlaneConstraint

---

# itk::PlaneConstraint





Inherits from [itk::Constraint](../Classes/classitk_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classitk_1_1PlaneConstraint.md#function-isviolated)**(const vnl_vector< double > & pt) const |
| virtual bool | **[isViolated](../Classes/classitk_1_1PlaneConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const |
| virtual void | **[printC](../Classes/classitk_1_1PlaneConstraint.md#function-printc)**() const |
| Eigen::Vector3d | **[GetPlaneNormal](../Classes/classitk_1_1PlaneConstraint.md#function-getplanenormal)**() |
| void | **[SetPlaneNormal](../Classes/classitk_1_1PlaneConstraint.md#function-setplanenormal)**(const Eigen::Vector3d & inPlane) |
| Eigen::Vector3d | **[GetPlanePoint](../Classes/classitk_1_1PlaneConstraint.md#function-getplanepoint)**() |
| void | **[SetPlanePoint](../Classes/classitk_1_1PlaneConstraint.md#function-setplanepoint)**(const vnl_vector< double > & ina) |
| void | **[SetPlanePoint](../Classes/classitk_1_1PlaneConstraint.md#function-setplanepoint)**(const Eigen::Vector3d & p) |
| virtual Eigen::Vector3d | **[ConstraintGradient](../Classes/classitk_1_1PlaneConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const |
| virtual double | **[ConstraintEval](../Classes/classitk_1_1PlaneConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const |

## Additional inherited members

**Public Functions inherited from [itk::Constraint](../Classes/classitk_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| shapeworks::ConstraintType | **[GetConstraintType](../Classes/classitk_1_1Constraint.md#function-getconstrainttype)**() |
| void | **[setConstraintType](../Classes/classitk_1_1Constraint.md#function-setconstrainttype)**(shapeworks::ConstraintType inType) |
| void | **[SetZ](../Classes/classitk_1_1Constraint.md#function-setz)**(double inz) |
| double | **[GetZ](../Classes/classitk_1_1Constraint.md#function-getz)**() |
| void | **[SetMu](../Classes/classitk_1_1Constraint.md#function-setmu)**(double inmu) |
| double | **[GetMu](../Classes/classitk_1_1Constraint.md#function-getmu)**() |
| void | **[SetLambda](../Classes/classitk_1_1Constraint.md#function-setlambda)**(double inLambda) |
| double | **[GetLambda](../Classes/classitk_1_1Constraint.md#function-getlambda)**() |
| void | **[UpdateZ](../Classes/classitk_1_1Constraint.md#function-updatez)**(const Eigen::Vector3d & pt, double C) |
| void | **[UpdateMu](../Classes/classitk_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C) |
| Eigen::Vector3d | **[LagragianGradient](../Classes/classitk_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C) |

**Protected Functions inherited from [itk::Constraint](../Classes/classitk_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classitk_1_1Constraint.md#function-sgn)**(double val) |

**Protected Attributes inherited from [itk::Constraint](../Classes/classitk_1_1Constraint.md)**

|                | Name           |
| -------------- | -------------- |
| shapeworks::ConstraintType | **[type](../Classes/classitk_1_1Constraint.md#variable-type)**  |
| double | **[mu](../Classes/classitk_1_1Constraint.md#variable-mu)**  |
| double | **[z](../Classes/classitk_1_1Constraint.md#variable-z)**  |
| double | **[lambda](../Classes/classitk_1_1Constraint.md#variable-lambda)**  |


## Public Functions Documentation

### function isViolated

```cpp
inline virtual bool isViolated(
    const vnl_vector< double > & pt
) const
```


**Reimplements**: [itk::Constraint::isViolated](../Classes/classitk_1_1Constraint.md#function-isviolated)


### function isViolated

```cpp
inline virtual bool isViolated(
    const Eigen::Vector3d & pt
) const
```


**Reimplements**: [itk::Constraint::isViolated](../Classes/classitk_1_1Constraint.md#function-isviolated)


### function printC

```cpp
inline virtual void printC() const
```


**Reimplements**: [itk::Constraint::printC](../Classes/classitk_1_1Constraint.md#function-printc)


### function GetPlaneNormal

```cpp
inline Eigen::Vector3d GetPlaneNormal()
```


### function SetPlaneNormal

```cpp
inline void SetPlaneNormal(
    const Eigen::Vector3d & inPlane
)
```


### function GetPlanePoint

```cpp
inline Eigen::Vector3d GetPlanePoint()
```


### function SetPlanePoint

```cpp
inline void SetPlanePoint(
    const vnl_vector< double > & ina
)
```


### function SetPlanePoint

```cpp
inline void SetPlanePoint(
    const Eigen::Vector3d & p
)
```


### function ConstraintGradient

```cpp
inline virtual Eigen::Vector3d ConstraintGradient(
    const Eigen::Vector3d & pt
) const
```


**Reimplements**: [itk::Constraint::ConstraintGradient](../Classes/classitk_1_1Constraint.md#function-constraintgradient)


### function ConstraintEval

```cpp
inline virtual double ConstraintEval(
    const Eigen::Vector3d & pt
) const
```


**Reimplements**: [itk::Constraint::ConstraintEval](../Classes/classitk_1_1Constraint.md#function-constrainteval)


-------------------------------

Updated on 2022-01-28 at 21:13:52 +0000