#pragma once

#include <itkImageRegionIterator.h>

namespace Shapeworks {

class ImageUtils
{
public:

  // For image region iteration as the image was not updated even after being set
  template<typename TImage>
  static void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output);

private:

};

// <ctc> I find it hard to believe ITK doesn't have a robust image copy
template<typename TImage>
void ImageUtils::DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
  output->SetRegions(input->GetLargestPossibleRegion());
  output->Allocate();

  itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
  itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());

  while(!inputIterator.IsAtEnd())
  {
    outputIterator.Set(inputIterator.Get());
    ++inputIterator;
    ++outputIterator;
  }
}


} // Shapeworks
