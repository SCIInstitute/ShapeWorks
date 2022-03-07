---
title: shapeworks::FreeFormConstraint

---

# shapeworks::FreeFormConstraint





Inherits from [shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-freeformconstraint)**() |
| void | **[setMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-setmesh)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh) |
| std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > | **[getMesh](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-getmesh)**() |
| virtual bool | **[isViolated](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-isviolated)**(const Eigen::Vector3d & pt) const override |
| virtual void | **[print](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-print)**() const override |
| virtual Eigen::Vector3d | **[constraintGradient](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constraintgradient)**(const Eigen::Vector3d & pt) const override |
| virtual double | **[constraintEval](../Classes/classshapeworks_1_1FreeFormConstraint.md#function-constrainteval)**(const Eigen::Vector3d & pt) const override |

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


### function isViolated

```cpp
inline virtual bool isViolated(
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


-------------------------------

Updated on 2022-03-07 at 00:21:27 +0000