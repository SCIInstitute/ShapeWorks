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
///   Transform xform = ImageUtils::createCenterOfMassTransform(image);
///   image.applyTransform(xform);
///
/// \param image      the binary image from which to generate the transform
Transform::Pointer ImageUtils::createCenterOfMassTransform(const Image &image)
{
  Point3 com = image.centerOfMass();
  Point3 center = image.center();

  std::cout << "ImageUtils::createCenterOfMassTransform\n\tcom: " << com << "\n\tctr: " << center << std::endl;

  Transform::Pointer xform = Transform::New();
  xform->Translate(center - com);
  return xform;
}

Transform::Pointer ImageUtils::rigidRegistration(const Image &image, Image &target, Image &source, float isoValue, unsigned iterations)
{
  vtkSmartPointer<vtkPolyData> targetContour = image.convert(target, isoValue);
  vtkSmartPointer<vtkPolyData> movingContour = image.convert(source, isoValue);
  Matrix mat = ShapeworksUtils::icp(targetContour, movingContour, iterations);
  Transform::Pointer xform = Transform::New();
  xform->SetMatrix(mat);
  return xform;
}

} //shapeworks
