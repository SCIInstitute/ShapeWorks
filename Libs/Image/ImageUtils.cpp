#include "ImageUtils.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <itkTranslationTransform.h>
#include <itkThinPlateSplineKernelTransform.h>
#include <itkPointSet.h>

namespace shapeworks {

Image::Region ImageUtils::boundingBox(std::vector<std::string> &filenames)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided from which to compute a bounding box"); 

  Image img(filenames[0]);
  Image::Region bbox(img.boundingBox());
  Dims dims(img.dims()); // images must all be the same size

  for (auto filename : filenames)
  {
    Image img(filename);
    if (img.dims() != dims)
    {
      throw std::invalid_argument("Image sizes do not match (" + filename + ")");
    }

    bbox.grow(img.boundingBox());
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
  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(-(image.center() - image.centerOfMass())); // ITK translations go in a counterintuitive direction
  return xform;
}

Image ImageUtils::rigidRegistration(Image &target, const Image &source, float isoValue, unsigned iterations)
{
  vtkSmartPointer<vtkPolyData> targetContour = Image::getPolyData(target, isoValue);
  vtkSmartPointer<vtkPolyData> sourceContour = Image::getPolyData(source, isoValue);
  Matrix mat = ShapeworksUtils::icp(targetContour, sourceContour, iterations);
  AffineTransformPtr xform(AffineTransform::New());
  xform->SetMatrix(mat);
  target.applyTransform(xform);
  return target;
}

TransformPtr ImageUtils::computeWarp(const std::string &source_file, const std::string &target_file, const int pointFactor)
{ 
  typedef itk::ThinPlateSplineKernelTransform<double, 3> TPSTransform;
  typedef TPSTransform::PointSetType PointSet;

  // Read the source and target sets of landmark points
  PointSet::Pointer sourceLandMarks = PointSet::New();
  PointSet::Pointer targetLandMarks = PointSet::New();
  PointSet::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
  PointSet::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();

  std::ifstream insourcefile;
  std::ifstream intargetfile;
  insourcefile.open(source_file.c_str());
  intargetfile.open(target_file.c_str());
  if (!insourcefile.is_open() || !intargetfile.is_open()) return TPSTransform::New();

  PointSet::PointIdentifier id{itk::NumericTraits<PointSet::PointIdentifier>::Zero};
  Point3 src, tgt;
  int count{0};

  while (!insourcefile.eof() && !intargetfile.eof())
  {
    insourcefile >> src[0] >> src[1] >> src[2];
    intargetfile >> tgt[0] >> tgt[1] >> tgt[2];

    if (count % pointFactor == 0)
    {
      // swap src and tgt b/c ITK transforms go backwards (must be inverted on creation since some do not provide an invert function)
      sourceLandMarkContainer->InsertElement( id, tgt );
      targetLandMarkContainer->InsertElement( id, src );
      id++;
    }
    count++;
  }
  insourcefile.close();
  intargetfile.close();

  // Create and return warp transform
  TPSTransform::Pointer tps(TPSTransform::New());
  tps->SetSourceLandmarks(sourceLandMarks);
  tps->SetTargetLandmarks(targetLandMarks);
  tps->ComputeWMatrix();

  return tps;
}

} //shapeworks
