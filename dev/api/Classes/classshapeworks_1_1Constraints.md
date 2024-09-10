---
title: shapeworks::Constraints

---

# shapeworks::Constraints



 [More...](#detailed-description)


`#include <Constraints.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::Point< double, 3 > | **[Point3](../Classes/classshapeworks_1_1Constraints.md#using-point3)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Constraints](../Classes/classshapeworks_1_1Constraints.md#function-constraints)**()<br>[Constraints](../Classes/classshapeworks_1_1Constraints.md) are inactive when initialized.  |
| | **[~Constraints](../Classes/classshapeworks_1_1Constraints.md#function-~constraints)**() |
| void | **[addPlane](../Classes/classshapeworks_1_1Constraints.md#function-addplane)**(const vnl_vector< double > & a, const vnl_vector< double > & b, const vnl_vector< double > & c)<br>Adds a cutting-plane boundary constraint. [Constraints](../Classes/classshapeworks_1_1Constraints.md) are used to isolate areas of interest on shape surfaces.  |
| void | **[addFreeFormConstraint](../Classes/classshapeworks_1_1Constraints.md#function-addfreeformconstraint)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh)<br>Adds the free-form boundary constraint, or FFC. [Constraints](../Classes/classshapeworks_1_1Constraints.md) are used to isolate areas of interest on shape surfaces.  |
| bool | **[transformConstraints](../Classes/classshapeworks_1_1Constraints.md#function-transformconstraints)**(const vnl_matrix_fixed< double, 4, 4 > & transform)<br>Applies transformations to free-form constraints (FFCs)  |
| bool | **[transformPlanes](../Classes/classshapeworks_1_1Constraints.md#function-transformplanes)**(const vnl_matrix_fixed< double, 4, 4 > & transform)<br>Applies transformations to cutting-plane constraints.  |
| std::vector< [PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md) > & | **[getPlaneConstraints](../Classes/classshapeworks_1_1Constraints.md#function-getplaneconstraints)**()<br>Returns the vector that constains all plane constraints, of type [PlaneConstraint](). See class [PlaneConstraint]() for more info.  |
| [FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md) & | **[getFreeformConstraint](../Classes/classshapeworks_1_1Constraints.md#function-getfreeformconstraint)**()<br>Returns the free form constraint, of type [FreeFormConstraint](). See class [FreeFormConstraint]() for more info.  |
| bool | **[isAnyViolated](../Classes/classshapeworks_1_1Constraints.md#function-isanyviolated)**(const Point3 & pos)<br>Returns true if any constraint is violated by point pos.  |
| void | **[printAll](../Classes/classshapeworks_1_1Constraints.md#function-printall)**()<br>Prints all constraints in a neat format. Make sure to disable multithreading if printing within to optimization to avoid jumbled output.  |
| std::string | **[violationReport](../Classes/classshapeworks_1_1Constraints.md#function-violationreport)**(const Point3 & pos, size_t index)<br>Prints out a comprehensive violation report with each constraint that is violated and the magnitude of such violation by point pos. Index can be 0 if one does not have the index.  |
| std::vector< std::vector< double > > | **[violationReportData](../Classes/classshapeworks_1_1Constraints.md#function-violationreportdata)**(const Point3 & pos)<br>Returns the distance to every cosntraint with [0] plane, [1] sphere, and [2] FFC.  |
| vnl_vector_fixed< double, 3 > | **[constraintsLagrangianGradient](../Classes/classshapeworks_1_1Constraints.md#function-constraintslagrangiangradient)**(const Point3 & pos, const Point3 & prepos, double C, size_t index)<br>Returns the constraint gradient. prepos is the current particle, pos is the particle with an optimization update applied, C can be 1, and index is the index of the particle.  |
| void | **[InitializeLagrangianParameters](../Classes/classshapeworks_1_1Constraints.md#function-initializelagrangianparameters)**(std::vector< double > mus)<br>Initializes lagrangian parameter mus.  |
| void | **[UpdateMus](../Classes/classshapeworks_1_1Constraints.md#function-updatemus)**(const Point3 & pos, double C, size_t index)<br>Updates mus, the momentum variable of the augmented lagrangian.  |
| bool | **[GetActive](../Classes/classshapeworks_1_1Constraints.md#function-getactive)**()<br>Gets the variable active, which determines whether constraints are being used.  |
| void | **[SetActive](../Classes/classshapeworks_1_1Constraints.md#function-setactive)**(bool ac)<br>Sets the variable active, which determines whether constraints are being used.  |
| void | **[read](../Classes/classshapeworks_1_1Constraints.md#function-read)**(std::string filename)<br>Reads in a constraint json file named filename.  |
| void | **[write](../Classes/classshapeworks_1_1Constraints.md#function-write)**(std::string filename)<br>Writes out a constraint json file named filename.  |
| bool | **[hasConstraints](../Classes/classshapeworks_1_1Constraints.md#function-hasconstraints)**() |
| void | **[clipMesh](../Classes/classshapeworks_1_1Constraints.md#function-clipmesh)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh)<br>Clips the mesh that has been passed into it by the constraints stored in this object.  |

## Detailed Description

```cpp
class shapeworks::Constraints;
```


This class contains the set of boundary constraints that are appleid to the shapes in the dataset. They are used to isolate areas of interest on shape surfaces/volumes. These boundary constraints currently take the form of cutting planes, cutting spheres (deprecated), or free-form constraints (FFCs). Do not confuse these with the ApplyConstraints function found in the "domain" code, they refer to the action of snapping to the surface and do not serve to isolate areas of interest.

Please check out Libs/Optimize/Constraints/Constraints for a constraints roadmap 

## Public Types Documentation

### using Point3

```cpp
using shapeworks::Constraints::Point3 =  itk::Point<double, 3>;
```


## Public Functions Documentation

### function Constraints

```cpp
inline Constraints()
```

[Constraints](../Classes/classshapeworks_1_1Constraints.md) are inactive when initialized. 

### function ~Constraints

```cpp
inline ~Constraints()
```


### function addPlane

```cpp
void addPlane(
    const vnl_vector< double > & a,
    const vnl_vector< double > & b,
    const vnl_vector< double > & c
)
```

Adds a cutting-plane boundary constraint. [Constraints](../Classes/classshapeworks_1_1Constraints.md) are used to isolate areas of interest on shape surfaces. 

### function addFreeFormConstraint

```cpp
void addFreeFormConstraint(
    std::shared_ptr< shapeworks::Mesh > mesh
)
```

Adds the free-form boundary constraint, or FFC. [Constraints](../Classes/classshapeworks_1_1Constraints.md) are used to isolate areas of interest on shape surfaces. 

### function transformConstraints

```cpp
bool transformConstraints(
    const vnl_matrix_fixed< double, 4, 4 > & transform
)
```

Applies transformations to free-form constraints (FFCs) 

### function transformPlanes

```cpp
bool transformPlanes(
    const vnl_matrix_fixed< double, 4, 4 > & transform
)
```

Applies transformations to cutting-plane constraints. 

### function getPlaneConstraints

```cpp
inline std::vector< PlaneConstraint > & getPlaneConstraints()
```

Returns the vector that constains all plane constraints, of type [PlaneConstraint](). See class [PlaneConstraint]() for more info. 

### function getFreeformConstraint

```cpp
FreeFormConstraint & getFreeformConstraint()
```

Returns the free form constraint, of type [FreeFormConstraint](). See class [FreeFormConstraint]() for more info. 

### function isAnyViolated

```cpp
bool isAnyViolated(
    const Point3 & pos
)
```

Returns true if any constraint is violated by point pos. 

### function printAll

```cpp
void printAll()
```

Prints all constraints in a neat format. Make sure to disable multithreading if printing within to optimization to avoid jumbled output. 

### function violationReport

```cpp
std::string violationReport(
    const Point3 & pos,
    size_t index
)
```

Prints out a comprehensive violation report with each constraint that is violated and the magnitude of such violation by point pos. Index can be 0 if one does not have the index. 

### function violationReportData

```cpp
std::vector< std::vector< double > > violationReportData(
    const Point3 & pos
)
```

Returns the distance to every cosntraint with [0] plane, [1] sphere, and [2] FFC. 

### function constraintsLagrangianGradient

```cpp
vnl_vector_fixed< double, 3 > constraintsLagrangianGradient(
    const Point3 & pos,
    const Point3 & prepos,
    double C,
    size_t index
)
```

Returns the constraint gradient. prepos is the current particle, pos is the particle with an optimization update applied, C can be 1, and index is the index of the particle. 

### function InitializeLagrangianParameters

```cpp
void InitializeLagrangianParameters(
    std::vector< double > mus
)
```

Initializes lagrangian parameter mus. 

### function UpdateMus

```cpp
void UpdateMus(
    const Point3 & pos,
    double C,
    size_t index
)
```

Updates mus, the momentum variable of the augmented lagrangian. 

### function GetActive

```cpp
inline bool GetActive()
```

Gets the variable active, which determines whether constraints are being used. 

### function SetActive

```cpp
inline void SetActive(
    bool ac
)
```

Sets the variable active, which determines whether constraints are being used. 

### function read

```cpp
void read(
    std::string filename
)
```

Reads in a constraint json file named filename. 

### function write

```cpp
void write(
    std::string filename
)
```

Writes out a constraint json file named filename. 

### function hasConstraints

```cpp
bool hasConstraints()
```


### function clipMesh

```cpp
void clipMesh(
    Mesh & mesh
)
```

Clips the mesh that has been passed into it by the constraints stored in this object. 

-------------------------------

Updated on 2024-09-10 at 03:33:07 +0000