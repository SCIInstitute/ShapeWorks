#include "ImageUtils.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace shapeworks {

Image::Region ImageUtils::boundingBox(std::vector<std::string> &filenames, Image::PixelType isoValue)
{
  if (filenames.empty())
    throw std::invalid_argument("no filenames provided from which to compute a bounding box"); 

  Image img(filenames[0]);
  Image::Region bbox(img.boundingBox(isoValue));
  Dims dims(img.dims()); // images must all be the same size

	auto filename = filenames.begin();
	while (++filename != filenames.end())
  {
    Image img(*filename);
    if (img.dims() != dims) { throw std::invalid_argument("image sizes do not match (" + *filename + ")"); }

    bbox.grow(img.boundingBox(isoValue));

		++filename;
  }

  return bbox;
}

/// createCenterOfMassTransform
///
/// Generates the Transform necessary to move the contents of this binary image to the center.
/// Example:
///   TransformPtr xform = ImageUtils::createCenterOfMassTransform(image);
///   image.applyTransform(xform);
///
/// \param image      the binary image from which to generate the transform
TransformPtr ImageUtils::createCenterOfMassTransform(const Image &image)
{
  TransformPtr xform(TransformType::New());
  xform->Translate(image.center() - image.centerOfMass());
  return xform;
}

Image ImageUtils::rigidRegistration(Image &target, const Image &source, float isoValue, unsigned iterations)
{
  vtkSmartPointer<vtkPolyData> targetContour = Image::getPolyData(target, isoValue);
  vtkSmartPointer<vtkPolyData> sourceContour = Image::getPolyData(source, isoValue);
  Matrix mat = ShapeworksUtils::icp(targetContour, sourceContour, iterations);
  TransformPtr xform(TransformType::New());
  xform->SetMatrix(mat);
  target.applyTransform(xform);
  return target;
}

} //shapeworks
