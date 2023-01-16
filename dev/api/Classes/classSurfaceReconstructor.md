---
title: SurfaceReconstructor

---

# SurfaceReconstructor





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[SurfaceReconstructor](../Classes/classSurfaceReconstructor.md#function-surfacereconstructor)**() |
| | **[~SurfaceReconstructor](../Classes/classSurfaceReconstructor.md#function-~surfacereconstructor)**() |
| void | **[initializeReconstruction](../Classes/classSurfaceReconstructor.md#function-initializereconstruction)**(std::vector< std::vector< itk::Point< double > > > local_pts, std::vector< std::vector< itk::Point< double > > > global_pts, std::vector< std::string > distance_transforms, double maxAngle, float decimationPercent, int numClusters) |
| bool | **[hasDenseMean](../Classes/classSurfaceReconstructor.md#function-hasdensemean)**() |
| void | **[setMean](../Classes/classSurfaceReconstructor.md#function-setmean)**(vtkSmartPointer< vtkPoints > sparseMean, vtkSmartPointer< vtkPolyData > denseMean, std::vector< bool > goodPoints) |
| void | **[writeMeanInfo](../Classes/classSurfaceReconstructor.md#function-writemeaninfo)**(std::string baseName) |
| void | **[readMeanInfo](../Classes/classSurfaceReconstructor.md#function-readmeaninfo)**(std::string dense, std::string sparse, std::string goodPoints) |
| void | **[resetReconstruct](../Classes/classSurfaceReconstructor.md#function-resetreconstruct)**() |
| void | **[set_number_of_clusters](../Classes/classSurfaceReconstructor.md#function-set-number-of-clusters)**(int num_clusters) |
| void | **[set_normal_angle](../Classes/classSurfaceReconstructor.md#function-set-normal-angle)**(double angle) |
| void | **[set_decimation_percent](../Classes/classSurfaceReconstructor.md#function-set-decimation-percent)**(double decimation) |
| bool | **[get_surface_reconstruction_available](../Classes/classSurfaceReconstructor.md#function-get-surface-reconstruction-available)**() |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classSurfaceReconstructor.md#function-build-mesh)**(const Eigen::VectorXd & shape) |

## Public Functions Documentation

### function SurfaceReconstructor

```cpp
SurfaceReconstructor()
```


### function ~SurfaceReconstructor

```cpp
~SurfaceReconstructor()
```


### function initializeReconstruction

```cpp
void initializeReconstruction(
    std::vector< std::vector< itk::Point< double > > > local_pts,
    std::vector< std::vector< itk::Point< double > > > global_pts,
    std::vector< std::string > distance_transforms,
    double maxAngle,
    float decimationPercent,
    int numClusters
)
```


### function hasDenseMean

```cpp
bool hasDenseMean()
```


### function setMean

```cpp
void setMean(
    vtkSmartPointer< vtkPoints > sparseMean,
    vtkSmartPointer< vtkPolyData > denseMean,
    std::vector< bool > goodPoints
)
```


### function writeMeanInfo

```cpp
void writeMeanInfo(
    std::string baseName
)
```


### function readMeanInfo

```cpp
void readMeanInfo(
    std::string dense,
    std::string sparse,
    std::string goodPoints
)
```


### function resetReconstruct

```cpp
void resetReconstruct()
```


### function set_number_of_clusters

```cpp
void set_number_of_clusters(
    int num_clusters
)
```


### function set_normal_angle

```cpp
void set_normal_angle(
    double angle
)
```


### function set_decimation_percent

```cpp
void set_decimation_percent(
    double decimation
)
```


### function get_surface_reconstruction_available

```cpp
bool get_surface_reconstruction_available()
```


### function build_mesh

```cpp
vtkSmartPointer< vtkPolyData > build_mesh(
    const Eigen::VectorXd & shape
)
```


-------------------------------

Updated on 2023-01-16 at 21:02:45 +0000