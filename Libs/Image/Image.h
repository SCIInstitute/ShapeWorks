#pragma once

#include "Shapeworks.h"
#include "Region.h"

#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <itkStatisticsImageFilter.h>

#include <limits>

namespace shapeworks {

class Mesh;

/**
 * \class Image
 * \ingroup Group-Image
 *
 * This class represents a 3D image volume and operations that can be performed on images.
 *
 */
class Image
{
public:
  enum InterpolationType { Linear, NearestNeighbor };

  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;
  using StatsPtr = itk::StatisticsImageFilter<ImageType>::Pointer;

  // constructors and assignment operators //
  Image(const std::string &pathname) : image(read(pathname)) {}
  Image(ImageType::Pointer imagePtr) : image(imagePtr) { if (!image) throw std::invalid_argument("null imagePtr"); }
  Image(const vtkSmartPointer<vtkImageData> vtkImage);
  Image(Image&& img) : image(nullptr) { this->image.Swap(img.image); }
  Image(const Image& img) : image(cloneData(img.image)) {}
  Image& operator=(const Image& img); /// lvalue assignment operator
  Image& operator=(Image&& img);      /// rvalue assignment operator

  /// return this as an ITK image
  operator ImageType::Pointer() { return image; }
  ImageType::Pointer getITKImage() const { return image; }

  /// creates a VTK filter for the given image
  vtkSmartPointer<vtkImageData> getVTKImage() const;
  
  // modification functions //

  /// negation operator
  Image& operator-();

  /// plus operator to add two images
  Image operator+(const Image& other) const;
  Image& operator+=(const Image& other);

  /// minus operator to add two images
  Image operator-(const Image& other) const;
  Image& operator-=(const Image& other);

  /// multiply operator to scale contents of an image
  Image operator*(const PixelType x) const;
  Image& operator*=(const PixelType x);

  /// divide operator to scale contents of an image
  Image operator/(const PixelType x) const;
  Image& operator/=(const PixelType x);

  /// plus operator to shift contents of an image
  Image operator+(const PixelType x) const;
  Image& operator+=(const PixelType x);

  /// minus operator to shift contents of an image
  Image operator-(const PixelType x) const;
  Image& operator-=(const PixelType x);

  /// antialiases image
  Image& antialias(unsigned iterations = 50, double maxRMSErr = 0.01f, int layers = 3);
  
  /// helper identical to setOrigin(image.center()) changing origin (in the image header) to physcial center of the image
  Image& recenter();

  /// resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator
  Image& resample(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing, const ImageType::DirectionType direction, InterpolationType interp = NearestNeighbor);

  /// resamples image using new physical spacing, updating logical dims to keep all image data for this spacing
  Image& resample(const Vector& physicalSpacing, InterpolationType interp = Linear);
  
  /// resamples image using isotropic physical spacing
  Image& resample(double isoSpacing = 1.0, InterpolationType interp = Linear);

  /// changes logical image size, computing new physical spacing based on this size (i.e., physical image size remains the same)
  Image& resize(Dims logicalDims, InterpolationType interp = Linear);

  /// pads an image by same number of voxels in all directions with constant value
  Image& pad(int padding, PixelType value = 0.0);

  /// pads an image by desired number of voxels in each direction with constant value
  Image& pad(int padx, int pady, int padz, PixelType value = 0.0);

  /// pads an image to include the given region with constant value
  Image& pad(IndexRegion &region, PixelType value = 0.0);

  /// helper to simply translate image
  Image& translate(const Vector3 &v);

  /// helper to simply scale image around center (not origin)
  Image& scale(const Vector3 &v);

  /// helper to simply rotate around axis through center (not origin) by given angle (in radians)
  Image& rotate(const double angle, const Vector3 &axis);

  /// helper to simply rotate around axis through center (not origin) by given angle (in radians)
  Image& rotate(const double angle, Axis axis);

  /// creates a transform that translates center of mass to center of image
  TransformPtr createCenterOfMassTransform();

  /// creates transform to target image using iterative closest point (ICP) registration; images MUST be distance transforms; isovalue is used to create meshes from these distance transform images, which are then passed to ICP for the given number of iterations
  TransformPtr createRigidRegistrationTransform(const Image &target_dt, float isoValue = 0.0, unsigned iterations = 20);

  /// applies the given transformation to the image by using resampling filter
  Image& applyTransform(const TransformPtr transform, InterpolationType interp = Linear);

  /// applies the given transformation to the image by using resampling filter with new origin, dims, spacing and direction values
  Image& applyTransform(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing, const ImageType::DirectionType direction, InterpolationType interp = NearestNeighbor);

  /// extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image
  Image& extractLabel(const PixelType label = 1.0);

  /// closes holes in a given volume, default foreground value assumes a binary volume
  Image& closeHoles(const PixelType foreground = 0.0);
  
  /// threholds image into binary label based on upper and lower intensity bounds given by user
  Image& binarize(PixelType minVal = 0.0, PixelType maxVal = std::numeric_limits<PixelType>::max(), PixelType innerVal = 1.0, PixelType outerVal = 0.0);

  /// computes distance transform volume from a (preferably antialiased) binary image using the specified isovalue
  Image& computeDT(PixelType isoValue = 0.0);

  /// denoises an image using curvature driven flow using curvature flow image filter
  Image& applyCurvatureFilter(unsigned iterations = 10);

  /// computes gradient magnitude at each pixel using gradient magnitude filter
  Image& applyGradientFilter();

  /// computes sigmoid function pixel-wise using sigmoid image filter
  Image& applySigmoidFilter(double alpha = 10.0, double beta = 10.0);

  /// segements structures in images using topology preserving geodesic active contour level set filter
  Image& applyTPLevelSetFilter(const Image& featureImage, double scaling = 20.0);

  /// creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter]
  Image& topologyPreservingSmooth(float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0);

  /// applies intensity windowing image filter
  Image& applyIntensityFilter(double minVal, double maxVal);

  /// applies gaussian blur with given sigma
  Image& gaussianBlur(double sigma = 0.0);

  /// crops the image down to the given region, with optional padding added
  Image& crop(PhysicalRegion region, const int padding = 0);

  /// clips an image using a cutting plane
  Image& clip(const Plane plane, const PixelType val = 0.0);

  /// reflect image around the plane specified by the logical center and the given normal (ex: <1,0,0> reflects across YZ-plane).
  Image& reflect(const Axis& axis);

  /// sets the image origin in physical space to the given value
  Image& setOrigin(Point3 origin = Point3({0, 0, 0}));

  /// sets the image spacing to the given value
  Image& setSpacing(Vector3 spacing);

  /// sets the coordinate system in which this image lives in physical space
  Image& setCoordsys(ImageType::DirectionType coordsys);

  // query functions //

  /// logical dimensions of the image
  Dims dims() const { return image->GetLargestPossibleRegion().GetSize(); }

  /// physical dimensions of the image (dims * spacing)
  Point3 size() const { return toPoint(spacing()) * toPoint(dims()); }

  /// physical spacing of the image
  Vector spacing() const { return image->GetSpacing(); }

  /// physical coordinates of image origin
  Point3 origin() const { return image->GetOrigin(); }

  /// physical coordinates of center of this image
  Point3 center() const { return origin() + size() / 2.0; }

  /// return coordinate system in which this image lives in physical space
  ImageType::DirectionType coordsys() const { return image->GetDirection(); };

  /// returns average physical coordinate of pixels in range (minval, maxval]
  Point3 centerOfMass(PixelType minVal = 0.0, PixelType maxVal = 1.0) const;

  /// minimum of image
  PixelType min();

  /// maximum of image
  PixelType max();

  /// mean of image
  PixelType mean();

  /// standard deviation of image
  PixelType std();

  /// bounding box of complete image in logical (index) space
  IndexRegion logicalBoundingBox() const;

  /// bounding box of complete image in physical space
  PhysicalRegion physicalBoundingBox() const;

  /// bounding box of largest region of data >= the given isoValue in physical space
  PhysicalRegion physicalBoundingBox(PixelType isovalue) const;

  /// converts a bounding box in logical (index) space to this image's index coordinates
  PhysicalRegion logicalToPhysical(IndexRegion region) const;

  /// converts a bounding box in physical space to this image's logical (index) coordinates
  IndexRegion physicalToLogical(PhysicalRegion region) const;

  /// converts from pixel coordinates to physical space
  Point3 logicalToPhysical(const Coord &c) const;

  /// converts from a physical coordinate to a logical coordinate
  Coord physicalToLogical(const Point3 &p) const;

  /// compares this with another image using the region of interest filter
  bool compare(const Image& other, bool verifyall = true, double tolerance = 0.0, double precision = 1e-12) const;

  /// compares this with another image using the region of interest filter
  bool operator==(const Image& other) const { return compare(other); }

  // export functions //

  /// writes image, format specified by filename extension
  Image& write(const std::string &filename, bool compressed = true);

  /// converts image to mesh
  Mesh toMesh(PixelType isovalue) const;

private:
  friend struct SharedCommandData;
  Image() : image(nullptr) {} // only for use by SharedCommandData since an Image should always be valid, never "empty"

  /// reads image (used only by constructor)
  static ImageType::Pointer read(const std::string &filename);
  static ImageType::Pointer readDICOMImage(const std::string &pathname);

  /// clones the underlying ImageType (ITK) data
  static ImageType::Pointer cloneData(const ImageType::Pointer img);

  /// creates a vtkPolyData for the given image
  static vtkSmartPointer<vtkPolyData> getPolyData(const Image& image, PixelType isoValue = 0.0);

  /// pad image by the given dims (always positive) in each direction
  Image& pad(Dims lowerExtendRegion, Dims upperExtendRegion, PixelType value = 0.0);

  StatsPtr statsFilter();

  ImageType::Pointer image;
};

/// stream insertion operators for Image
std::ostream& operator<<(std::ostream &os, const Image& img);

/// override templates defined in Shapeworks.h
template<>
Image operator*(const Image& img, const double x);
template<>
Image operator/(const Image& img, const double x);
template<>
Image& operator*=(Image& img, const double x);
template<>
Image& operator/=(Image& img, const double x);

} // shapeworks
