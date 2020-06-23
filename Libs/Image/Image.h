#pragma once

#include "Shapeworks.h"

#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <limits>

namespace shapeworks {

class Image
{
public:
  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;

  /// Logical region of an image (may be negative for relative regions to a given location in an image).
  struct Region
  {
    Coord min = Coord({ 1000000000, 1000000000, 1000000000 });
    Coord max = Coord({-1000000000, -1000000000, -1000000000 });
    Region(const Dims &dims) : min({0, 0, 0}) {
      if (0 != (dims[0] + dims[1] + dims[2])) 
        max = Coord({static_cast<long>(dims[0])-1,
                     static_cast<long>(dims[1])-1,
                     static_cast<long>(dims[2])-1});
    }
    Region(const Coord &_min, const Coord &_max) : min(_min), max(_max) {}
    Region() = default;
    bool operator==(const Region &other) const { return min == other.min && max == other.max; }

    /// verified min/max do not create an inverted or an empty region
    bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

    Coord origin() const { return min; }
    Dims size() const {
      return Dims({static_cast<unsigned long>(max[0]-min[0]),
                   static_cast<unsigned long>(max[1]-min[1]),
                   static_cast<unsigned long>(max[2]-min[2])});
    }

    /// clip region to fit inside image
    Region& clip(const Image& image)
    {
      shrink(Region(image.dims()));
      return *this;
    }
    
    /// grows or shrinks the region by the specified amount
    void pad(int padding) {
      for (auto i=0; i<3; i++) {
        min[i] -= padding;
        max[i] += padding;
      }
    }

    /// shrink this region down to the smallest portions of both
    void shrink(const Region &other) {
      for (auto i=0; i<3; i++) {
        min[i] = std::max(min[i], other.min[i]);
        max[i] = std::min(max[i], other.max[i]);
      }
    }

    /// grow this region up to the largest portions of both
    void grow(const Region &other) {
      for (auto i=0; i<3; i++) {
        min[i] = std::min(min[i], other.min[i]);
        max[i] = std::max(max[i], other.max[i]);
      }
    }

    /// expand this region to include this point
    void expand(const Coord &pt) {
      for (auto i=0; i<3; i++) {
        min[i] = std::min(min[i], pt[i]);
        max[i] = std::max(max[i], pt[i]);
      }
    }

    /// implicit conversion to an itk region
    operator ImageType::RegionType() const { return ImageType::RegionType(origin(), size()); }
  };

  // constructors and assignment operators //
  Image(const std::string &pathname) : image(read(pathname)) {}
  Image(ImageType::Pointer imagePtr) : image(imagePtr) { if (!image) throw std::invalid_argument("null imagePtr"); }
  Image(Image&& img) : image(nullptr) { this->image.Swap(img.image); }
  Image(const Image& img) : image(cloneData(img.image)) {}
  Image& operator=(const Image& img); /// lvalue assignment operator
  Image& operator=(Image&& img);      /// rvalue assignment operator

  // return this as an ITK image
  operator ImageType::Pointer() { return image; }
  
  // modification functions //

  /// negation operator
  Image& operator-();

  /// plus operator to add two images
  Image& operator+(const Image& other);
  Image& operator+=(const Image& other) { return *this = *this + other; }

  /// minus operator to add two images
  Image& operator-(const Image& other);
  Image& operator-=(const Image& other) { return *this = *this - other; }

  /// antialiases image
  Image& antialias(unsigned iterations = 50, double maxRMSErr = 0.01f, int layers = 3);
  
  /// helper identical to setOrigin(image.center()) changing origin (in the image header) to physcial center of the image
  Image& recenter();

  /// Resamples image with new physical spacing and logical size [same size if unspecified]
  Image& resample(const Point3& physicalSpacing, Dims logicalDims);

  /// pads an image in all directions with constant value
  Image& pad(int padding, PixelType value = 0.0);

  /// pads an image by desired number of voxels in each direction with constant value
  Image& pad(int padx, int pady, int padz, PixelType value = 0.0);

  /// helper to simply translate image
  Image& translate(const Vector3 &v);

  /// helper to simply scale image around center (not origin)
  Image& scale(const Vector3 &v);

  /// helper to simply rotate around center (not origin) using axis (default z-axis) by angle (in radians) 
  Image& rotate(const double angle, const Vector3 &axis);

  /// applies the given transformation to the image by using resampling filter
  Image& applyTransform(const TransformPtr transform, const Dims dims, const Point3 origin, const Vector spacing, const ImageType::DirectionType direction);

  /// applies the given transformation to the image by using resampling filter
  Image& applyTransform(const TransformPtr transform);

  /// extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image
  Image& extractLabel(PixelType label = 1.0);

  /// closes holes in a given binary volume
  Image& closeHoles();
  
  /// threholds image into binary label based on upper and lower intensity bounds given by user
  Image& binarize(PixelType minval = 0.0, PixelType maxval = std::numeric_limits<PixelType>::max(), PixelType inner_value = 1.0, PixelType outer_value = 0.0);

  /// computes distance transform volume from a (preferably antialiased) binary image using the specified isovalue
  Image& computeDT(PixelType isoValue = 0.0);

  /// denoises an image using curvature driven flow using curvature flow image filter
  Image& applyCurvatureFilter(unsigned iterations = 10);

  /// computes gradient magnitude of an image region at each pixel using gradient magnitude filter
  Image& applyGradientFilter();

  /// computes sigmoid function pixel-wise using sigmoid image filter
  Image& applySigmoidFilter(double alpha = 10.0, double beta = 10.0);

  /// segemnts structures in images using topology preserving geodesic active contour level set filter
  Image& applyTPLevelSetFilter(const Image& featureImage, double scaling = 20.0);

  /// applies gaussian blur with given sigma
  Image& gaussianBlur(double sigma = 0.0);

  /// crops the image down to the given region
  Image& crop(const Region &region);

  /// sets values on the back side of cutting plane (containing three non-colinear points) to val (default 0.0)
  Image& clip(const Point &o, const Point &p1, const Point &p2, const PixelType val = 0.0);

  /// sets values on the back side of cutting plane (normal n containing point p) to val (default 0.0)
  Image& clip(const Vector &n, const Point &q, const PixelType val = 0.0);

  /// sets the iamge origin in physical space to the given value
  Image& setOrigin(Point3 origin = Point3({0, 0, 0}));

  /// Reflect image around the plane specified by the logical center and the given normal (ex: <1,0,0> reflects across YZ-plane).
  Image& reflect(const Axis& axis);

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
  const ImageType::DirectionType coordsys() const { return image->GetDirection(); };

  /// returns average physical coordinate of pixels in range (minval, maxval]
  Point3 centerOfMass(PixelType minval = 0.0, PixelType maxval = 1.0) const;  

  /// computes the logical coordinates of the largest region of data <= the given isoValue
  Image::Region boundingBox(PixelType isoValue = 1.0) const;

  /// converts from pixel coordinates to physical space
  Point3 logicalToPhysical(const Coord &v) const;

  /// converts from a physical coordinate to a logical coordinate
  Coord physicalToLogical(const Point3 &p) const;

  /// compares this with another image using the region of interest filter
  bool compare(const Image& other, bool verifyall = true, double precision = 1e-12) const;

  /// compares this with another image using the region of interest filter
  bool operator==(const Image& other) const { return compare(other); }

  // export functions //

  /// writes image, format specified by filename extension
  Image& write(const std::string &filename, bool compressed = true);

  /// creates a vtkPolyData for the given image
  static vtkSmartPointer<vtkPolyData> getPolyData(const Image& img, PixelType isoValue = 0.0);

private:
  friend struct SharedCommandData;
  Image() : image(nullptr) {} // only for use by SharedCommandData since an Image should always be valid, never "empty"

  /// reads image (used only by constructor)
  static ImageType::Pointer read(const std::string &filename);
  static ImageType::Pointer readDICOMImage(const std::string &pathname);

  /// clones the underlying ImageType (ITK) data
  static ImageType::Pointer cloneData(const ImageType::Pointer img);

  ImageType::Pointer image;
};

std::ostream& operator<<(std::ostream &os, const Image& img);
std::ostream& operator<<(std::ostream &os, const Image::Region &region);

} // shapeworks
