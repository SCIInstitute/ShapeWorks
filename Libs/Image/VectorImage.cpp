#include "VectorImage.h"

namespace shapeworks {

VectorImage::VectorImage(const Image& dt)
{
  GradientImageFilter::Pointer filter = GradientImageFilter::New();
  filter->SetInput(dt.getITKImage());
  filter->SetUseImageSpacingOn();
  filter->Update();
  this->image = filter->GetOutput();

  GradientInterpolator::Pointer gradientInterpolator = GradientInterpolator::New();
  gradientInterpolator->SetInputImage(this->image);
  this->interpolator = itk::SmartPointer<GradientInterpolator>(gradientInterpolator);
}

VectorImage::ImageIterator VectorImage::setIterator()
{
  ImageIterator iter(this->image, image->GetRequestedRegion());
  return iter;
}

} // shapeworks
