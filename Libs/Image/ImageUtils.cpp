#include "ImageUtils.h"

#include <itkPointSet.h>

namespace shapeworks {

PhysicalRegion ImageUtils::boundingBox(const std::vector<std::string>& filenames,
                                       Image::PixelType isoValue)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided to compute a bounding box");

  Image img(filenames[0]);
  PhysicalRegion bbox;
  Dims dims(img.dims()); // images must all be the same size

  for (auto filename : filenames)
  {
    Image img(filename);
    if (img.dims() != dims)
    {
      throw std::invalid_argument("Image sizes do not match (" + filename + ")");
    }

    bbox.expand(img.physicalBoundingBox(isoValue));
  }

  return bbox;
}

PhysicalRegion ImageUtils::boundingBox(const std::vector<std::reference_wrapper<const Image>>& images,
                                       Image::PixelType isoValue)
{
  if (images.empty())
    throw std::invalid_argument("No images provided to compute a bounding box");

  PhysicalRegion bbox;
  Dims dims(images[0].get().dims()); // images must all be the same size

  for (auto img : images)
  {
    if (img.get().dims() != dims)
      throw std::invalid_argument("Image sizes do not match");

    bbox.expand(img.get().physicalBoundingBox(isoValue));
  }

  return bbox;
}

ImageUtils::TPSTransform::Pointer ImageUtils::createWarpTransform(const std::string& source_landmarks_file,
                                                                  const std::string& target_landmarks_file,
                                                                  const int stride)
{ 
  typedef TPSTransform::PointSetType PointSet;

  // Read the source and target sets of landmark points
  PointSet::Pointer sourceLandMarks = PointSet::New();
  PointSet::Pointer targetLandMarks = PointSet::New();
  PointSet::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
  PointSet::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();

  std::ifstream insourcefile;
  std::ifstream intargetfile;
  insourcefile.open(source_landmarks_file);
  intargetfile.open(target_landmarks_file);
  if (!insourcefile.is_open() || !intargetfile.is_open())  {
    throw std::invalid_argument("Error: unable to open landmark files. Check paths.");
  }

  PointSet::PointIdentifier id{itk::NumericTraits<PointSet::PointIdentifier>::Zero};
  Point3 src, tgt;
  int count{0};

  while (!insourcefile.eof() && !intargetfile.eof())
  {
    insourcefile >> src[0] >> src[1] >> src[2];
    intargetfile >> tgt[0] >> tgt[1] >> tgt[2];

    if (count % stride == 0)
    {
      // swap src and tgt b/c ITK transforms must be inverted on creation since some do not provide an invert function
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
