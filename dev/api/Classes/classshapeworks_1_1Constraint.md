---
title: shapeworks::Constraint

---

# shapeworks::Constraint





Inherited by [shapeworks::FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md), [shapeworks::PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md), [shapeworks::SphereConstraint](../Classes/classshapeworks_1_1SphereConstraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)**(const vnl_vector< double > & pt) const |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const =0 |
| virtual void | **[print](../Classes/classshapeworks_1_1Constraint.md#function-print)**() const =0 |
| void | **[setZ](../Classes/classshapeworks_1_1Constraint.md#function-setz)**(double inz) |
| double | **[getZ](../Classes/classshapeworks_1_1Constraint.md#function-getz)**() |
| void | **[setMus](../Classes/classshapeworks_1_1Constraint.md#function-setmus)**(std::vector< double > inmu) |
| std::vector< double > | **[getMus](../Classes/classshapeworks_1_1Constraint.md#function-getmus)**() |
| void | **[setLambda](../Classes/classshapeworks_1_1Constraint.md#function-setlambda)**(double inLambda) |
| double | **[getLambda](../Classes/classshapeworks_1_1Constraint.md#function-getlambda)**() |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const =0 |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1Constraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const =0 |
| void | **[updateZ](../Classes/classshapeworks_1_1Constraint.md#function-updatez)**(const Eigen::Vector3d & pt, double C) |
| void | **[updateMu](../Classes/classshapeworks_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C, size_t index) |
| Eigen::Vector3d | **[lagragianGradient](../Classes/classshapeworks_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C, size_t index) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classshapeworks_1_1Constraint.md#function-sgn)**(double val) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< double > | **[mus_](../Classes/classshapeworks_1_1Constraint.md#variable-mus-)**  |
| double | **[z_](../Classes/classshapeworks_1_1Constraint.md#variable-z-)**  |
| double | **[lambda_](../Classes/classshapeworks_1_1Constraint.md#variable-lambda-)**  |

## Public Functions Documentation

### function isViolated

```cpp
inline bool isViolated(
    const vnl_vector< double > & pt
) const
```


### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [shapeworks::FreeFormConstraint::isViolated](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isviolated), [shapeworks::PlaneConstraint::isViolated](../Classes/classshapeworks_1_1PlaneConstraint.md#function-isviolated), [shapeworks::SphereConstraint::isViolated](../Classes/classshapeworks_1_1SphereConstraint.md#function-isviolated)


### function print

```cpp
virtual void print() const =0
```


**Reimplemented by**: [shapeworks::FreeFormConstraint::print](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-print), [shapeworks::PlaneConstraint::print](../Classes/classshapeworks_1_1PlaneConstraint.md#function-print), [shapeworks::SphereConstraint::print](../Classes/classshapeworks_1_1SphereConstraint.md#function-print)


### function setZ

```cpp
inline void setZ(
    double inz
)
```


### function getZ

```cpp
inline double getZ()
```


### function setMus

```cpp
inline void setMus(
    std::vector< double > inmu
)
```


### function getMus

```cpp
inline std::vector< double > getMus()
```


### function setLambda

```cpp
inline void setLambda(
    double inLambda
)
```


### function getLambda

```cpp
inline double getLambda()
```


### function constraintGradient

```cpp
virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [shapeworks::FreeFormConstraint::constraintGradient](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constraintgradient), [shapeworks::PlaneConstraint::constraintGradient](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constraintgradient), [shapeworks::SphereConstraint::constraintGradient](../Classes/classshapeworks_1_1SphereConstraint.md#function-constraintgradient)


### function constraintEval

```cpp
virtual double constraintEval(
    const Eigen::Vector3d & pt
) const =0
```


**Reimplemented by**: [shapeworks::FreeFormConstraint::constraintEval](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constrainteval), [shapeworks::PlaneConstraint::constraintEval](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constrainteval), [shapeworks::SphereConstraint::constraintEval](../Classes/classshapeworks_1_1SphereConstraint.md#function-constrainteval)


### function updateZ

```cpp
void updateZ(
    const Eigen::Vector3d & pt,
    double C
)
```


### function updateMu

```cpp
void updateMu(
    const Eigen::Vector3d & pt,
    double C,
    size_t index
)
```


### function lagragianGradient

```cpp
Eigen::Vector3d lagragianGradient(
    const Eigen::Vector3d & pt,
    double C,
    size_t index
) const
```


## Protected Functions Documentation

### function sgn

```cpp
inline int sgn(
    double val
)
```


## Protected Attributes Documentation

### variable mus_

```cpp
std::vector< double > mus_;
```


### variable z_

```cpp
double z_;
```


### variable lambda_

```cpp
double lambda_;
```


-------------------------------

Updated on 2023-06-04 at 06:35:58 +0000