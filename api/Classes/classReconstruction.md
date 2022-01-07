---
title: Reconstruction

---

# Reconstruction



 [More...](#detailed-description)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef itk::GradientImageFilter< ImageType, PixelType > | **[GradientFilterType](../Classes/classReconstruction.md#typedef-gradientfiltertype)**  |
| typedef itk::GradientMagnitudeImageFilter< ImageType, ImageType > | **[GradientMagnitudeFilterType](../Classes/classReconstruction.md#typedef-gradientmagnitudefiltertype)**  |
| typedef itk::Image< itk::CovariantVector< PixelType, 3 >, 3 > | **[GradientImageType](../Classes/classReconstruction.md#typedef-gradientimagetype)**  |
| typedef itk::ImageRegionIterator< GradientImageType > | **[GradientImageIteratorType](../Classes/classReconstruction.md#typedef-gradientimageiteratortype)**  |
| typedef itk::ImageRegionIterator< ImageType > | **[ImageIteratorType](../Classes/classReconstruction.md#typedef-imageiteratortype)**  |
| typedef itk::ImageFileWriter< ImageType > | **[WriterType](../Classes/classReconstruction.md#typedef-writertype)**  |
| typedef [itk::ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md)< ImageType > | **[ITK2VTKConnectorType](../Classes/classReconstruction.md#typedef-itk2vtkconnectortype)**  |
| typedef itk::AddImageFilter< ImageType, ImageType > | **[AddImageFilterType](../Classes/classReconstruction.md#typedef-addimagefiltertype)**  |
| typedef itk::ResampleImageFilter< ImageType, ImageType > | **[ResampleFilterType](../Classes/classReconstruction.md#typedef-resamplefiltertype)**  |
| typedef TInterpolatorType< ImageType, TCoordRep > | **[InterpolatorType](../Classes/classReconstruction.md#typedef-interpolatortype)**  |
| typedef itk::MultiplyImageFilter< ImageType, ImageType, ImageType > | **[MultiplyByConstantImageFilterType](../Classes/classReconstruction.md#typedef-multiplybyconstantimagefiltertype)**  |
| typedef itk::ImageDuplicator< ImageType > | **[DuplicatorType](../Classes/classReconstruction.md#typedef-duplicatortype)**  |
| typedef TTransformType< TCoordRep, 3 > | **[TransformType](../Classes/classReconstruction.md#typedef-transformtype)**  |
| typedef itk::Point< TCoordRep, 3 > | **[PointType](../Classes/classReconstruction.md#typedef-pointtype)**  |
| typedef std::vector< PointType > | **[PointArrayType](../Classes/classReconstruction.md#typedef-pointarraytype)**  |
| typedef TransformType::PointSetType | **[PointSetType](../Classes/classReconstruction.md#typedef-pointsettype)**  |
| typedef PointSetType::PointIdentifier | **[PointIdType](../Classes/classReconstruction.md#typedef-pointidtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Reconstruction](../Classes/classReconstruction.md#function-reconstruction)**(std::string out_prefix ="", float decimationPercent =0.3f, double angleThresh =45.0f, size_t numClusters =5, bool fixWinding =true, bool doLaplacianSmoothingBeforeDecimation =true, bool doLaplacianSmoothingAfterDecimation =true, float smoothingLambda =0.5f, int smoothingIterations =1, bool usePairwiseNormalsDifferencesForGoodBad =false) |
| | **[~Reconstruction](../Classes/classReconstruction.md#function-~reconstruction)**() |
| vtkSmartPointer< vtkPolyData > | **[getDenseMean](../Classes/classReconstruction.md#function-getdensemean)**(std::vector< PointArrayType > local_pts =std::vector< PointArrayType >(), std::vector< PointArrayType > global_pts =std::vector< PointArrayType >(), std::vector< std::string > distance_transform =std::vector< std::string >()) |
| void | **[reset](../Classes/classReconstruction.md#function-reset)**() |
| void | **[setDecimation](../Classes/classReconstruction.md#function-setdecimation)**(float dec) |
| void | **[setNumClusters](../Classes/classReconstruction.md#function-setnumclusters)**(int num) |
| void | **[setMaxAngle](../Classes/classReconstruction.md#function-setmaxangle)**(double angleDegrees) |
| void | **[setFixWinding](../Classes/classReconstruction.md#function-setfixwinding)**(bool fixWinding) |
| void | **[setLaplacianSmoothingBeforeDecimation](../Classes/classReconstruction.md#function-setlaplaciansmoothingbeforedecimation)**(bool doLaplacianSmoothingBeforeDecimation) |
| void | **[setLaplacianSmoothingAfterDecimation](../Classes/classReconstruction.md#function-setlaplaciansmoothingafterdecimation)**(bool doLaplacianSmoothingAfterDecimation) |
| void | **[setSmoothingLambda](../Classes/classReconstruction.md#function-setsmoothinglambda)**(float smoothingLambda) |
| void | **[setSmoothingIterations](../Classes/classReconstruction.md#function-setsmoothingiterations)**(int smoothingIterations) |
| void | **[setOutputEnabled](../Classes/classReconstruction.md#function-setoutputenabled)**(bool enabled) |
| void | **[setMeanBeforeWarpEnabled](../Classes/classReconstruction.md#function-setmeanbeforewarpenabled)**(bool enabled) |
| vtkSmartPointer< vtkPolyData > | **[getMesh](../Classes/classReconstruction.md#function-getmesh)**(PointArrayType local_pts) |
| void | **[readMeanInfo](../Classes/classReconstruction.md#function-readmeaninfo)**(std::string dense, std::string sparse, std::string goodPoints) |
| bool | **[sparseDone](../Classes/classReconstruction.md#function-sparsedone)**() |
| bool | **[denseDone](../Classes/classReconstruction.md#function-densedone)**() |
| void | **[writeMeanInfo](../Classes/classReconstruction.md#function-writemeaninfo)**(std::string nameBase) |
| vtkSmartPointer< vtkPoints > | **[SparseMean](../Classes/classReconstruction.md#function-sparsemean)**() |
| vtkSmartPointer< vtkPolyData > | **[DenseMean](../Classes/classReconstruction.md#function-densemean)**() |
| std::vector< bool > | **[GoodPoints](../Classes/classReconstruction.md#function-goodpoints)**() |
| std::string | **[OutPrefix](../Classes/classReconstruction.md#function-outprefix)**() |
| void | **[setOutPrefix](../Classes/classReconstruction.md#function-setoutprefix)**(std::string out_prefix) |
| std::vector< PointArrayType > | **[computeSparseMean](../Classes/classReconstruction.md#function-computesparsemean)**(std::vector< PointArrayType > local_pts, itk::Point< TCoordRep > & common_center, bool do_procrustes =true, bool do_procrustes_scaling =false) |
| void | **[setOrigin](../Classes/classReconstruction.md#function-setorigin)**(typename ImageType::PointType origin) |
| void | **[EnablePairwiseNormalsDifferencesForGoodBad](../Classes/classReconstruction.md#function-enablepairwisenormalsdifferencesforgoodbad)**() |
| void | **[DisablePairwiseNormalsDifferencesForGoodBad](../Classes/classReconstruction.md#function-disablepairwisenormalsdifferencesforgoodbad)**() |

## Detailed Description

```cpp
template <template< typename TCoordRep, unsigned > class TTransformType =itk::CompactlySupportedRBFSparseKernelTransform,
template< typename ImageType, typename TCoordRep > class TInterpolatorType =itk::LinearInterpolateImageFunction,
typename TCoordRep  =double,
typename PixelType  =float,
typename ImageType  =itk::Image<PixelType, 3>>
class Reconstruction;
```

## Public Types Documentation

### typedef GradientFilterType

```cpp
typedef itk::GradientImageFilter<ImageType, PixelType> Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::GradientFilterType;
```


### typedef GradientMagnitudeFilterType

```cpp
typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::GradientMagnitudeFilterType;
```


### typedef GradientImageType

```cpp
typedef itk::Image< itk::CovariantVector< PixelType, 3 >, 3 > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::GradientImageType;
```


### typedef GradientImageIteratorType

```cpp
typedef itk::ImageRegionIterator< GradientImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::GradientImageIteratorType;
```


### typedef ImageIteratorType

```cpp
typedef itk::ImageRegionIterator< ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::ImageIteratorType;
```


### typedef WriterType

```cpp
typedef itk::ImageFileWriter< ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::WriterType;
```


### typedef ITK2VTKConnectorType

```cpp
typedef itk::ImageToVTKImageFilter<ImageType> Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::ITK2VTKConnectorType;
```


### typedef AddImageFilterType

```cpp
typedef itk::AddImageFilter<ImageType, ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::AddImageFilterType;
```


### typedef ResampleFilterType

```cpp
typedef itk::ResampleImageFilter<ImageType, ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::ResampleFilterType;
```


### typedef InterpolatorType

```cpp
typedef TInterpolatorType< ImageType, TCoordRep > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::InterpolatorType;
```


### typedef MultiplyByConstantImageFilterType

```cpp
typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::MultiplyByConstantImageFilterType;
```


### typedef DuplicatorType

```cpp
typedef itk::ImageDuplicator< ImageType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::DuplicatorType;
```


### typedef TransformType

```cpp
typedef TTransformType< TCoordRep, 3 > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::TransformType;
```


### typedef PointType

```cpp
typedef itk::Point< TCoordRep, 3 > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::PointType;
```


### typedef PointArrayType

```cpp
typedef std::vector< PointType > Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::PointArrayType;
```


### typedef PointSetType

```cpp
typedef TransformType::PointSetType Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::PointSetType;
```


### typedef PointIdType

```cpp
typedef PointSetType::PointIdentifier Reconstruction< TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType >::PointIdType;
```


## Public Functions Documentation

### function Reconstruction

```cpp
Reconstruction(
    std::string out_prefix ="",
    float decimationPercent =0.3f,
    double angleThresh =45.0f,
    size_t numClusters =5,
    bool fixWinding =true,
    bool doLaplacianSmoothingBeforeDecimation =true,
    bool doLaplacianSmoothingAfterDecimation =true,
    float smoothingLambda =0.5f,
    int smoothingIterations =1,
    bool usePairwiseNormalsDifferencesForGoodBad =false
)
```


### function ~Reconstruction

```cpp
~Reconstruction()
```


### function getDenseMean

```cpp
vtkSmartPointer< vtkPolyData > getDenseMean(
    std::vector< PointArrayType > local_pts =std::vector< PointArrayType >(),
    std::vector< PointArrayType > global_pts =std::vector< PointArrayType >(),
    std::vector< std::string > distance_transform =std::vector< std::string >()
)
```


### function reset

```cpp
void reset()
```


### function setDecimation

```cpp
void setDecimation(
    float dec
)
```


### function setNumClusters

```cpp
void setNumClusters(
    int num
)
```


### function setMaxAngle

```cpp
void setMaxAngle(
    double angleDegrees
)
```


### function setFixWinding

```cpp
void setFixWinding(
    bool fixWinding
)
```


### function setLaplacianSmoothingBeforeDecimation

```cpp
void setLaplacianSmoothingBeforeDecimation(
    bool doLaplacianSmoothingBeforeDecimation
)
```


### function setLaplacianSmoothingAfterDecimation

```cpp
void setLaplacianSmoothingAfterDecimation(
    bool doLaplacianSmoothingAfterDecimation
)
```


### function setSmoothingLambda

```cpp
void setSmoothingLambda(
    float smoothingLambda
)
```


### function setSmoothingIterations

```cpp
void setSmoothingIterations(
    int smoothingIterations
)
```


### function setOutputEnabled

```cpp
void setOutputEnabled(
    bool enabled
)
```


### function setMeanBeforeWarpEnabled

```cpp
void setMeanBeforeWarpEnabled(
    bool enabled
)
```


Set if the mean DT before warp is enabled or not Disabling this allows [Reconstruction](../Classes/classReconstruction.md) to use DTs that are of different sizes and with different origins 


### function getMesh

```cpp
vtkSmartPointer< vtkPolyData > getMesh(
    PointArrayType local_pts
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


### function sparseDone

```cpp
bool sparseDone()
```


### function denseDone

```cpp
bool denseDone()
```


### function writeMeanInfo

```cpp
void writeMeanInfo(
    std::string nameBase
)
```


### function SparseMean

```cpp
inline vtkSmartPointer< vtkPoints > SparseMean()
```


### function DenseMean

```cpp
inline vtkSmartPointer< vtkPolyData > DenseMean()
```


### function GoodPoints

```cpp
inline std::vector< bool > GoodPoints()
```


### function OutPrefix

```cpp
inline std::string OutPrefix()
```


### function setOutPrefix

```cpp
inline void setOutPrefix(
    std::string out_prefix
)
```


### function computeSparseMean

```cpp
std::vector< PointArrayType > computeSparseMean(
    std::vector< PointArrayType > local_pts,
    itk::Point< TCoordRep > & common_center,
    bool do_procrustes =true,
    bool do_procrustes_scaling =false
)
```


### function setOrigin

```cpp
inline void setOrigin(
    typename ImageType::PointType origin
)
```


### function EnablePairwiseNormalsDifferencesForGoodBad

```cpp
inline void EnablePairwiseNormalsDifferencesForGoodBad()
```


### function DisablePairwiseNormalsDifferencesForGoodBad

```cpp
inline void DisablePairwiseNormalsDifferencesForGoodBad()
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000