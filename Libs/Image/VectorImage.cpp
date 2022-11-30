#include "VectorImage.h"

namespace shapeworks {

VectorImage::VectorImage(const Image& dt_img) {
  GradientImageFilter::Pointer filter = GradientImageFilter::New();
  filter->SetInput(dt_img.getITKImage());
  filter->SetUseImageSpacingOn();
  filter->Update();
  itk_image_ = filter->GetOutput();
  interpolator_ = GradientInterpolatorType::New();
  interpolator_->SetInputImage(itk_image_);
}

Vector VectorImage::evaluate(Point p) { return toVector(interpolator_->Evaluate(p)); }

VectorImage::ImageIterator VectorImage::iterator() {
  ImageIterator iter(itk_image_, itk_image_->GetRequestedRegion());
  return iter;
}

}  // namespace shapeworks
