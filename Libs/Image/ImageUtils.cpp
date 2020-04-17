#include "ImageUtils.h"

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

  Matrix33 reflection;
  reflection[0][0] = x;
  reflection[0][1] = reflection[0][2] = 0;
  reflection[1][1] = y;
  reflection[1][0] = reflection[1][2] = 0;
  reflection[2][2] = z;
  reflection[2][0] = reflection[2][1] = 0;

  Point3 origin = image.origin();
  image.recenter();
  Transform xform;
  xform.setMatrix(reflection);
  image.applyTransform(xform);
  image.changeOrigin();
  return image;
}

Image ImageUtils::rigidRegisteration(Image &img)
{
  Image &image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image &>(img);

  // ShapeworksUtils::icp();

  return image;
}

} //shapeworks
