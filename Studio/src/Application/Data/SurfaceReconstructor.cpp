#include <Data/SurfaceReconstructor.h>

#include <itkImage.h>
#include <itkNrrdImageIOFactory.h>
#include <itkMetaImageIOFactory.h>

#include <Libs/Utils/Utils.h>

//---------------------------------------------------------------------------
SurfaceReconstructor::SurfaceReconstructor()
{
  this->reconstructor_.setOutputEnabled(false);
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::initializeReconstruction(
  std::vector<std::vector<itk::Point<double>>> local_pts,
  std::vector<std::vector<itk::Point<double>>> global_pts,
  std::vector<ImageType::Pointer> distance_transforms,
  double maxAngle, float decimationPercent, int numClusters)
{

  this->reconstructor_.setDecimation(decimationPercent);
  this->reconstructor_.setMaxAngle(maxAngle);
  this->reconstructor_.setNumClusters(numClusters);

  // compute the dense shape
  vtkSmartPointer<vtkPolyData> denseMean = this->reconstructor_.getDenseMean(local_pts, global_pts,
                                                                             distance_transforms);

  this->surface_reconstruction_available_ = true;
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::hasDenseMean()
{
  return this->reconstructor_.denseDone();
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::writeMeanInfo(string baseName)
{
  this->reconstructor_.writeMeanInfo(baseName);
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::readMeanInfo(string dense, string sparse, string goodPoints)
{
  this->reconstructor_.readMeanInfo(dense, sparse, goodPoints);
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
  this->reconstructor_.setNumClusters(this->num_clusters_);
  std::cerr << "Setting number of reconstruction clusters to " << this->num_clusters_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_normal_angle(double angle)
{
  this->normal_angle_ = angle;
  this->reconstructor_.setMaxAngle(this->normal_angle_);
  std::cerr << "Setting normal angle to " << this->normal_angle_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_decimation_percent(double decimation)
{
  this->decimation_percent_ = decimation;
  this->reconstructor_.setDecimation(this->decimation_percent_);
  std::cerr << "Setting decimation percent to " << this->decimation_percent_ << "\n";
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::get_surface_reconstruction_available()
{
  return this->surface_reconstruction_available_;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> SurfaceReconstructor::build_mesh(const vnl_vector<double> &shape)
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

  vtkSmartPointer<vtkPolyData> poly_data = this->reconstructor_.getMesh(points);
  return poly_data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
