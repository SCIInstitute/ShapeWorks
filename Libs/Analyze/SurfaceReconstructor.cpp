#include <SurfaceReconstructor.h>

#include <itkImage.h>

#include <Utils/Utils.h>

#include <Reconstruction.h>


typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;


typedef double CoordinateRepType;
typedef itk::CompactlySupportedRBFSparseKernelTransform < CoordinateRepType,
                                                          3> RBFTransformType;
typedef itk::ThinPlateSplineKernelTransform2< CoordinateRepType, 3> ThinPlateSplineType;

typedef itk::LinearInterpolateImageFunction<ImageType, double > InterpolatorType;

typedef Reconstruction < itk::CompactlySupportedRBFSparseKernelTransform,
                         itk::LinearInterpolateImageFunction,
                         CoordinateRepType, PixelType, ImageType> ReconstructionType;

/*
   typedef Reconstruction < itk::ThinPlateSplineKernelTransform2,
                           itk::LinearInterpolateImageFunction,
                           CoordinateRepType, PixelType, ImageType> ReconstructionType;
 */

typedef typename ReconstructionType::PointType PointType;
typedef typename ReconstructionType::PointArrayType PointArrayType;



class SurfaceReconstructorPrivate {
 public:
  ReconstructionType reconstructor_;
};

//---------------------------------------------------------------------------
SurfaceReconstructor::SurfaceReconstructor() : private_(new SurfaceReconstructorPrivate)
{
  private_->reconstructor_.setOutputEnabled(false);
}

SurfaceReconstructor::~SurfaceReconstructor() = default;

//---------------------------------------------------------------------------
void SurfaceReconstructor::initializeReconstruction(
  std::vector<std::vector<itk::Point<double>>> local_pts,
  std::vector<std::vector<itk::Point<double>>> global_pts,
  std::vector<std::string> distance_transforms,
  double maxAngle, float decimationPercent, int numClusters)
{

  private_->reconstructor_.setDecimation(decimationPercent);
  private_->reconstructor_.setMaxAngle(maxAngle);
  private_->reconstructor_.setNumClusters(numClusters);
  private_->reconstructor_.setMeanBeforeWarpEnabled(false);

  // compute the dense shape
  vtkSmartPointer<vtkPolyData> denseMean = private_->reconstructor_.getDenseMean(local_pts, global_pts,
                                                                             distance_transforms);

  this->surface_reconstruction_available_ = true;
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::hasDenseMean()
{
  return private_->reconstructor_.denseDone();
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::writeMeanInfo(string baseName)
{
  private_->reconstructor_.writeMeanInfo(baseName);
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::readMeanInfo(string dense, string sparse, string goodPoints)
{
  private_->reconstructor_.readMeanInfo(dense, sparse, goodPoints);
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::resetReconstruct()
{
  this->surface_reconstruction_available_ = false;
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_number_of_clusters(int num_clusters)
{
  this->num_clusters_ = num_clusters;
  private_->reconstructor_.setNumClusters(this->num_clusters_);
  std::cerr << "Setting number of reconstruction clusters to " << this->num_clusters_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_normal_angle(double angle)
{
  this->normal_angle_ = angle;
  private_->reconstructor_.setMaxAngle(this->normal_angle_);
  std::cerr << "Setting normal angle to " << this->normal_angle_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_decimation_percent(double decimation)
{
  this->decimation_percent_ = decimation;
  private_->reconstructor_.setDecimation(this->decimation_percent_);
  std::cerr << "Setting decimation percent to " << this->decimation_percent_ << "\n";
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::get_surface_reconstruction_available()
{
  return this->surface_reconstruction_available_;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> SurfaceReconstructor::build_mesh(const Eigen::VectorXd &shape)
{
  PointArrayType points;

  unsigned int numPoints = static_cast<unsigned int>(shape.size() / 3);
  unsigned int k = 0;
  for (unsigned int i = 0; i < numPoints; i++) {
    double x = shape[k++];
    double y = shape[k++];
    double z = shape[k++];
    PointType point;
    point[0] = x;
    point[1] = y;
    point[2] = z;
    points.push_back(point);
  }

  vtkSmartPointer<vtkPolyData> poly_data = private_->reconstructor_.getMesh(points);
  return poly_data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
