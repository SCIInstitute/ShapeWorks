#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"
#include <limits>
#include <itkTranslationTransform.h>
#include "itkThinPlateSplineKernelTransform.h"
// Software Guide : EndCodeSnippet

#include "itkPointSet.h"

namespace shapeworks {

class Image
{
public:
  static const unsigned dims = 3;
  using PixelType = float;
  using ImageType = itk::Image<PixelType, dims>;
  typedef double CoordinateRepType;
  typedef itk::ThinPlateSplineKernelTransform< CoordinateRepType, dims> TransformType;
  using PointType = itk::Point< CoordinateRepType, dims>;
  typedef std::vector< PointType > PointArrayType;
  typedef TransformType::PointSetType PointSetType;
  typedef PointSetType::Pointer  PointSetPointer;
  typedef PointSetType::PointIdentifier PointIdType;

  Image() {}
  Image(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &inFilename);
  bool write(const std::string &outFilename, bool useCompression = true);
  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = dims);
  bool binarize(PixelType threshold = std::numeric_limits<PixelType>::epsilon(),
                PixelType inside = itk::NumericTraits<PixelType>::One,
                PixelType outside = itk::NumericTraits<PixelType>::Zero);
  bool recenter();
  bool isoresample(double isoSpacing = 1.0f, Dims outputSize = Dims());
  bool pad(int padding, PixelType value);
  // bool nextfunction(...);
  bool compare_equal(const Image &other);
  bool warp(std::string &source_file, std::string &target_file, int pointFactor);

private:

  static bool is_directory(const std::string &pathname);

  bool read_image_dir(const std::string &pathname);

  ImageType::Pointer image;
};
} // shapeworks
