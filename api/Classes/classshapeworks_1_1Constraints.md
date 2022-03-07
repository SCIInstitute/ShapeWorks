---
title: shapeworks::Constraints

---

# shapeworks::Constraints





## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::Point< double, 3 > | **[Point3](../Classes/classshapeworks_1_1Constraints.md#using-point3)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Constraints](../Classes/classshapeworks_1_1Constraints.md#function-constraints)**() |
| | **[~Constraints](../Classes/classshapeworks_1_1Constraints.md#function-~constraints)**() |
| void | **[addPlane](../Classes/classshapeworks_1_1Constraints.md#function-addplane)**(const vnl_vector< double > & a, const vnl_vector< double > & b, const vnl_vector< double > & c) |
| void | **[addSphere](../Classes/classshapeworks_1_1Constraints.md#function-addsphere)**(const vnl_vector_fixed< double, DIMENSION > & v, double r) |
| void | **[addFreeFormConstraint](../Classes/classshapeworks_1_1Constraints.md#function-addfreeformconstraint)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh) |
| bool | **[transformConstraints](../Classes/classshapeworks_1_1Constraints.md#function-transformconstraints)**(const vnl_matrix_fixed< double, 4, 4 > & transform) |
| bool | **[transformPlanes](../Classes/classshapeworks_1_1Constraints.md#function-transformplanes)**(const vnl_matrix_fixed< double, 4, 4 > & transform) |
| std::stringstream | **[applyBoundaryConstraints](../Classes/classshapeworks_1_1Constraints.md#function-applyboundaryconstraints)**(vnl_vector_fixed< double, 3 > & gradE, const Point3 & pos) |
| std::stringstream | **[applyBoundaryConstraints](../Classes/classshapeworks_1_1Constraints.md#function-applyboundaryconstraints)**(vnl_vector_fixed< float, 3 > & gradE, const Point3 & pos) |
| std::stringstream | **[applyPlaneConstraints](../Classes/classshapeworks_1_1Constraints.md#function-applyplaneconstraints)**(vnl_vector_fixed< double, 3 > & gradE, const Point3 & pos) |
| bool | **[writePlanes](../Classes/classshapeworks_1_1Constraints.md#function-writeplanes)**(std::string filename) |
| bool | **[writeSpheres](../Classes/classshapeworks_1_1Constraints.md#function-writespheres)**(std::string filename) |
| bool | **[writeFreeFormConstraint](../Classes/classshapeworks_1_1Constraints.md#function-writefreeformconstraint)**(std::string filename) |
| bool | **[isCuttingPlaneDefined](../Classes/classshapeworks_1_1Constraints.md#function-iscuttingplanedefined)**() const |
| bool | **[isCuttingSphereDefined](../Classes/classshapeworks_1_1Constraints.md#function-iscuttingspheredefined)**() const |
| std::vector< [PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md) > & | **[getPlaneConstraints](../Classes/classshapeworks_1_1Constraints.md#function-getplaneconstraints)**() |
| std::vector< [SphereConstraint](../Classes/classshapeworks_1_1SphereConstraint.md) > & | **[getSphereConstraints](../Classes/classshapeworks_1_1Constraints.md#function-getsphereconstraints)**() |
| bool | **[isAnyViolated](../Classes/classshapeworks_1_1Constraints.md#function-isanyviolated)**(const Point3 & pos) |
| std::vector< int > | **[planesViolated](../Classes/classshapeworks_1_1Constraints.md#function-planesviolated)**(Eigen::Vector3d pt) |
| void | **[printAll](../Classes/classshapeworks_1_1Constraints.md#function-printall)**() |
| std::string | **[violationReport](../Classes/classshapeworks_1_1Constraints.md#function-violationreport)**(const Point3 & pos) |
| std::vector< std::vector< double > > | **[violationReportData](../Classes/classshapeworks_1_1Constraints.md#function-violationreportdata)**(const Point3 & pos) |
| vnl_vector_fixed< double, 3 > | **[constraintsGradient](../Classes/classshapeworks_1_1Constraints.md#function-constraintsgradient)**(const Point3 & pos) const |
| vnl_vector_fixed< double, 3 > | **[constraintsLagrangianGradient](../Classes/classshapeworks_1_1Constraints.md#function-constraintslagrangiangradient)**(const Point3 & pos, const Point3 & prepos, double C) |
| void | **[InitializeLagrangianParameters](../Classes/classshapeworks_1_1Constraints.md#function-initializelagrangianparameters)**(double lambda, double mu, double z) |
| void | **[UpdateZs](../Classes/classshapeworks_1_1Constraints.md#function-updatezs)**(const Point3 & pos, double C) |
| void | **[UpdateMus](../Classes/classshapeworks_1_1Constraints.md#function-updatemus)**(const Point3 & pos, double C) |
| bool | **[GetActive](../Classes/classshapeworks_1_1Constraints.md#function-getactive)**() |
| void | **[SetActive](../Classes/classshapeworks_1_1Constraints.md#function-setactive)**(bool ac) |
| void | **[Read](../Classes/classshapeworks_1_1Constraints.md#function-read)**(std::string filename) |
| void | **[Write](../Classes/classshapeworks_1_1Constraints.md#function-write)**(std::string filename) |

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


### function addSphere

```cpp
void addSphere(
    const vnl_vector_fixed< double, DIMENSION > & v,
    double r
)
```


### function addFreeFormConstraint

```cpp
void addFreeFormConstraint(
    std::shared_ptr< shapeworks::Mesh > mesh
)
```


### function transformConstraints

```cpp
bool transformConstraints(
    const vnl_matrix_fixed< double, 4, 4 > & transform
)
```


### function transformPlanes

```cpp
bool transformPlanes(
    const vnl_matrix_fixed< double, 4, 4 > & transform
)
```


### function applyBoundaryConstraints

```cpp
std::stringstream applyBoundaryConstraints(
    vnl_vector_fixed< double, 3 > & gradE,
    const Point3 & pos
)
```


### function applyBoundaryConstraints

```cpp
std::stringstream applyBoundaryConstraints(
    vnl_vector_fixed< float, 3 > & gradE,
    const Point3 & pos
)
```


### function applyPlaneConstraints

```cpp
std::stringstream applyPlaneConstraints(
    vnl_vector_fixed< double, 3 > & gradE,
    const Point3 & pos
)
```


### function writePlanes

```cpp
inline bool writePlanes(
    std::string filename
)
```


### function writeSpheres

```cpp
inline bool writeSpheres(
    std::string filename
)
```


### function writeFreeFormConstraint

```cpp
inline bool writeFreeFormConstraint(
    std::string filename
)
```


### function isCuttingPlaneDefined

```cpp
inline bool isCuttingPlaneDefined() const
```


### function isCuttingSphereDefined

```cpp
inline bool isCuttingSphereDefined() const
```


### function getPlaneConstraints

```cpp
inline std::vector< PlaneConstraint > & getPlaneConstraints()
```


### function getSphereConstraints

```cpp
inline std::vector< SphereConstraint > & getSphereConstraints()
```


### function isAnyViolated

```cpp
bool isAnyViolated(
    const Point3 & pos
)
```


### function planesViolated

```cpp
std::vector< int > planesViolated(
    Eigen::Vector3d pt
)
```


### function printAll

```cpp
void printAll()
```


### function violationReport

```cpp
std::string violationReport(
    const Point3 & pos
)
```


### function violationReportData

```cpp
std::vector< std::vector< double > > violationReportData(
    const Point3 & pos
)
```


### function constraintsGradient

```cpp
vnl_vector_fixed< double, 3 > constraintsGradient(
    const Point3 & pos
) const
```


### function constraintsLagrangianGradient

```cpp
vnl_vector_fixed< double, 3 > constraintsLagrangianGradient(
    const Point3 & pos,
    const Point3 & prepos,
    double C
)
```


### function InitializeLagrangianParameters

```cpp
void InitializeLagrangianParameters(
    double lambda,
    double mu,
    double z
)
```


### function UpdateZs

```cpp
void UpdateZs(
    const Point3 & pos,
    double C
)
```


### function UpdateMus

```cpp
void UpdateMus(
    const Point3 & pos,
    double C
)
```


### function GetActive

```cpp
inline bool GetActive()
```


### function SetActive

```cpp
inline void SetActive(
    bool ac
)
```


### function Read

```cpp
void Read(
    std::string filename
)
```


### function Write

```cpp
void Write(
    std::string filename
)
```


-------------------------------

Updated on 2022-03-07 at 00:21:27 +0000