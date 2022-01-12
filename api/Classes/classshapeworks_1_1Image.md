---
title: shapeworks::Image

---

# shapeworks::Image

**Module:** **[Image Classes](../Modules/group__Group-Image.md)**



 [More...](#detailed-description)


`#include <Image.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[InterpolationType](../Classes/classshapeworks_1_1Image.md#enum-interpolationtype)** { Linear, NearestNeighbor} |
| using float | **[PixelType](../Classes/classshapeworks_1_1Image.md#using-pixeltype)**  |
| using itk::Image< PixelType, 3 > | **[ImageType](../Classes/classshapeworks_1_1Image.md#using-imagetype)**  |
| using itk::StatisticsImageFilter< ImageType >::Pointer | **[StatsPtr](../Classes/classshapeworks_1_1Image.md#using-statsptr)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Image](../Classes/classshapeworks_1_1Image.md#function-image)**(const std::string & pathname) |
| | **[Image](../Classes/classshapeworks_1_1Image.md#function-image)**(ImageType::Pointer imagePtr) |
| | **[Image](../Classes/classshapeworks_1_1Image.md#function-image)**(const vtkSmartPointer< vtkImageData > vtkImage) |
| | **[Image](../Classes/classshapeworks_1_1Image.md#function-image)**([Image](../Classes/classshapeworks_1_1Image.md) && img) |
| | **[Image](../Classes/classshapeworks_1_1Image.md#function-image)**(const [Image](../Classes/classshapeworks_1_1Image.md) & img) |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator=](../Classes/classshapeworks_1_1Image.md#function-operator=)**(const [Image](../Classes/classshapeworks_1_1Image.md) & img) |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator=](../Classes/classshapeworks_1_1Image.md#function-operator=)**([Image](../Classes/classshapeworks_1_1Image.md) && img)<br>lvalue assignment operator  |
| | **[operator ImageType::Pointer](../Classes/classshapeworks_1_1Image.md#function-operator-imagetypepointer)**()<br>rvalue assignment operator  |
| ImageType::Pointer | **[getITKImage](../Classes/classshapeworks_1_1Image.md#function-getitkimage)**() const |
| vtkSmartPointer< vtkImageData > | **[getVTKImage](../Classes/classshapeworks_1_1Image.md#function-getvtkimage)**() const<br>creates a VTK filter for the given image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator-](../Classes/classshapeworks_1_1Image.md#function-operator-)**()<br>negation operator  |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator+](../Classes/classshapeworks_1_1Image.md#function-operator+)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other) const<br>plus operator to add two images  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator+=](../Classes/classshapeworks_1_1Image.md#function-operator+=)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other) |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator-](../Classes/classshapeworks_1_1Image.md#function-operator-)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other) const<br>minus operator to add two images  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator-=](../Classes/classshapeworks_1_1Image.md#function-operator-=)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other) |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator*](../Classes/classshapeworks_1_1Image.md#function-operator*)**(const PixelType x) const<br>multiply operator to scale contents of an image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator*=](../Classes/classshapeworks_1_1Image.md#function-operator*=)**(const PixelType x) |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator/](../Classes/classshapeworks_1_1Image.md#function-operator/)**(const PixelType x) const<br>divide operator to scale contents of an image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator/=](../Classes/classshapeworks_1_1Image.md#function-operator/=)**(const PixelType x) |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator+](../Classes/classshapeworks_1_1Image.md#function-operator+)**(const PixelType x) const<br>plus operator to shift contents of an image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator+=](../Classes/classshapeworks_1_1Image.md#function-operator+=)**(const PixelType x) |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[operator-](../Classes/classshapeworks_1_1Image.md#function-operator-)**(const PixelType x) const<br>minus operator to shift contents of an image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[operator-=](../Classes/classshapeworks_1_1Image.md#function-operator-=)**(const PixelType x) |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[antialias](../Classes/classshapeworks_1_1Image.md#function-antialias)**(unsigned iterations =50, double maxRMSErr =0.01f, int layers =3)<br>antialiases image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[recenter](../Classes/classshapeworks_1_1Image.md#function-recenter)**()<br>helper identical to setOrigin(image.center()) changing origin (in the image header) to physcial center of the image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[resample](../Classes/classshapeworks_1_1Image.md#function-resample)**(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing, const ImageType::DirectionType direction, InterpolationType interp =NearestNeighbor)<br>resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[resample](../Classes/classshapeworks_1_1Image.md#function-resample)**(const Vector & physicalSpacing, InterpolationType interp =Linear)<br>resamples image using new physical spacing, updating logical dims to keep all image data for this spacing  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[resample](../Classes/classshapeworks_1_1Image.md#function-resample)**(double isoSpacing =1.0, InterpolationType interp =Linear)<br>resamples image using isotropic physical spacing  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[resize](../Classes/classshapeworks_1_1Image.md#function-resize)**(Dims logicalDims, InterpolationType interp =Linear)<br>changes logical image size, computing new physical spacing based on this size (i.e., physical image size remains the same)  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[pad](../Classes/classshapeworks_1_1Image.md#function-pad)**(int padding, PixelType value =0.0)<br>pads an image by same number of voxels in all directions with constant value  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[pad](../Classes/classshapeworks_1_1Image.md#function-pad)**(int padx, int pady, int padz, PixelType value =0.0)<br>pads an image by desired number of voxels in each direction with constant value  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[pad](../Classes/classshapeworks_1_1Image.md#function-pad)**([IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) & region, PixelType value =0.0)<br>pads an image to include the given region with constant value  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[translate](../Classes/classshapeworks_1_1Image.md#function-translate)**(const Vector3 & v)<br>helper to simply translate image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[scale](../Classes/classshapeworks_1_1Image.md#function-scale)**(const Vector3 & v)<br>helper to simply scale image around center (not origin)  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[rotate](../Classes/classshapeworks_1_1Image.md#function-rotate)**(const double angle, const Vector3 & axis)<br>helper to simply rotate around axis through center (not origin) by given angle (in radians)  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[rotate](../Classes/classshapeworks_1_1Image.md#function-rotate)**(const double angle, Axis axis)<br>helper to simply rotate around axis through center (not origin) by given angle (in radians)  |
| TransformPtr | **[createCenterOfMassTransform](../Classes/classshapeworks_1_1Image.md#function-createcenterofmasstransform)**()<br>creates a transform that translates center of mass to center of image  |
| TransformPtr | **[createRigidRegistrationTransform](../Classes/classshapeworks_1_1Image.md#function-createrigidregistrationtransform)**(const [Image](../Classes/classshapeworks_1_1Image.md) & target_dt, float isoValue =0.0, unsigned iterations =20)<br>creates transform to target image using iterative closest point (ICP) registration; images MUST be distance transforms; isovalue is used to create meshes from these distance transform images, which are then passed to ICP for the given number of iterations  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyTransform](../Classes/classshapeworks_1_1Image.md#function-applytransform)**(const TransformPtr transform, InterpolationType interp =Linear)<br>applies the given transformation to the image by using resampling filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyTransform](../Classes/classshapeworks_1_1Image.md#function-applytransform)**(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing, const ImageType::DirectionType direction, InterpolationType interp =NearestNeighbor)<br>applies the given transformation to the image by using resampling filter with new origin, dims, spacing and direction values  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[extractLabel](../Classes/classshapeworks_1_1Image.md#function-extractlabel)**(const PixelType label =1.0)<br>extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[closeHoles](../Classes/classshapeworks_1_1Image.md#function-closeholes)**(const PixelType foreground =0.0)<br>closes holes in a given volume, default foreground value assumes a binary volume  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[binarize](../Classes/classshapeworks_1_1Image.md#function-binarize)**(PixelType minVal =0.0, PixelType maxVal =std::numeric_limits< PixelType >::[max](../Classes/classshapeworks_1_1Image.md#function-max)(), PixelType innerVal =1.0, PixelType outerVal =0.0)<br>threholds image into binary label based on upper and lower intensity bounds given by user  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[computeDT](../Classes/classshapeworks_1_1Image.md#function-computedt)**(PixelType isoValue =0.0)<br>computes distance transform volume from a (preferably antialiased) binary image using the specified isovalue  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyCurvatureFilter](../Classes/classshapeworks_1_1Image.md#function-applycurvaturefilter)**(unsigned iterations =10)<br>denoises an image using curvature driven flow using curvature flow image filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyGradientFilter](../Classes/classshapeworks_1_1Image.md#function-applygradientfilter)**()<br>computes gradient magnitude at each pixel using gradient magnitude filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applySigmoidFilter](../Classes/classshapeworks_1_1Image.md#function-applysigmoidfilter)**(double alpha =10.0, double beta =10.0)<br>computes sigmoid function pixel-wise using sigmoid image filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyTPLevelSetFilter](../Classes/classshapeworks_1_1Image.md#function-applytplevelsetfilter)**(const [Image](../Classes/classshapeworks_1_1Image.md) & featureImage, double scaling =20.0)<br>segements structures in images using topology preserving geodesic active contour level set filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[topologyPreservingSmooth](../Classes/classshapeworks_1_1Image.md#function-topologypreservingsmooth)**(float scaling =20.0, float sigmoidAlpha =10.5, float sigmoidBeta =10.0)<br>creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter]  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[applyIntensityFilter](../Classes/classshapeworks_1_1Image.md#function-applyintensityfilter)**(double minVal, double maxVal)<br>applies intensity windowing image filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[gaussianBlur](../Classes/classshapeworks_1_1Image.md#function-gaussianblur)**(double sigma =0.0)<br>applies gaussian blur with given sigma  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[crop](../Classes/classshapeworks_1_1Image.md#function-crop)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region, const int padding =0)<br>crops the image down to the given region, with optional padding added  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[clip](../Classes/classshapeworks_1_1Image.md#function-clip)**(const Plane plane, const PixelType val =0.0)<br>clips an image using a cutting plane  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[reflect](../Classes/classshapeworks_1_1Image.md#function-reflect)**(const Axis & axis)<br>reflect image around the plane specified by the logical center and the given normal (ex: <1,0,0> reflects across YZ-plane).  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[setOrigin](../Classes/classshapeworks_1_1Image.md#function-setorigin)**(Point3 origin =Point3({0, 0, 0}))<br>sets the image origin in physical space to the given value  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[setSpacing](../Classes/classshapeworks_1_1Image.md#function-setspacing)**(Vector3 spacing)<br>sets the image spacing to the given value  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[setCoordsys](../Classes/classshapeworks_1_1Image.md#function-setcoordsys)**(ImageType::DirectionType coordsys)<br>sets the coordinate system in which this image lives in physical space  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[isolate](../Classes/classshapeworks_1_1Image.md#function-isolate)**()<br>isolate the largest object in a binary segmentation  |
| Dims | **[dims](../Classes/classshapeworks_1_1Image.md#function-dims)**() const<br>logical dimensions of the image  |
| Point3 | **[size](../Classes/classshapeworks_1_1Image.md#function-size)**() const<br>physical dimensions of the image (dims * spacing)  |
| Vector | **[spacing](../Classes/classshapeworks_1_1Image.md#function-spacing)**() const<br>physical spacing of the image  |
| Point3 | **[origin](../Classes/classshapeworks_1_1Image.md#function-origin)**() const<br>physical coordinates of image origin  |
| Point3 | **[center](../Classes/classshapeworks_1_1Image.md#function-center)**() const<br>physical coordinates of center of this image  |
| ImageType::DirectionType | **[coordsys](../Classes/classshapeworks_1_1Image.md#function-coordsys)**() const<br>return coordinate system in which this image lives in physical space  |
| Point3 | **[centerOfMass](../Classes/classshapeworks_1_1Image.md#function-centerofmass)**(PixelType minVal =0.0, PixelType maxVal =1.0) const<br>returns average physical coordinate of pixels in range (minval, maxval]  |
| PixelType | **[min](../Classes/classshapeworks_1_1Image.md#function-min)**()<br>minimum of image  |
| PixelType | **[max](../Classes/classshapeworks_1_1Image.md#function-max)**()<br>maximum of image  |
| PixelType | **[mean](../Classes/classshapeworks_1_1Image.md#function-mean)**()<br>mean of image  |
| PixelType | **[std](../Classes/classshapeworks_1_1Image.md#function-std)**()<br>standard deviation of image  |
| [IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) | **[logicalBoundingBox](../Classes/classshapeworks_1_1Image.md#function-logicalboundingbox)**() const<br>bounding box of complete image in logical (index) space  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[physicalBoundingBox](../Classes/classshapeworks_1_1Image.md#function-physicalboundingbox)**() const<br>bounding box of complete image in physical space  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[physicalBoundingBox](../Classes/classshapeworks_1_1Image.md#function-physicalboundingbox)**(PixelType isovalue) const<br>bounding box of largest region of data >= the given isoValue in physical space  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[logicalToPhysical](../Classes/classshapeworks_1_1Image.md#function-logicaltophysical)**([IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) region) const<br>converts a bounding box in logical (index) space to this image's index coordinates  |
| [IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) | **[physicalToLogical](../Classes/classshapeworks_1_1Image.md#function-physicaltological)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region) const<br>converts a bounding box in physical space to this image's logical (index) coordinates  |
| Point3 | **[logicalToPhysical](../Classes/classshapeworks_1_1Image.md#function-logicaltophysical)**(const Coord & c) const<br>converts from pixel coordinates to physical space  |
| Coord | **[physicalToLogical](../Classes/classshapeworks_1_1Image.md#function-physicaltological)**(const Point3 & p) const<br>converts from a physical coordinate to a logical coordinate  |
| bool | **[compare](../Classes/classshapeworks_1_1Image.md#function-compare)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other, bool verifyall =true, double tolerance =0.0, double precision =1e-12) const<br>compares this with another image using the region of interest filter  |
| bool | **[operator==](../Classes/classshapeworks_1_1Image.md#function-operator==)**(const [Image](../Classes/classshapeworks_1_1Image.md) & other) const<br>compares this with another image using the region of interest filter  |
| [Image](../Classes/classshapeworks_1_1Image.md) & | **[write](../Classes/classshapeworks_1_1Image.md#function-write)**(const std::string & filename, bool compressed =true)<br>writes image, format specified by filename extension  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[toMesh](../Classes/classshapeworks_1_1Image.md#function-tomesh)**(PixelType isovalue) const<br>converts image to mesh  |

## Friends

|                | Name           |
| -------------- | -------------- |
| struct | **[SharedCommandData](../Classes/classshapeworks_1_1Image.md#friend-sharedcommanddata)**  |

## Detailed Description

```cpp
class shapeworks::Image;
```


This class represents a 3D image volume and operations that can be performed on images. 

## Public Types Documentation

### enum InterpolationType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Linear | |   |
| NearestNeighbor | |   |




### using PixelType

```cpp
using shapeworks::Image::PixelType =  float;
```


### using ImageType

```cpp
using shapeworks::Image::ImageType =  itk::Image<PixelType, 3>;
```


### using StatsPtr

```cpp
using shapeworks::Image::StatsPtr =  itk::StatisticsImageFilter<ImageType>::Pointer;
```


## Public Functions Documentation

### function Image

```cpp
inline Image(
    const std::string & pathname
)
```


### function Image

```cpp
inline Image(
    ImageType::Pointer imagePtr
)
```


### function Image

```cpp
Image(
    const vtkSmartPointer< vtkImageData > vtkImage
)
```


### function Image

```cpp
inline Image(
    Image && img
)
```


### function Image

```cpp
inline Image(
    const Image & img
)
```


### function operator=

```cpp
Image & operator=(
    const Image & img
)
```


### function operator=

```cpp
Image & operator=(
    Image && img
)
```

lvalue assignment operator 

### function operator ImageType::Pointer

```cpp
inline operator ImageType::Pointer()
```

rvalue assignment operator 

return this as an ITK image 


### function getITKImage

```cpp
inline ImageType::Pointer getITKImage() const
```


### function getVTKImage

```cpp
vtkSmartPointer< vtkImageData > getVTKImage() const
```

creates a VTK filter for the given image 

### function operator-

```cpp
Image & operator-()
```

negation operator 

### function operator+

```cpp
Image operator+(
    const Image & other
) const
```

plus operator to add two images 

### function operator+=

```cpp
Image & operator+=(
    const Image & other
)
```


### function operator-

```cpp
Image operator-(
    const Image & other
) const
```

minus operator to add two images 

### function operator-=

```cpp
Image & operator-=(
    const Image & other
)
```


### function operator*

```cpp
Image operator*(
    const PixelType x
) const
```

multiply operator to scale contents of an image 

### function operator*=

```cpp
Image & operator*=(
    const PixelType x
)
```


### function operator/

```cpp
Image operator/(
    const PixelType x
) const
```

divide operator to scale contents of an image 

### function operator/=

```cpp
Image & operator/=(
    const PixelType x
)
```


### function operator+

```cpp
Image operator+(
    const PixelType x
) const
```

plus operator to shift contents of an image 

### function operator+=

```cpp
Image & operator+=(
    const PixelType x
)
```


### function operator-

```cpp
Image operator-(
    const PixelType x
) const
```

minus operator to shift contents of an image 

### function operator-=

```cpp
Image & operator-=(
    const PixelType x
)
```


### function antialias

```cpp
Image & antialias(
    unsigned iterations =50,
    double maxRMSErr =0.01f,
    int layers =3
)
```

antialiases image 

### function recenter

```cpp
Image & recenter()
```

helper identical to setOrigin(image.center()) changing origin (in the image header) to physcial center of the image 

### function resample

```cpp
Image & resample(
    const TransformPtr transform,
    const Point3 origin,
    const Dims dims,
    const Vector3 spacing,
    const ImageType::DirectionType direction,
    InterpolationType interp =NearestNeighbor
)
```

resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator 

### function resample

```cpp
Image & resample(
    const Vector & physicalSpacing,
    InterpolationType interp =Linear
)
```

resamples image using new physical spacing, updating logical dims to keep all image data for this spacing 

### function resample

```cpp
Image & resample(
    double isoSpacing =1.0,
    InterpolationType interp =Linear
)
```

resamples image using isotropic physical spacing 

### function resize

```cpp
Image & resize(
    Dims logicalDims,
    InterpolationType interp =Linear
)
```

changes logical image size, computing new physical spacing based on this size (i.e., physical image size remains the same) 

### function pad

```cpp
Image & pad(
    int padding,
    PixelType value =0.0
)
```

pads an image by same number of voxels in all directions with constant value 

### function pad

```cpp
Image & pad(
    int padx,
    int pady,
    int padz,
    PixelType value =0.0
)
```

pads an image by desired number of voxels in each direction with constant value 

### function pad

```cpp
Image & pad(
    IndexRegion & region,
    PixelType value =0.0
)
```

pads an image to include the given region with constant value 

### function translate

```cpp
Image & translate(
    const Vector3 & v
)
```

helper to simply translate image 

### function scale

```cpp
Image & scale(
    const Vector3 & v
)
```

helper to simply scale image around center (not origin) 

### function rotate

```cpp
Image & rotate(
    const double angle,
    const Vector3 & axis
)
```

helper to simply rotate around axis through center (not origin) by given angle (in radians) 

### function rotate

```cpp
Image & rotate(
    const double angle,
    Axis axis
)
```

helper to simply rotate around axis through center (not origin) by given angle (in radians) 

### function createCenterOfMassTransform

```cpp
TransformPtr createCenterOfMassTransform()
```

creates a transform that translates center of mass to center of image 

### function createRigidRegistrationTransform

```cpp
TransformPtr createRigidRegistrationTransform(
    const Image & target_dt,
    float isoValue =0.0,
    unsigned iterations =20
)
```

creates transform to target image using iterative closest point (ICP) registration; images MUST be distance transforms; isovalue is used to create meshes from these distance transform images, which are then passed to ICP for the given number of iterations 

### function applyTransform

```cpp
Image & applyTransform(
    const TransformPtr transform,
    InterpolationType interp =Linear
)
```

applies the given transformation to the image by using resampling filter 

### function applyTransform

```cpp
Image & applyTransform(
    const TransformPtr transform,
    const Point3 origin,
    const Dims dims,
    const Vector3 spacing,
    const ImageType::DirectionType direction,
    InterpolationType interp =NearestNeighbor
)
```

applies the given transformation to the image by using resampling filter with new origin, dims, spacing and direction values 

### function extractLabel

```cpp
Image & extractLabel(
    const PixelType label =1.0
)
```

extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image 

### function closeHoles

```cpp
Image & closeHoles(
    const PixelType foreground =0.0
)
```

closes holes in a given volume, default foreground value assumes a binary volume 

### function binarize

```cpp
Image & binarize(
    PixelType minVal =0.0,
    PixelType maxVal =std::numeric_limits< PixelType >::max(),
    PixelType innerVal =1.0,
    PixelType outerVal =0.0
)
```

threholds image into binary label based on upper and lower intensity bounds given by user 

### function computeDT

```cpp
Image & computeDT(
    PixelType isoValue =0.0
)
```

computes distance transform volume from a (preferably antialiased) binary image using the specified isovalue 

### function applyCurvatureFilter

```cpp
Image & applyCurvatureFilter(
    unsigned iterations =10
)
```

denoises an image using curvature driven flow using curvature flow image filter 

### function applyGradientFilter

```cpp
Image & applyGradientFilter()
```

computes gradient magnitude at each pixel using gradient magnitude filter 

### function applySigmoidFilter

```cpp
Image & applySigmoidFilter(
    double alpha =10.0,
    double beta =10.0
)
```

computes sigmoid function pixel-wise using sigmoid image filter 

### function applyTPLevelSetFilter

```cpp
Image & applyTPLevelSetFilter(
    const Image & featureImage,
    double scaling =20.0
)
```

segements structures in images using topology preserving geodesic active contour level set filter 

### function topologyPreservingSmooth

```cpp
Image & topologyPreservingSmooth(
    float scaling =20.0,
    float sigmoidAlpha =10.5,
    float sigmoidBeta =10.0
)
```

creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter] 

### function applyIntensityFilter

```cpp
Image & applyIntensityFilter(
    double minVal,
    double maxVal
)
```

applies intensity windowing image filter 

### function gaussianBlur

```cpp
Image & gaussianBlur(
    double sigma =0.0
)
```

applies gaussian blur with given sigma 

### function crop

```cpp
Image & crop(
    PhysicalRegion region,
    const int padding =0
)
```

crops the image down to the given region, with optional padding added 

### function clip

```cpp
Image & clip(
    const Plane plane,
    const PixelType val =0.0
)
```

clips an image using a cutting plane 

### function reflect

```cpp
Image & reflect(
    const Axis & axis
)
```

reflect image around the plane specified by the logical center and the given normal (ex: <1,0,0> reflects across YZ-plane). 

### function setOrigin

```cpp
Image & setOrigin(
    Point3 origin =Point3({0, 0, 0})
)
```

sets the image origin in physical space to the given value 

### function setSpacing

```cpp
Image & setSpacing(
    Vector3 spacing
)
```

sets the image spacing to the given value 

### function setCoordsys

```cpp
Image & setCoordsys(
    ImageType::DirectionType coordsys
)
```

sets the coordinate system in which this image lives in physical space 

### function isolate

```cpp
Image & isolate()
```

isolate the largest object in a binary segmentation 

### function dims

```cpp
inline Dims dims() const
```

logical dimensions of the image 

### function size

```cpp
inline Point3 size() const
```

physical dimensions of the image (dims * spacing) 

### function spacing

```cpp
inline Vector spacing() const
```

physical spacing of the image 

### function origin

```cpp
inline Point3 origin() const
```

physical coordinates of image origin 

### function center

```cpp
inline Point3 center() const
```

physical coordinates of center of this image 

### function coordsys

```cpp
inline ImageType::DirectionType coordsys() const
```

return coordinate system in which this image lives in physical space 

### function centerOfMass

```cpp
Point3 centerOfMass(
    PixelType minVal =0.0,
    PixelType maxVal =1.0
) const
```

returns average physical coordinate of pixels in range (minval, maxval] 

### function min

```cpp
PixelType min()
```

minimum of image 

### function max

```cpp
PixelType max()
```

maximum of image 

### function mean

```cpp
PixelType mean()
```

mean of image 

### function std

```cpp
PixelType std()
```

standard deviation of image 

### function logicalBoundingBox

```cpp
IndexRegion logicalBoundingBox() const
```

bounding box of complete image in logical (index) space 

### function physicalBoundingBox

```cpp
PhysicalRegion physicalBoundingBox() const
```

bounding box of complete image in physical space 

### function physicalBoundingBox

```cpp
PhysicalRegion physicalBoundingBox(
    PixelType isovalue
) const
```

bounding box of largest region of data >= the given isoValue in physical space 

### function logicalToPhysical

```cpp
PhysicalRegion logicalToPhysical(
    IndexRegion region
) const
```

converts a bounding box in logical (index) space to this image's index coordinates 

### function physicalToLogical

```cpp
IndexRegion physicalToLogical(
    PhysicalRegion region
) const
```

converts a bounding box in physical space to this image's logical (index) coordinates 

### function logicalToPhysical

```cpp
Point3 logicalToPhysical(
    const Coord & c
) const
```

converts from pixel coordinates to physical space 

### function physicalToLogical

```cpp
Coord physicalToLogical(
    const Point3 & p
) const
```

converts from a physical coordinate to a logical coordinate 

### function compare

```cpp
bool compare(
    const Image & other,
    bool verifyall =true,
    double tolerance =0.0,
    double precision =1e-12
) const
```

compares this with another image using the region of interest filter 

### function operator==

```cpp
inline bool operator==(
    const Image & other
) const
```

compares this with another image using the region of interest filter 

### function write

```cpp
Image & write(
    const std::string & filename,
    bool compressed =true
)
```

writes image, format specified by filename extension 

### function toMesh

```cpp
Mesh toMesh(
    PixelType isovalue
) const
```

converts image to mesh 

## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData();
```


-------------------------------

Updated on 2022-01-12 at 19:06:14 +0000