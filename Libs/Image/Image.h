#pragma once

#include "Shapeworks.h"
#include <limits>
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace shapeworks {

class Image
{
public:
  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;

  struct Region
  {
    Coord min = {0,0,0};
    Coord max = {0,0,0};
    Region(const Dims &dims) : min({0, 0, 0}), max({static_cast<long>(dims[0]), static_cast<long>(dims[1]), static_cast<long>(dims[2])}) {}
    Region() = default;

    /// verified min/max do not create an inverted or an empty region
    bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

    Coord origin() const { return min; }
    Dims size() const {
      return Dims({static_cast<unsigned long>(max[0]-min[0]),
                   static_cast<unsigned long>(max[1]-min[1]),
                   static_cast<unsigned long>(max[2]-min[2])});
    }

    /// grows or shrinks the region by the specified amount
    void pad(int padding) {
      for (auto i=0; i<3; i++) {
        min[i] += padding;
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
  Image(Image &&img) : image(nullptr) { this->image.Swap(img.image); }
  Image(const Image &img) : image(cloneData(img.image)) {}
  Image& operator=(const Image &img); /// lvalue assignment operator
  Image& operator=(Image &&img);      /// rvalue assignment operator

  // modification functions //

  /// antialiases image
  Image &antialias(unsigned numIterations = 50, double maxRMSErr = 0.01f, unsigned numLayers = 3);
  
  /// recenters by changing origin (in the image header) to the physcial coordinates of the center of the image
  Image &recenter();

  /// Resamples image with new voxel spacing and output size [same size if unspecified]
  Image &resample(const Point3 &spacing, Dims outputSize = Dims());

  /// pads an image by desired number of voxels in each direction with constant value
  Image &pad(int padding = 0, PixelType value = 0.0);

  /// computes translation
  Transform translate(Vector3 v);

  /// computes scaling
  Transform scale(Vector3 v);

  /// computes rotation
  Transform rotate(Vector3 v, double angle);

  /// applies the computed transformation to the image by using resampling filter
  Image &applyTransform(const Transform &transform);

  /// extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image
  Image &extractLabel(PixelType label = 1.0);

  /// closes holes in a given binary volume
  Image &closeHoles();
  
  /// threholds image into binary label based on upper and lower intensity bounds given by user
  Image &threshold(PixelType minval = std::numeric_limits<PixelType>::epsilon(), PixelType maxval = std::numeric_limits<PixelType>::max());

  /// computes distance transform volume from an antialiased binary image using the specified isovalue
  Image &computeDT(PixelType isoValue = 0.0);

  /// applies curvature flow image filter
  Image &applyCurvatureFilter(unsigned iterations = 10);

  /// applies gradient magnitude image filter
  Image &applyGradientFilter();

  /// applies sigmoid image filter with given alpha and beta, whatever that means
  Image &applySigmoidFilter(double alpha = 10.0, double beta = 10.0);

  /// applies TPLevelSet level set image filter using given 
  Image &applyTPLevelSetFilter(const Image &featureImage, double scaling = 20.0);

  /// applies gaussian blur with given sigma
  Image &gaussianBlur(double sigma = 0.0);

  /// crops the image down to the given region
  Image &crop(const Region &region);

  /// sets values on the back side of the cutting plane to val (default 0.0)
  Image &clip(Matrix cuttingPlane, const PixelType val = 0.0);

  /// sets the iamge origin in physical space to the given value
  Image &setOrigin(Point3 origin = Point3({0, 0, 0}));

  /// Reflect image on the plane specified by center of image and given normal (ex: <1,0,0> to reflect "vertically" across YZ-plane).
  Image& reflect(const Vector3 &normal);

  // query functions //

  /// logical dimensions of the image
  Dims dims() const { return image->GetLargestPossibleRegion().GetSize(); }

  /// physical dimensions of the image
  Point3 size() const;

  /// physical coordinates of image origin
  Point3 origin() const { return image->GetOrigin(); }

  /// physical coordinates of center of this image
  Point3 center() const { return origin() + size() / 2.0; }

  /// returns average physical coordinate of pixels in range (minval, maxval]
  Point3 centerOfMass(PixelType minval = 0.0, PixelType maxval = 1.0) const;  

  /// computes the logical coordinates of the largest region of data <= the given isoValue
  Image::Region boundingBox(PixelType isoValue = 1.0) const;

  // converts from pixel coordinates to physical space
  Point3 logicalToPhysical(const IPoint3 &v) const;

  // converts from a physical coordinate to a logical coordinate
  IPoint3 physicalToLogical(const Point3 &p) const;

  // compares this with another image using the region of interest filter
  bool operator==(const Image &other) const;

  // export functions //

  /// writes image, format specified by filename extension
  Image &write(const std::string &filename, bool compressed = true);

  /// creates a vtkPolyData from this image
  vtkSmartPointer<vtkPolyData> getPolyData(const Image &img, PixelType isoValue = 0.0) const;

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

std::ostream& operator<<(std::ostream &os, const Image &img);
std::ostream& operator<<(std::ostream &os, const Image::Region &region);

/// connects pipeline from an itk exporter to a vtk importer
template <typename ITK_Exporter, typename VTK_Importer>
void connectPipelines(ITK_Exporter exporter, VTK_Importer *importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

} // shapeworks
