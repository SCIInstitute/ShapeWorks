/*
 * Shapeworks license
 */

#include <SurfaceReconstructor.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkNrrdImageIOFactory.h>
#include <itkMetaImageIOFactory.h>

#include <Libs/SurfaceReconstruction/Reconstruction.h>
#include <Libs/Utils/Utils.h>

//---------------------------------------------------------------------------
SurfaceReconstructor::SurfaceReconstructor()
{}

void SurfaceReconstructor::set_filenames(std::vector<string> distance_transform_filenames,
                                         std::vector<string> local_point_filenames,
                                         std::vector<string> world_point_filenames)
{
  this->distance_transform_filenames_ = distance_transform_filenames;
  this->local_point_filenames_ = local_point_filenames;
  this->world_point_filenames_ = world_point_filenames;
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::generate_mean_dense()
{

  typedef float PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;
  const unsigned int Dimension = 3;

  typedef itk::ImageFileReader< ImageType >   ReaderType;

  typedef double CoordinateRepType;
  typedef itk::CompactlySupportedRBFSparseKernelTransform < CoordinateRepType,
                                                            Dimension>     RBFTransformType;
  typedef itk::ThinPlateSplineKernelTransform2< CoordinateRepType, Dimension>  ThinPlateSplineType;
  typedef itk::LinearInterpolateImageFunction<ImageType, double > InterpolatorType;

  typedef Reconstruction < itk::ThinPlateSplineKernelTransform2,
                           itk::LinearInterpolateImageFunction,
                           CoordinateRepType, PixelType, ImageType> ReconstructionType;
  typedef typename ReconstructionType::PointType PointType;
  typedef typename ReconstructionType::PointArrayType PointArrayType;

  ReconstructionType reconstructor;

  std::cerr << "Generate Mean Dense!\n";

  // read local points and world points if given
  std::vector< PointArrayType > local_pts;  local_pts.clear();
  std::vector< PointArrayType > global_pts; global_pts.clear();

  // read local points
  for (unsigned int shapeNo = 0; shapeNo < this->local_point_filenames_.size(); shapeNo++) {
    std::cout << "Loading local points file: " << this->local_point_filenames_[shapeNo].c_str() <<
      std::endl;

    PointArrayType curShape;
    Utils::readSparseShape(curShape,
                           const_cast<char*> (this->local_point_filenames_[shapeNo].c_str()));

    local_pts.push_back(curShape);
  }

  // read distance transforms
  std::vector<typename ImageType::Pointer> distance_transforms;
  for (unsigned int shapeNo = 0; shapeNo < this->distance_transform_filenames_.size(); shapeNo++) {
    std::string filename = this->distance_transform_filenames_[shapeNo];

    if (filename.find(".nrrd") != std::string::npos) {
      itk::NrrdImageIOFactory::RegisterOneFactory();
    }
    else
    if (filename.find(".mha") != std::string::npos) {
      itk::MetaImageIOFactory::RegisterOneFactory();
    }
    typename ReaderType::Pointer reader = ReaderType::New();
    std::cout << "Reading distance transform file : " << filename << std::endl;
    reader->SetFileName(filename.c_str());
    reader->Update();
    distance_transforms.push_back(reader->GetOutput());
  }

  std::cout << "Computing mean sparse shape .... \n ";
  PointType commonCenter;
  global_pts = reconstructor.computeSparseMean(local_pts, commonCenter, false, false);
  global_pts.clear(); // clear

  // read given world points
  for (unsigned int shapeNo = 0; shapeNo < this->world_point_filenames_.size(); shapeNo++) {
    std::cout << "Loading world points file: " << this->world_point_filenames_[shapeNo].c_str() <<
    std::endl;

    PointArrayType curShape;
    Utils::readSparseShape(curShape,
                           const_cast<char*> (this->world_point_filenames_[shapeNo].c_str()));

    for (unsigned int ii = 0; ii < curShape.size(); ii++) {
      curShape[ii][0] -= commonCenter[0];
      curShape[ii][1] -= commonCenter[1];
      curShape[ii][2] -= commonCenter[2];
    }

    global_pts.push_back(curShape);
  }

  // compute the dense shape
  vtkSmartPointer<vtkPolyData> denseMean = reconstructor.getDenseMean(local_pts, global_pts,
                                                                      distance_transforms);


}
