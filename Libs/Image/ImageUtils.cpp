#include "ImageUtils.h"

namespace shapeworks {

Transform ImageUtils::createCenterOfMassTransform(const Image &image)
{
  Point3 com = image.centerOfMass();
  Point3 center = image.center();

  Transform xform;
  xform.translate(center - com);
  return xform;
}

} //shapeworks
