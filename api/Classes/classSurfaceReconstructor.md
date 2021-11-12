---
title: SurfaceReconstructor

---

# SurfaceReconstructor





## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef float | **[PixelType](../Classes/classSurfaceReconstructor.md#typedef-pixeltype)**  |
| typedef itk::Image< PixelType, 3 > | **[ImageType](../Classes/classSurfaceReconstructor.md#typedef-imagetype)**  |
| typedef itk::ImageFileReader< ImageType > | **[ReaderType](../Classes/classSurfaceReconstructor.md#typedef-readertype)**  |
| typedef double | **[CoordinateRepType](../Classes/classSurfaceReconstructor.md#typedef-coordinatereptype)**  |
| typedef [itk::CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md)< CoordinateRepType, 3 > | **[RBFTransformType](../Classes/classSurfaceReconstructor.md#typedef-rbftransformtype)**  |
| typedef [itk::ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md)< CoordinateRepType, 3 > | **[ThinPlateSplineType](../Classes/classSurfaceReconstructor.md#typedef-thinplatesplinetype)**  |
| typedef itk::LinearInterpolateImageFunction< ImageType, double > | **[InterpolatorType](../Classes/classSurfaceReconstructor.md#typedef-interpolatortype)**  |
| typedef [Reconstruction](../Classes/classReconstruction.md)< [itk::CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md), itk::LinearInterpolateImageFunction, CoordinateRepType, PixelType, ImageType > | **[ReconstructionType](../Classes/classSurfaceReconstructor.md#typedef-reconstructiontype)**  |
| typedef ReconstructionType::PointType | **[PointType](../Classes/classSurfaceReconstructor.md#typedef-pointtype)**  |
| typedef ReconstructionType::PointArrayType | **[PointArrayType](../Classes/classSurfaceReconstructor.md#typedef-pointarraytype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[SurfaceReconstructor](../Classes/classSurfaceReconstructor.md#function-surfacereconstructor)**() |
| void | **[initializeReconstruction](../Classes/classSurfaceReconstructor.md#function-initializereconstruction)**(std::vector< std::vector< itk::Point< double >>> local_pts, std::vector< std::vector< itk::Point< double >>> global_pts, std::vector< std::string > distance_transforms, double maxAngle, float decimationPercent, int numClusters) |
| bool | **[hasDenseMean](../Classes/classSurfaceReconstructor.md#function-hasdensemean)**() |
| void | **[setMean](../Classes/classSurfaceReconstructor.md#function-setmean)**(vtkSmartPointer< vtkPoints > sparseMean, vtkSmartPointer< vtkPolyData > denseMean, std::vector< bool > goodPoints) |
| void | **[writeMeanInfo](../Classes/classSurfaceReconstructor.md#function-writemeaninfo)**(std::string baseName) |
| void | **[readMeanInfo](../Classes/classSurfaceReconstructor.md#function-readmeaninfo)**(std::string dense, std::string sparse, std::string goodPoints) |
| void | **[resetReconstruct](../Classes/classSurfaceReconstructor.md#function-resetreconstruct)**() |
| void | **[set_number_of_clusters](../Classes/classSurfaceReconstructor.md#function-set-number-of-clusters)**(int num_clusters) |
| void | **[set_normal_angle](../Classes/classSurfaceReconstructor.md#function-set-normal-angle)**(double angle) |
| void | **[set_decimation_percent](../Classes/classSurfaceReconstructor.md#function-set-decimation-percent)**(double decimation) |
| bool | **[get_surface_reconstruction_available](../Classes/classSurfaceReconstructor.md#function-get-surface-reconstruction-available)**() |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classSurfaceReconstructor.md#function-build-mesh)**(const vnl_vector< double > & shape) |

## Public Types Documentation

### typedef PixelType

```cpp
typedef float SurfaceReconstructor::PixelType;
```


### typedef ImageType

```cpp
typedef itk::Image< PixelType, 3 > SurfaceReconstructor::ImageType;
```


### typedef ReaderType

```cpp
typedef itk::ImageFileReader< ImageType > SurfaceReconstructor::ReaderType;
```


### typedef CoordinateRepType

```cpp
typedef double SurfaceReconstructor::CoordinateRepType;
```


### typedef RBFTransformType

```cpp
typedef itk::CompactlySupportedRBFSparseKernelTransform< CoordinateRepType, 3> SurfaceReconstructor::RBFTransformType;
```


### typedef ThinPlateSplineType

```cpp
typedef itk::ThinPlateSplineKernelTransform2< CoordinateRepType, 3> SurfaceReconstructor::ThinPlateSplineType;
```


### typedef InterpolatorType

```cpp
typedef itk::LinearInterpolateImageFunction<ImageType, double > SurfaceReconstructor::InterpolatorType;
```


### typedef ReconstructionType

```cpp
typedef Reconstruction< itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction, CoordinateRepType, PixelType, ImageType> SurfaceReconstructor::ReconstructionType;
```


### typedef PointType

```cpp
typedef ReconstructionType::PointType SurfaceReconstructor::PointType;
```


### typedef PointArrayType

```cpp
typedef ReconstructionType::PointArrayType SurfaceReconstructor::PointArrayType;
```


## Public Functions Documentation

### function SurfaceReconstructor

```cpp
SurfaceReconstructor()
```


### function initializeReconstruction

```cpp
void initializeReconstruction(
    std::vector< std::vector< itk::Point< double >>> local_pts,
    std::vector< std::vector< itk::Point< double >>> global_pts,
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
    const vnl_vector< double > & shape
)
```


-------------------------------

Updated on 2021-11-12 at 04:35:45 +0000