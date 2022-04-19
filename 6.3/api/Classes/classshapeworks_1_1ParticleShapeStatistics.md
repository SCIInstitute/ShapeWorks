---
title: shapeworks::ParticleShapeStatistics

---

# shapeworks::ParticleShapeStatistics



 [More...](#detailed-description)


`#include <ParticleShapeStatistics.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-particleshapestatistics)**() |
| | **[ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-particleshapestatistics)**(std::shared_ptr< [Project](../Classes/classshapeworks_1_1Project.md) > project) |
| | **[~ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-~particleshapestatistics)**() |
| int | **[DoPCA](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-dopca)**(std::vector< std::vector< Point >> global_pts, int domainsPerShape =1) |
| int | **[DoPCA](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-dopca)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) particleSystem, int domainsPerShape =1) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension )<br>Dimensionality of the domain of the particle system.  |
| int | **[ImportPoints](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-importpoints)**(std::vector< Eigen::VectorXd > points, std::vector< int > group_ids)<br>Loads a set of point files and pre-computes some statistics.  |
| int | **[ReadPointFiles](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-readpointfiles)**(const std::string & s)<br>Loads a set of point files and pre-computes some statistics.  |
| int | **[ReloadPointFiles](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-reloadpointfiles)**()<br>Reloads a set of point files and recomputes some statistics.  |
| int | **[WriteCSVFile](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-writecsvfile)**(const std::string & s)<br>Writes a text file in comma-separated format. Suitable for reading into excel or R or Matlab for analysis.  |
| int | **[WriteCSVFile2](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-writecsvfile2)**(const std::string & s) |
| int | **[ComputeModes](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-computemodes)**()<br>Computes PCA modes from the set of correspondence mode positions. Requires that ReadPointFiles be called first.  |
| int | **[PrincipalComponentProjections](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-principalcomponentprojections)**() |
| int | **[FisherLinearDiscriminant](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-fisherlineardiscriminant)**(unsigned int numModes)<br>Computes fishers linear discriminant line for best group separation.  |
| int | **[SampleSize](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-samplesize)**() const<br>Returns the sample size.  |
| int | **[Group1SampleSize](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-group1samplesize)**() const |
| int | **[Group2SampleSize](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-group2samplesize)**() const |
| int | **[NumberOfDimensions](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-numberofdimensions)**() const<br>Returns the number of dimensions (this is number of points times Dimension)  |
| int | **[GroupID](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-groupid)**(unsigned int i) const<br>Returns the group ids.  |
| const std::vector< int > & | **[GroupID](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-groupid)**() const |
| const Eigen::MatrixXd & | **[Eigenvectors](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-eigenvectors)**() const<br>Returns the eigenvectors/values.  |
| const std::vector< double > & | **[Eigenvalues](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-eigenvalues)**() const |
| const Eigen::VectorXd & | **[Mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-mean)**() const<br>Returns the mean shape.  |
| const Eigen::VectorXd & | **[Group1Mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-group1mean)**() const |
| const Eigen::VectorXd & | **[Group2Mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-group2mean)**() const |
| const Eigen::VectorXd & | **[NormalizedGroupDifference](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-normalizedgroupdifference)**() const<br>Returns group2 - group1 mean.  |
| const Eigen::VectorXd & | **[GroupDifference](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-groupdifference)**() const |
| int | **[ComputeMedianShape](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-computemedianshape)**(const int ID) |
| double | **[L1Norm](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-l1norm)**(unsigned int a, unsigned int b)<br>Returns the euclidean L1 norm between shape a and b.  |
| Eigen::MatrixXd & | **[PCALoadings](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-pcaloadings)**()<br>Returns the component loadings.  |
| const Eigen::VectorXd & | **[FishersLDA](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-fisherslda)**() const<br>Returns the Fisher linear discriminant.  |
| const Eigen::MatrixXd & | **[ShapeMatrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-shapematrix)**() const<br>Returns the shape matrix.  |
| const Eigen::MatrixXd & | **[RecenteredShape](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-recenteredshape)**() const<br>Returns the shape with the mean subtracted.  |
| const std::vector< double > & | **[PercentVarByMode](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-percentvarbymode)**() const |
| int | **[SimpleLinearRegression](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-simplelinearregression)**(const std::vector< double > & y, const std::vector< double > & x, double & a, double & b) const |
| Eigen::VectorXd | **[get_compactness](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-compactness)**(std::function< void(float)> progress_callback) |
| Eigen::VectorXd | **[get_specificity](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-specificity)**(std::function< void(float)> progress_callback) |
| Eigen::VectorXd | **[get_generalization](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-generalization)**(std::function< void(float)> progress_callback) |
| Eigen::MatrixXd | **[get_group1_matrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group1-matrix)**() |
| Eigen::MatrixXd | **[get_group2_matrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group2-matrix)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#variable-vdimension)**  |

## Detailed Description

```cpp
class shapeworks::ParticleShapeStatistics;
```


This class computes various statistics for a set of correspondence positions and group ids. 

## Public Functions Documentation

### function ParticleShapeStatistics

```cpp
inline ParticleShapeStatistics()
```


### function ParticleShapeStatistics

```cpp
ParticleShapeStatistics(
    std::shared_ptr< Project > project
)
```


### function ~ParticleShapeStatistics

```cpp
inline ~ParticleShapeStatistics()
```


### function DoPCA

```cpp
int DoPCA(
    std::vector< std::vector< Point >> global_pts,
    int domainsPerShape =1
)
```


### function DoPCA

```cpp
int DoPCA(
    ParticleSystem particleSystem,
    int domainsPerShape =1
)
```


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```

Dimensionality of the domain of the particle system. 

### function ImportPoints

```cpp
int ImportPoints(
    std::vector< Eigen::VectorXd > points,
    std::vector< int > group_ids
)
```

Loads a set of point files and pre-computes some statistics. 

### function ReadPointFiles

```cpp
int ReadPointFiles(
    const std::string & s
)
```

Loads a set of point files and pre-computes some statistics. 

### function ReloadPointFiles

```cpp
int ReloadPointFiles()
```

Reloads a set of point files and recomputes some statistics. 

### function WriteCSVFile

```cpp
int WriteCSVFile(
    const std::string & s
)
```

Writes a text file in comma-separated format. Suitable for reading into excel or R or Matlab for analysis. 

### function WriteCSVFile2

```cpp
int WriteCSVFile2(
    const std::string & s
)
```


### function ComputeModes

```cpp
int ComputeModes()
```

Computes PCA modes from the set of correspondence mode positions. Requires that ReadPointFiles be called first. 

### function PrincipalComponentProjections

```cpp
int PrincipalComponentProjections()
```


Computes the principal component loadings, or projections onto the principal componenent axes for each of the samples. ComputeModes must be called first. 


### function FisherLinearDiscriminant

```cpp
int FisherLinearDiscriminant(
    unsigned int numModes
)
```

Computes fishers linear discriminant line for best group separation. 

### function SampleSize

```cpp
inline int SampleSize() const
```

Returns the sample size. 

### function Group1SampleSize

```cpp
inline int Group1SampleSize() const
```


### function Group2SampleSize

```cpp
inline int Group2SampleSize() const
```


### function NumberOfDimensions

```cpp
inline int NumberOfDimensions() const
```

Returns the number of dimensions (this is number of points times Dimension) 

### function GroupID

```cpp
inline int GroupID(
    unsigned int i
) const
```

Returns the group ids. 

### function GroupID

```cpp
inline const std::vector< int > & GroupID() const
```


### function Eigenvectors

```cpp
inline const Eigen::MatrixXd & Eigenvectors() const
```

Returns the eigenvectors/values. 

### function Eigenvalues

```cpp
inline const std::vector< double > & Eigenvalues() const
```


### function Mean

```cpp
inline const Eigen::VectorXd & Mean() const
```

Returns the mean shape. 

### function Group1Mean

```cpp
inline const Eigen::VectorXd & Group1Mean() const
```


### function Group2Mean

```cpp
inline const Eigen::VectorXd & Group2Mean() const
```


### function NormalizedGroupDifference

```cpp
inline const Eigen::VectorXd & NormalizedGroupDifference() const
```

Returns group2 - group1 mean. 

### function GroupDifference

```cpp
inline const Eigen::VectorXd & GroupDifference() const
```


### function ComputeMedianShape

```cpp
int ComputeMedianShape(
    const int ID
)
```


Returns the median shape for the set of shapes with Group ID equal to the integer argument. For example, ComputeMedianShape(0) returns the median shape for the set of shapes with m_groupIDs == 0. The median shape is defined as the shape with the minimum sum of Euclidean L1 norms to all other shapes in that group. Arguments passed to this function are set to the index number of the median shape for Group A and Group B, respectively.*/ 


### function L1Norm

```cpp
double L1Norm(
    unsigned int a,
    unsigned int b
)
```

Returns the euclidean L1 norm between shape a and b. 

### function PCALoadings

```cpp
inline Eigen::MatrixXd & PCALoadings()
```

Returns the component loadings. 

### function FishersLDA

```cpp
inline const Eigen::VectorXd & FishersLDA() const
```

Returns the Fisher linear discriminant. 

### function ShapeMatrix

```cpp
inline const Eigen::MatrixXd & ShapeMatrix() const
```

Returns the shape matrix. 

### function RecenteredShape

```cpp
inline const Eigen::MatrixXd & RecenteredShape() const
```

Returns the shape with the mean subtracted. 

### function PercentVarByMode

```cpp
inline const std::vector< double > & PercentVarByMode() const
```


### function SimpleLinearRegression

```cpp
int SimpleLinearRegression(
    const std::vector< double > & y,
    const std::vector< double > & x,
    double & a,
    double & b
) const
```


Computes a simple linear regression of the first list of values with respect to the second y=a + bx. Returns the estimated parameters a & b. Returns 0 on success and -1 on fail.*/ 


### function get_compactness

```cpp
Eigen::VectorXd get_compactness(
    std::function< void(float)> progress_callback
)
```


### function get_specificity

```cpp
Eigen::VectorXd get_specificity(
    std::function< void(float)> progress_callback
)
```


### function get_generalization

```cpp
Eigen::VectorXd get_generalization(
    std::function< void(float)> progress_callback
)
```


### function get_group1_matrix

```cpp
Eigen::MatrixXd get_group1_matrix()
```


### function get_group2_matrix

```cpp
Eigen::MatrixXd get_group2_matrix()
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
```


-------------------------------

Updated on 2022-04-19 at 06:47:58 +0000