---
title: shapeworks::SphereConstraint

---

# shapeworks::SphereConstraint





Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1SphereConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override |
| virtual void | **[print](../Classes/classshapeworks_1_1SphereConstraint.md#function-print)**() const override |
| Eigen::Vector3d | **[getCenter](../Classes/classshapeworks_1_1SphereConstraint.md#function-getcenter)**() |
| void | **[setCenter](../Classes/classshapeworks_1_1SphereConstraint.md#function-setcenter)**(Eigen::Vector3d inCenter) |
| double | **[getRadius](../Classes/classshapeworks_1_1SphereConstraint.md#function-getradius)**() |
| void | **[setRadius](../Classes/classshapeworks_1_1SphereConstraint.md#function-setradius)**(double inRadius) |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1SphereConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override |
| Eigen::Vector3d | **[constraintGradientSphere](../Classes/classshapeworks_1_1SphereConstraint.md#function-constraintgradientsphere)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt) const |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1SphereConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override |
| double | **[constraintEvalSphere](../Classes/classshapeworks_1_1SphereConstraint.md#function-constraintevalsphere)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt) const |
| Eigen::Vector3d | **[lagragianGradient](../Classes/classshapeworks_1_1SphereConstraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, const Eigen::Vector3d & updpt, double C) const |

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
inline virtual void print() const override
```


**Reimplements**: [shapeworks::Constraint::print](../Classes/classshapeworks_1_1Constraint.md#function-print)


### function getCenter

```cpp
inline Eigen::Vector3d getCenter()
```


### function setCenter

```cpp
inline void setCenter(
    Eigen::Vector3d inCenter
)
```


### function getRadius

```cpp
inline double getRadius()
```


### function setRadius

```cpp
inline void setRadius(
    double inRadius
)
```


### function constraintGradient

```cpp
inline virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)


### function constraintGradientSphere

```cpp
Eigen::Vector3d constraintGradientSphere(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt
) const
```


### function constraintEval

```cpp
inline virtual double constraintEval(
    const Eigen::Vector3d & pt
) const override
```


**Reimplements**: [shapeworks::Constraint::constraintEval](../Classes/classshapeworks_1_1Constraint.md#function-constrainteval)


### function constraintEvalSphere

```cpp
double constraintEvalSphere(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt
) const
```


### function lagragianGradient

```cpp
Eigen::Vector3d lagragianGradient(
    const Eigen::Vector3d & pt,
    const Eigen::Vector3d & updpt,
    double C
) const
```


-------------------------------

Updated on 2022-03-07 at 00:21:28 +0000