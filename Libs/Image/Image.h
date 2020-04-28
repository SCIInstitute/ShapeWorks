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

  Image& write(const std::string &filename, bool compressed = true);

  Image& antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = 3);
  Image& recenter();
  Image& resample(const Point3 &spacing, Dims outputSize = Dims()); // Resamples image with new voxel spacing and output size [same size if unspecified]
  Image& pad(int padding = 0, PixelType value = 0.0);
  Image& applyTransform(const Transform &transform);
  Image& extractLabel(PixelType label = 1.0);
  Image& closeHoles();
  Image& threshold(PixelType min = std::numeric_limits<PixelType>::epsilon(), PixelType max = std::numeric_limits<PixelType>::max());
  Image& computeDT(float isoValue = 0.0);
  Image& applyCurvatureFilter(unsigned iterations = 10);
  Image& applyGradientFilter();
  Image& applySigmoidFilter(double alpha = 10.0, double beta = 10.0);
  Image& applyTPLevelSetFilter(const Image &featureImage, double scaling = 20.0);
  Image& gaussianBlur(double sigma = 0.0);
  Region binaryBoundingBox(std::vector<std::string> &filenames, int padding = 0);
  Image& crop(const Region &region);

  Point3 logicalToPhysical(const IPoint3 &v) const;
  IPoint3 physicalToLogical(const Point3 &p) const;

  bool operator==(const Image &other) const;

  /// returns average spatial coordinate of pixels in range (minval, maxval]
  Point3 centerOfMass(PixelType minval = 0.0, PixelType maxval = 1.0) const;  

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
