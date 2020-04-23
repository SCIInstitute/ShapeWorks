#include "ImageUtils.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace shapeworks {

/// createCenterOfMassTransform
///
/// Generates the Transform necessary to move the contents of this binary image to the center.
/// Example:
///   Transform xform = ImageUtils::createCenterOfMassTransform(image);
///   image.applyTransform(xform);
///
/// \param image      the binary image from which to generate the transform
Transform ImageUtils::createCenterOfMassTransform(const Image &image)
{
  Point3 com = image.centerOfMass();
  Point3 center = image.center();

  std::cout << "ImageUtils::createCenterOfMassTransform\n\tcom: " << com << "\n\tctr: " << center << std::endl;

  Transform xform;
  xform.translate(center - com);
  return xform;
}

Image ImageUtils::reflect(Image &img, double axis)
{
  Image &image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image &>(img);

  double x = 1, y = 1, z = 1;
  if (axis == 0)
    x = -1;
  if (axis == 1)
    y = -1;
  if (axis == 2)
    z = -1;

  Matrix reflection;
  reflection.Fill(0);
  reflection[0][0] = x;
  reflection[1][1] = y;
  reflection[2][2] = z;

  Transform xform;
  xform.setMatrix(reflection);
  Point3 origin = image.origin();
  image.recenter().applyTransform(xform).changeOrigin();
  return image;
}

Image ImageUtils::rigidRegisteration(Image &img, Image &target, Image &source, float isoValue, unsigned iterations)
{
  Image &image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image &>(img);

  vtkSmartPointer<vtkPolyData> targetContour = image.convert(target, isoValue);
  vtkSmartPointer<vtkPolyData> movingContour = image.convert(source, isoValue);
  Matrix mat = ShapeworksUtils::icp(targetContour, movingContour, iterations);
  Transform xform;
  xform.setMatrix(mat);
  target.applyTransform(xform);
  return target;
}

} //shapeworks
