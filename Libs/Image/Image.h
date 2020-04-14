#pragma once

#include "Shapeworks.h"
#include "Transform.h"
#include <limits>
#include <itkImage.h>

namespace shapeworks {

class Image
{
public:
  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;

  struct Region
  {
    int min[3] = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    int max[3] = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
    bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

    IPoint3 origin() const { return IPoint3(min); }
    IPoint3 size() const { return IPoint3({max[0]-min[0], max[1]-min[1], max[2]-min[2]}); }

    operator ImageType::RegionType() const { return ImageType::RegionType(toIndex(origin()), toSize(size())); }
  };

  Image(const std::string &pathname) { *this = read(pathname); }
  Image(ImageType::Pointer imagePtr) : image(imagePtr) { if (!image) throw std::invalid_argument("null imagePtr"); }
  Image(Image &&img) { this->image.Swap(img.image); }
  Image(const Image &img);
  Image& operator=(Image &img) { this->image.Swap(img.image); return *this; }
  Image& operator=(const Image &img);

  bool write(const std::string &filename, bool compressed = true);

  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = 3); //todo: no need for a return value
  bool recenter();

  /// Resamples image with new voxel spacing and output size [same size if unspecified]
  Image& resample(const Point3& spacing, Dims outputSize = Dims());

  bool pad(int padding = 0, PixelType value = 0.0);
  bool applyTransform(const Transform &transform);
  bool extractLabel(PixelType label = 1.0);
  bool closeHoles();
  bool threshold(PixelType min = std::numeric_limits<PixelType>::epsilon(), PixelType max = std::numeric_limits<PixelType>::max());
  bool computeDT(float isoValue = 0.0);
  bool applyCurvatureFilter(unsigned iterations = 10);
  bool applyGradientFilter();
  bool applySigmoidFilter(double alpha = 10.0, double beta = 10.0);
  bool applyTPLevelSetFilter(const Image &featureImage, double scaling = 20.0);
  bool gaussianBlur(double sigma = 0.0);
  Region binaryBoundingBox(std::vector<std::string> &filenames, int padding = 0);
  bool crop(const Region &region);

  Point3 logicalToPhysical(const IPoint3 &v) const;
  IPoint3 physicalToLogical(const Point3 &p) const;

  bool operator==(const Image &other) const;

  Point3 centerOfMass() const;  
  Point3 origin() const { return image->GetOrigin(); }
  Dims dims() const { return image->GetLargestPossibleRegion().GetSize(); }
  Point3 size() const;                                      // spatial size of image
  Point3 center() const { return origin() + size() / 2.0; } // spatial coordinates of center of this image

private:
  friend struct SharedCommandData;
  Image() {} // only for use by SharedCommandData since an Image should always be valid, never "empty"
  static Image read(const std::string &filename);
  static Image readDICOMImage(const std::string &pathname);
  
  friend std::ostream& operator<<(std::ostream &os, const Transform &t);

  ImageType::Pointer image;
};

std::ostream& operator<<(std::ostream &os, const Image &img);
std::ostream& operator<<(std::ostream &os, const Image::Region &region);

} // shapeworks
