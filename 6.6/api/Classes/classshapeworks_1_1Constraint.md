---
title: shapeworks::Constraint

---

# shapeworks::Constraint



 [More...](#detailed-description)


`#include <Constraint.h>`

Inherited by [shapeworks::FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md), [shapeworks::PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)**(const vnl_vector< double > & pt) const<br>Returns if pt in vnl_vector format is violated by the constraint.  |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1Constraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const =0<br>Returns if pt in Eigen format is violated by the constraint.  |
| virtual void | **[print](../Classes/classshapeworks_1_1Constraint.md#function-print)**() const =0<br>Prints the constraint neatly.  |
| void | **[setMus](../Classes/classshapeworks_1_1Constraint.md#function-setmus)**(std::vector< double > inmu)<br>Initializes mu.  |
| std::vector< double > | **[getMus](../Classes/classshapeworks_1_1Constraint.md#function-getmus)**()<br>Gets mu.  |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1Constraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const =0<br>Returns the gradient of the constraint.  |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1Constraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const =0<br>Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary.  |
| void | **[updateMu](../Classes/classshapeworks_1_1Constraint.md#function-updatemu)**(const Eigen::Vector3d & pt, double C, size_t index)<br>Updates the value of mu according to the augmented lagrangian update.  |
| Eigen::Vector3d | **[lagragianGradient](../Classes/classshapeworks_1_1Constraint.md#function-lagragiangradient)**(const Eigen::Vector3d & pt, double C, size_t index) const<br>Computes the lagrangian gradient based on lagrangian inequality equations. NOTE: Not actually lagrangian. We are using quadratic penalty and not lagrangian because it works better.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| int | **[sgn](../Classes/classshapeworks_1_1Constraint.md#function-sgn)**(double val)<br>Returns the sign of the double.  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< double > | **[mus_](../Classes/classshapeworks_1_1Constraint.md#variable-mus-)** <br>Mu is the lagrangian momentum term.  |

## Detailed Description

```cpp
class shapeworks::Constraint;
```


This class is the general constraint class. Each instance represents a single constraint, either cutting-plane, sphere or free-form. They all inherit from this class. This class containts all the infrastructure to handle gradients and evaluations, which is shared among all constraint types. NOTE: Not actually using the augmented lagrangian. We are using quadratic penalty and not lagrangian because it works better. 

## Public Functions Documentation

### function isViolated

```cpp
inline bool isViolated(
    const vnl_vector< double > & pt
) const
```

Returns if pt in vnl_vector format is violated by the constraint. 

### function isViolated

```cpp
virtual bool isViolated(
    const Eigen::Vector3d & pt
) const =0
```

Returns if pt in Eigen format is violated by the constraint. 

**Reimplemented by**: [shapeworks::FreeFormConstraint::isViolated](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isviolated), [shapeworks::PlaneConstraint::isViolated](../Classes/classshapeworks_1_1PlaneConstraint.md#function-isviolated)


### function print

```cpp
virtual void print() const =0
```

Prints the constraint neatly. 

**Reimplemented by**: [shapeworks::FreeFormConstraint::print](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-print), [shapeworks::PlaneConstraint::print](../Classes/classshapeworks_1_1PlaneConstraint.md#function-print)


### function setMus

```cpp
inline void setMus(
    std::vector< double > inmu
)
```

Initializes mu. 

### function getMus

```cpp
inline std::vector< double > getMus()
```

Gets mu. 

### function constraintGradient

```cpp
virtual Eigen::Vector3d constraintGradient(
    const Eigen::Vector3d & pt
) const =0
```

Returns the gradient of the constraint. 

**Reimplemented by**: [shapeworks::FreeFormConstraint::constraintGradient](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constraintgradient), [shapeworks::PlaneConstraint::constraintGradient](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constraintgradient)


### function constraintEval

```cpp
virtual double constraintEval(
    const Eigen::Vector3d & pt
) const =0
```

Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary. 

**Reimplemented by**: [shapeworks::FreeFormConstraint::constraintEval](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constrainteval), [shapeworks::PlaneConstraint::constraintEval](../Classes/classshapeworks_1_1PlaneConstraint.md#function-constrainteval)


### function updateMu

```cpp
void updateMu(
    const Eigen::Vector3d & pt,
    double C,
    size_t index
)
```

Updates the value of mu according to the augmented lagrangian update. 

### function lagragianGradient

```cpp
Eigen::Vector3d lagragianGradient(
    const Eigen::Vector3d & pt,
    double C,
    size_t index
) const
```

Computes the lagrangian gradient based on lagrangian inequality equations. NOTE: Not actually lagrangian. We are using quadratic penalty and not lagrangian because it works better. 

## Protected Functions Documentation

### function sgn

```cpp
inline int sgn(
    double val
)
```

Returns the sign of the double. 

## Protected Attributes Documentation

### variable mus_

```cpp
std::vector< double > mus_;
```

Mu is the lagrangian momentum term. 

-------------------------------

Updated on 2024-07-12 at 16:32:47 +0000