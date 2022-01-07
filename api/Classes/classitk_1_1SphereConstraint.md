---
title: itk::SphereConstraint

---

# itk::SphereConstraint





Inherits from [itk::Constraint](../Classes/classitk_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classitk_1_1SphereConstraint.md#function-isviolated)**(const vnl_vector< double > & pt) const |
| virtual bool | **[isViolated](../Classes/classitk_1_1SphereConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const |
| virtual void | **[printC](../Classes/classitk_1_1SphereConstraint.md#function-printc)**() const |
| Eigen::Vector3d | **[GetCenter](../Classes/classitk_1_1SphereConstraint.md#function-getcenter)**() |
| void | **[SetCenter](../Classes/classitk_1_1SphereConstraint.md#function-setcenter)**(Eigen::Vector3d inCenter) |
| double | **[GetRadius](../Classes/classitk_1_1SphereConstraint.md#function-getradius)**() |
| void | **[SetRadius](../Classes/classitk_1_1SphereConstraint.md#function-setradius)**(double inRadius) |
| virtual Eigen::Vector3d | **[ConstraintGradient](../Classes/classitk_1_1SphereConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const |
| Eigen::Vector3d | **[ConstraintGradientSphere](../Classes/classitk_1_1SphereConstraint.md#function-constraintgradientsphere)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt) const |
| virtual double | **[ConstraintEval](../Classes/classitk_1_1SphereConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const |
| double | **[ConstraintEvalSphere](../Classes/classitk_1_1SphereConstraint.md#function-constraintevalsphere)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt) const |
| Eigen::Vector3d | **[LagragianGradient](../Classes/classitk_1_1SphereConstraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt, double C) |

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


### function GetCenter

```cpp
inline Eigen::Vector3d GetCenter()
```


### function SetCenter

```cpp
inline void SetCenter(
    Eigen::Vector3d inCenter
)
```


### function GetRadius

```cpp
inline double GetRadius()
```


### function SetRadius

```cpp
inline void SetRadius(
    double inRadius
)
```


### function ConstraintGradient

```cpp
inline virtual Eigen::Vector3d ConstraintGradient(
    const Eigen::Vector3d & pt
) const
```


**Reimplements**: [itk::Constraint::ConstraintGradient](../Classes/classitk_1_1Constraint.md#function-constraintgradient)


### function ConstraintGradientSphere

```cpp
inline Eigen::Vector3d ConstraintGradientSphere(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt
) const
```


### function ConstraintEval

```cpp
inline virtual double ConstraintEval(
    const Eigen::Vector3d & pt
) const
```


**Reimplements**: [itk::Constraint::ConstraintEval](../Classes/classitk_1_1Constraint.md#function-constrainteval)


### function ConstraintEvalSphere

```cpp
inline double ConstraintEvalSphere(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt
) const
```


### function LagragianGradient

```cpp
inline Eigen::Vector3d LagragianGradient(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt,
    double C
)
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000