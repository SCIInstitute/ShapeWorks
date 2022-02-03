---
title: itk::Constraint

---

# itk::Constraint





Inherited by [itk::FreeFormConstraint](../Classes/classitk_1_1FreeFormConstraint.md), [itk::PlaneConstraint](../Classes/classitk_1_1PlaneConstraint.md), [itk::SphereConstraint](../Classes/classitk_1_1SphereConstraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classitk_1_1Constraint.md#function-isviolated)**(const vnl_vector< double > & pt) const =0 |
| virtual bool | **[isViolated](../Classes/classitk_1_1Constraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const =0 |
| virtual void | **[printC](../Classes/classitk_1_1Constraint.md#function-printc)**() const =0 |
| shapeworks::ConstraintType | **[GetConstraintType](../Classes/classitk_1_1Constraint.md#function-getconstrainttype)**() |
| void | **[setConstraintType](../Classes/classitk_1_1Constraint.md#function-setconstrainttype)**(shapeworks::ConstraintType inType) |
| void | **[SetZ](../Classes/classitk_1_1Constraint.md#function-setz)**(double inz) |
| double | **[GetZ](../Classes/classitk_1_1Constraint.md#function-getz)**() |
| void | **[SetMu](../Classes/classitk_1_1Constraint.md#function-setmu)**(double inmu) |
| double | **[GetMu](../Classes/classitk_1_1Constraint.md#function-getmu)**() |
| void | **[SetLambda](../Classes/classitk_1_1Constraint.md#function-setlambda)**(double inLambda) |
| double | **[GetLambda](../Classes/classitk_1_1Constraint.md#function-getlambda)**() |
| virtual Eigen::Vector3d | **[ConstraintGradient](../Classes/classitk_1_1Constraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const =0 |
| virtual double | **[ConstraintEval](../Classes/classitk_1_1Constraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const =0 |
| void | **[UpdateZ](../Classes/classitk_1_1Constraint.md#function-updatez)**(const Eigen::Vector3d & pt, double C) |
| void | **[UpdateMu](../Classes/classitk_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C) |
| Eigen::Vector3d | **[LagragianGradient](../Classes/classitk_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classitk_1_1Constraint.md#function-sgn)**(double val) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| shapeworks::ConstraintType | **[type](../Classes/classitk_1_1Constraint.md#variable-type)**  |
| double | **[mu](../Classes/classitk_1_1Constraint.md#variable-mu)**  |
| double | **[z](../Classes/classitk_1_1Constraint.md#variable-z)**  |
| double | **[lambda](../Classes/classitk_1_1Constraint.md#variable-lambda)**  |

## Public Functions Documentation

### function isViolated

```cpp
virtual bool isViolated(
    const vnl_vector< double > & pt
) const =0
```


**Reimplemented by**: [itk::FreeFormConstraint::isViolated](../Classes/classitk_1_1FreeFormConstraint.md#function-isviolated), [itk::PlaneConstraint::isViolated](../Classes/classitk_1_1PlaneConstraint.md#function-isviolated), [itk::SphereConstraint::isViolated](../Classes/classitk_1_1SphereConstraint.md#function-isviolated)


### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [itk::FreeFormConstraint::isViolated](../Classes/classitk_1_1FreeFormConstraint.md#function-isviolated), [itk::PlaneConstraint::isViolated](../Classes/classitk_1_1PlaneConstraint.md#function-isviolated), [itk::SphereConstraint::isViolated](../Classes/classitk_1_1SphereConstraint.md#function-isviolated)


### function printC

```cpp
virtual void printC() const =0
```


**Reimplemented by**: [itk::FreeFormConstraint::printC](../Classes/classitk_1_1FreeFormConstraint.md#function-printc), [itk::PlaneConstraint::printC](../Classes/classitk_1_1PlaneConstraint.md#function-printc), [itk::SphereConstraint::printC](../Classes/classitk_1_1SphereConstraint.md#function-printc)


### function GetConstraintType

```cpp
inline shapeworks::ConstraintType GetConstraintType()
```


### function setConstraintType

```cpp
inline void setConstraintType(
    shapeworks::ConstraintType inType
)
```


### function SetZ

```cpp
inline void SetZ(
    double inz
)
```


### function GetZ

```cpp
inline double GetZ()
```


### function SetMu

```cpp
inline void SetMu(
    double inmu
)
```


### function GetMu

```cpp
inline double GetMu()
```


### function SetLambda

```cpp
inline void SetLambda(
    double inLambda
)
```


### function GetLambda

```cpp
inline double GetLambda()
```


### function ConstraintGradient

```cpp
virtual Eigen::Vector3d ConstraintGradient(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [itk::FreeFormConstraint::ConstraintGradient](../Classes/classitk_1_1FreeFormConstraint.md#function-constraintgradient), [itk::PlaneConstraint::ConstraintGradient](../Classes/classitk_1_1PlaneConstraint.md#function-constraintgradient), [itk::SphereConstraint::ConstraintGradient](../Classes/classitk_1_1SphereConstraint.md#function-constraintgradient)


### function ConstraintEval

```cpp
virtual double ConstraintEval(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [itk::FreeFormConstraint::ConstraintEval](../Classes/classitk_1_1FreeFormConstraint.md#function-constrainteval), [itk::PlaneConstraint::ConstraintEval](../Classes/classitk_1_1PlaneConstraint.md#function-constrainteval), [itk::SphereConstraint::ConstraintEval](../Classes/classitk_1_1SphereConstraint.md#function-constrainteval)


### function UpdateZ

```cpp
inline void UpdateZ(
    const Eigen::Vector3d & pt,
    double C
)
```


### function UpdateMu

```cpp
inline void UpdateMu(
    const Eigen::Vector3d & pt,
    double C
)
```


### function LagragianGradient

```cpp
inline Eigen::Vector3d LagragianGradient(
    const Eigen::Vector3d & pt,
    double C
)
```


## Protected Functions Documentation

### function sgn

```cpp
inline int sgn(
    double val
)
```


## Protected Attributes Documentation

### variable type

```cpp
shapeworks::ConstraintType type;
```


### variable mu

```cpp
double mu;
```


### variable z

```cpp
double z;
```


### variable lambda

```cpp
double lambda;
```


-------------------------------

Updated on 2022-02-03 at 23:25:51 +0000