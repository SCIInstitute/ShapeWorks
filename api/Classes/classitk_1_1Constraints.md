---
title: itk::Constraints

---

# itk::Constraints





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Constraints](../Classes/classitk_1_1Constraints.md#function-constraints)**() |
| | **[~Constraints](../Classes/classitk_1_1Constraints.md#function-~constraints)**() |
| void | **[addPlane](../Classes/classitk_1_1Constraints.md#function-addplane)**(const vnl_vector< double > & a, const vnl_vector< double > & b, const vnl_vector< double > & c) |
| void | **[addSphere](../Classes/classitk_1_1Constraints.md#function-addsphere)**(const vnl_vector_fixed< double, DIMENSION > & v, double r) |
| void | **[addFreeFormConstraint](../Classes/classitk_1_1Constraints.md#function-addfreeformconstraint)**(std::shared_ptr< [shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh) |
| bool | **[transformConstraints](../Classes/classitk_1_1Constraints.md#function-transformconstraints)**(const vnl_matrix_fixed< double, 4, 4 > & Trans) |
| bool | **[transformPlanes](../Classes/classitk_1_1Constraints.md#function-transformplanes)**(const vnl_matrix_fixed< double, 4, 4 > & Trans) |
| std::stringstream | **[applyBoundaryConstraints](../Classes/classitk_1_1Constraints.md#function-applyboundaryconstraints)**(vnl_vector_fixed< double, 3 > & gradE, const Point< double, 3 > & pos) |
| std::stringstream | **[applyBoundaryConstraints](../Classes/classitk_1_1Constraints.md#function-applyboundaryconstraints)**(vnl_vector_fixed< float, 3 > & gradE, const Point< double, 3 > & pos) |
| std::stringstream | **[applyPlaneConstraints](../Classes/classitk_1_1Constraints.md#function-applyplaneconstraints)**(vnl_vector_fixed< double, 3 > & gradE, const Point< double, 3 > & pos) |
| bool | **[writePlanes](../Classes/classitk_1_1Constraints.md#function-writeplanes)**(std::string filename) |
| bool | **[writeSpheres](../Classes/classitk_1_1Constraints.md#function-writespheres)**(std::string filename) |
| bool | **[writeFreeFormConstraint](../Classes/classitk_1_1Constraints.md#function-writefreeformconstraint)**(std::string filename) |
| bool | **[IsCuttingPlaneDefined](../Classes/classitk_1_1Constraints.md#function-iscuttingplanedefined)**() const |
| bool | **[IsCuttingSphereDefined](../Classes/classitk_1_1Constraints.md#function-iscuttingspheredefined)**() const |
| std::vector< [PlaneConstraint](../Classes/classitk_1_1PlaneConstraint.md) > * | **[getPlaneConstraints](../Classes/classitk_1_1Constraints.md#function-getplaneconstraints)**() |
| std::vector< [SphereConstraint](../Classes/classitk_1_1SphereConstraint.md) > * | **[GetSphereConstraints](../Classes/classitk_1_1Constraints.md#function-getsphereconstraints)**() |
| bool | **[IsAnyViolated](../Classes/classitk_1_1Constraints.md#function-isanyviolated)**(const Point< double, 3 > & pos) |
| std::vector< int > | **[planesViolated](../Classes/classitk_1_1Constraints.md#function-planesviolated)**(Eigen::Vector3d pt) |
| void | **[PrintAll](../Classes/classitk_1_1Constraints.md#function-printall)**() |
| std::string | **[ViolationReport](../Classes/classitk_1_1Constraints.md#function-violationreport)**(const Point< double, 3 > & pos) |
| std::vector< std::vector< double > > | **[ViolationReportData](../Classes/classitk_1_1Constraints.md#function-violationreportdata)**(const Point< double, 3 > & pos) |
| vnl_vector_fixed< double, 3 > | **[ConstraintsGradient](../Classes/classitk_1_1Constraints.md#function-constraintsgradient)**(const Point< double, 3 > & pos) const |
| vnl_vector_fixed< double, 3 > | **[ConstraintsLagrangianGradient](../Classes/classitk_1_1Constraints.md#function-constraintslagrangiangradient)**(const Point< double, 3 > & pos, const Point< double, 3 > & prepos, double C) const |
| void | **[InitializeLagrangianParameters](../Classes/classitk_1_1Constraints.md#function-initializelagrangianparameters)**(double lambda, double mu, double z) const |
| void | **[UpdateZs](../Classes/classitk_1_1Constraints.md#function-updatezs)**(const Point< double, 3 > & pos, double C) |
| void | **[UpdateMus](../Classes/classitk_1_1Constraints.md#function-updatemus)**(const Point< double, 3 > & pos, double C) |
| bool | **[GetActive](../Classes/classitk_1_1Constraints.md#function-getactive)**() |
| void | **[SetActive](../Classes/classitk_1_1Constraints.md#function-setactive)**(bool ac) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< [PlaneConstraint](../Classes/classitk_1_1PlaneConstraint.md) > * | **[planeConsts](../Classes/classitk_1_1Constraints.md#variable-planeconsts)**  |
| std::vector< [SphereConstraint](../Classes/classitk_1_1SphereConstraint.md) > * | **[sphereConsts](../Classes/classitk_1_1Constraints.md#variable-sphereconsts)**  |
| std::vector< [FreeFormConstraint](../Classes/classitk_1_1FreeFormConstraint.md) > * | **[freeFormConsts](../Classes/classitk_1_1Constraints.md#variable-freeformconsts)**  |

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
    const vnl_matrix_fixed< double, 4, 4 > & Trans
)
```


### function transformPlanes

```cpp
bool transformPlanes(
    const vnl_matrix_fixed< double, 4, 4 > & Trans
)
```


### function applyBoundaryConstraints

```cpp
std::stringstream applyBoundaryConstraints(
    vnl_vector_fixed< double, 3 > & gradE,
    const Point< double, 3 > & pos
)
```


### function applyBoundaryConstraints

```cpp
std::stringstream applyBoundaryConstraints(
    vnl_vector_fixed< float, 3 > & gradE,
    const Point< double, 3 > & pos
)
```


### function applyPlaneConstraints

```cpp
std::stringstream applyPlaneConstraints(
    vnl_vector_fixed< double, 3 > & gradE,
    const Point< double, 3 > & pos
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


### function IsCuttingPlaneDefined

```cpp
inline bool IsCuttingPlaneDefined() const
```


### function IsCuttingSphereDefined

```cpp
inline bool IsCuttingSphereDefined() const
```


### function getPlaneConstraints

```cpp
inline std::vector< PlaneConstraint > * getPlaneConstraints()
```


### function GetSphereConstraints

```cpp
inline std::vector< SphereConstraint > * GetSphereConstraints()
```


### function IsAnyViolated

```cpp
inline bool IsAnyViolated(
    const Point< double, 3 > & pos
)
```


### function planesViolated

```cpp
inline std::vector< int > planesViolated(
    Eigen::Vector3d pt
)
```


### function PrintAll

```cpp
inline void PrintAll()
```


### function ViolationReport

```cpp
inline std::string ViolationReport(
    const Point< double, 3 > & pos
)
```


### function ViolationReportData

```cpp
inline std::vector< std::vector< double > > ViolationReportData(
    const Point< double, 3 > & pos
)
```


### function ConstraintsGradient

```cpp
inline vnl_vector_fixed< double, 3 > ConstraintsGradient(
    const Point< double, 3 > & pos
) const
```


### function ConstraintsLagrangianGradient

```cpp
inline vnl_vector_fixed< double, 3 > ConstraintsLagrangianGradient(
    const Point< double, 3 > & pos,
    const Point< double, 3 > & prepos,
    double C
) const
```


### function InitializeLagrangianParameters

```cpp
inline void InitializeLagrangianParameters(
    double lambda,
    double mu,
    double z
) const
```


### function UpdateZs

```cpp
inline void UpdateZs(
    const Point< double, 3 > & pos,
    double C
)
```


### function UpdateMus

```cpp
inline void UpdateMus(
    const Point< double, 3 > & pos,
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


## Protected Attributes Documentation

### variable planeConsts

```cpp
std::vector< PlaneConstraint > * planeConsts;
```


### variable sphereConsts

```cpp
std::vector< SphereConstraint > * sphereConsts;
```


### variable freeFormConsts

```cpp
std::vector< FreeFormConstraint > * freeFormConsts;
```


-------------------------------

Updated on 2021-12-10 at 00:32:10 +0000