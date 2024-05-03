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
| int | **[do_pca](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-do-pca)**(std::vector< std::vector< Point > > global_pts, int domainsPerShape =1) |
| int | **[do_pca](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-do-pca)**([ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) particleSystem, int domainsPerShape =1) |
| int | **[import_points](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-import-points)**(std::vector< Eigen::VectorXd > points, std::vector< int > group_ids)<br>Loads a set of point files and pre-computes some statistics.  |
| void | **[compute_multi_level_analysis_statistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-compute-multi-level-analysis-statistics)**(std::vector< Eigen::VectorXd > points, unsigned int dps)<br>Loads a set of point files and pre-computes statistics for multi-level analysis.  |
| int | **[compute_shape_dev_modes_for_mca](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-compute-shape-dev-modes-for-mca)**()<br>Compute shape variations for MLCA.  |
| int | **[compute_relative_pose_modes_for_mca](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-compute-relative-pose-modes-for-mca)**()<br>Compute pose variations for MLCA.  |
| void | **[set_num_particles_per_domain](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-set-num-particles-per-domain)**(const std::vector< int > & num_particles_array)<br>Set number of particles per domain/object (required for multi-level analysis)  |
| int | **[read_point_files](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-read-point-files)**(const std::string & s)<br>Loads a set of point files and pre-computes some statistics.  |
| int | **[write_csv_file](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-write-csv-file)**(const std::string & s)<br>Writes a text file in comma-separated format. Suitable for reading into excel or R or Matlab for analysis.  |
| int | **[compute_modes](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-compute-modes)**()<br>Computes PCA modes from the set of correspondence mode positions. Requires that ReadPointFiles be called first.  |
| int | **[get_num_modes](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-num-modes)**() const<br>Return the number of modes.  |
| int | **[principal_component_projections](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-principal-component-projections)**() |
| int | **[get_num_samples](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-num-samples)**() const<br>Returns the sample size.  |
| int | **[get_group1_num_samples](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group1-num-samples)**() const |
| int | **[get_group2_num_samples](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group2-num-samples)**() const |
| int | **[get_num_dimensions](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-num-dimensions)**() const<br>Returns the number of dimensions (this is number of points times Dimension)  |
| int | **[get_domains_per_shape](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-domains-per-shape)**()<br>Return Number of objects present in Multi-Object [Shape]() Structure.  |
| std::vector< int > | **[NumberOfPointsArray](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-numberofpointsarray)**()<br>Returns Number of [Particles](../Classes/classshapeworks_1_1Particles.md) Array.  |
| int | **[GroupID](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-groupid)**(unsigned int i) const<br>Returns the group ids.  |
| const std::vector< int > & | **[GroupID](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-groupid)**() const |
| const Eigen::MatrixXd & | **[get_eigen_vectors](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigen-vectors)**() const<br>Returns the eigenvectors/values.  |
| const std::vector< double > & | **[get_eigen_values](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigen-values)**() const |
| const Eigen::MatrixXd & | **[get_eigenvectors_rel_pos](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigenvectors-rel-pos)**()<br>Returns the eigenvectors/eigenvalues for morphological and relative pose pariations of MLCA.  |
| const std::vector< double > & | **[get_eigenvalues_shape_dev](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigenvalues-shape-dev)**() |
| const Eigen::MatrixXd & | **[get_eigenvectors_shape_dev](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigenvectors-shape-dev)**() |
| const std::vector< double > & | **[get_eigenvalues_rel_pose](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-eigenvalues-rel-pose)**() |
| const Eigen::VectorXd & | **[get_mean_shape_dev](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-mean-shape-dev)**()<br>Returns the mean vector of within and between subspace.  |
| const Eigen::VectorXd & | **[get_mean_rel_pos](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-mean-rel-pos)**() |
| const Eigen::VectorXd & | **[get_mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-mean)**() const<br>Returns the mean shape.  |
| const Eigen::VectorXd & | **[get_group1_mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group1-mean)**() const |
| const Eigen::VectorXd & | **[get_group2_mean](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group2-mean)**() const |
| const Eigen::VectorXd & | **[get_group_difference](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group-difference)**() const<br>Returns group2 - group1 mean.  |
| int | **[compute_median_shape](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-compute-median-shape)**(const int ID) |
| double | **[l1_norm](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-l1-norm)**(unsigned int a, unsigned int b)<br>Returns the euclidean L1 norm between shape a and b.  |
| Eigen::MatrixXd & | **[get_pca_loadings](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-pca-loadings)**()<br>Returns the component loadings.  |
| const std::vector< double > & | **[get_percent_variance_by_mode](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-percent-variance-by-mode)**() const |
| Eigen::VectorXd | **[get_compactness](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-compactness)**(const std::function< void(float)> & progress_callback =nullptr) const |
| Eigen::VectorXd | **[get_specificity](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-specificity)**(const std::function< void(float)> & progress_callback =nullptr) const |
| Eigen::VectorXd | **[get_generalization](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-generalization)**(const std::function< void(float)> & progress_callback =nullptr) const |
| Eigen::MatrixXd | **[get_group1_matrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group1-matrix)**() const |
| Eigen::MatrixXd | **[get_group2_matrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-get-group2-matrix)**() const |
| Eigen::MatrixXd & | **[matrix](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-matrix)**() |
| void | **[set_num_values_per_particle](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-set-num-values-per-particle)**(int value_per_particle)<br>Set the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar)  |
| int | **[simple_linear_regression](../Classes/classshapeworks_1_1ParticleShapeStatistics.md#function-simple-linear-regression)**(const std::vector< double > & y, const std::vector< double > & x, double & a, double & b) |

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


### function do_pca

```cpp
int do_pca(
    std::vector< std::vector< Point > > global_pts,
    int domainsPerShape =1
)
```


### function do_pca

```cpp
int do_pca(
    ParticleSystemEvaluation particleSystem,
    int domainsPerShape =1
)
```


### function import_points

```cpp
int import_points(
    std::vector< Eigen::VectorXd > points,
    std::vector< int > group_ids
)
```

Loads a set of point files and pre-computes some statistics. 

### function compute_multi_level_analysis_statistics

```cpp
void compute_multi_level_analysis_statistics(
    std::vector< Eigen::VectorXd > points,
    unsigned int dps
)
```

Loads a set of point files and pre-computes statistics for multi-level analysis. 

### function compute_shape_dev_modes_for_mca

```cpp
int compute_shape_dev_modes_for_mca()
```

Compute shape variations for MLCA. 

### function compute_relative_pose_modes_for_mca

```cpp
int compute_relative_pose_modes_for_mca()
```

Compute pose variations for MLCA. 

### function set_num_particles_per_domain

```cpp
void set_num_particles_per_domain(
    const std::vector< int > & num_particles_array
)
```

Set number of particles per domain/object (required for multi-level analysis) 

### function read_point_files

```cpp
int read_point_files(
    const std::string & s
)
```

Loads a set of point files and pre-computes some statistics. 

### function write_csv_file

```cpp
int write_csv_file(
    const std::string & s
)
```

Writes a text file in comma-separated format. Suitable for reading into excel or R or Matlab for analysis. 

### function compute_modes

```cpp
int compute_modes()
```

Computes PCA modes from the set of correspondence mode positions. Requires that ReadPointFiles be called first. 

### function get_num_modes

```cpp
int get_num_modes() const
```

Return the number of modes. 

### function principal_component_projections

```cpp
int principal_component_projections()
```


Computes the principal component loadings, or projections onto the principal componenent axes for each of the samples. ComputeModes must be called first. 


### function get_num_samples

```cpp
inline int get_num_samples() const
```

Returns the sample size. 

### function get_group1_num_samples

```cpp
inline int get_group1_num_samples() const
```


### function get_group2_num_samples

```cpp
inline int get_group2_num_samples() const
```


### function get_num_dimensions

```cpp
inline int get_num_dimensions() const
```

Returns the number of dimensions (this is number of points times Dimension) 

### function get_domains_per_shape

```cpp
inline int get_domains_per_shape()
```

Return Number of objects present in Multi-Object [Shape]() Structure. 

### function NumberOfPointsArray

```cpp
inline std::vector< int > NumberOfPointsArray()
```

Returns Number of [Particles](../Classes/classshapeworks_1_1Particles.md) Array. 

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


### function get_eigen_vectors

```cpp
inline const Eigen::MatrixXd & get_eigen_vectors() const
```

Returns the eigenvectors/values. 

### function get_eigen_values

```cpp
inline const std::vector< double > & get_eigen_values() const
```


### function get_eigenvectors_rel_pos

```cpp
inline const Eigen::MatrixXd & get_eigenvectors_rel_pos()
```

Returns the eigenvectors/eigenvalues for morphological and relative pose pariations of MLCA. 

### function get_eigenvalues_shape_dev

```cpp
inline const std::vector< double > & get_eigenvalues_shape_dev()
```


### function get_eigenvectors_shape_dev

```cpp
inline const Eigen::MatrixXd & get_eigenvectors_shape_dev()
```


### function get_eigenvalues_rel_pose

```cpp
inline const std::vector< double > & get_eigenvalues_rel_pose()
```


### function get_mean_shape_dev

```cpp
inline const Eigen::VectorXd & get_mean_shape_dev()
```

Returns the mean vector of within and between subspace. 

### function get_mean_rel_pos

```cpp
inline const Eigen::VectorXd & get_mean_rel_pos()
```


### function get_mean

```cpp
inline const Eigen::VectorXd & get_mean() const
```

Returns the mean shape. 

### function get_group1_mean

```cpp
inline const Eigen::VectorXd & get_group1_mean() const
```


### function get_group2_mean

```cpp
inline const Eigen::VectorXd & get_group2_mean() const
```


### function get_group_difference

```cpp
inline const Eigen::VectorXd & get_group_difference() const
```

Returns group2 - group1 mean. 

### function compute_median_shape

```cpp
int compute_median_shape(
    const int ID
)
```


Returns the median shape for the set of shapes with Group ID equal to the integer argument. For example, compute_median_shape(0) returns the median shape for the set of shapes with m_groupIDs == 0. The median shape is defined as the shape with the minimum sum of Euclidean L1 norms to all other shapes in that group. Arguments passed to this function are set to the index number of the median shape for Group A and Group B, respectively.*/ 


### function l1_norm

```cpp
double l1_norm(
    unsigned int a,
    unsigned int b
)
```

Returns the euclidean L1 norm between shape a and b. 

### function get_pca_loadings

```cpp
inline Eigen::MatrixXd & get_pca_loadings()
```

Returns the component loadings. 

### function get_percent_variance_by_mode

```cpp
inline const std::vector< double > & get_percent_variance_by_mode() const
```


### function get_compactness

```cpp
Eigen::VectorXd get_compactness(
    const std::function< void(float)> & progress_callback =nullptr
) const
```


### function get_specificity

```cpp
Eigen::VectorXd get_specificity(
    const std::function< void(float)> & progress_callback =nullptr
) const
```


### function get_generalization

```cpp
Eigen::VectorXd get_generalization(
    const std::function< void(float)> & progress_callback =nullptr
) const
```


### function get_group1_matrix

```cpp
Eigen::MatrixXd get_group1_matrix() const
```


### function get_group2_matrix

```cpp
Eigen::MatrixXd get_group2_matrix() const
```


### function matrix

```cpp
inline Eigen::MatrixXd & matrix()
```


### function set_num_values_per_particle

```cpp
inline void set_num_values_per_particle(
    int value_per_particle
)
```

Set the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar) 

### function simple_linear_regression

```cpp
static int simple_linear_regression(
    const std::vector< double > & y,
    const std::vector< double > & x,
    double & a,
    double & b
)
```


Computes a simple linear regression of the first list of values with respect to the second y=a + bx. Returns the estimated parameters a & b. Returns 0 on success and -1 on fail.*/ 


-------------------------------

Updated on 2024-05-03 at 18:21:06 +0000