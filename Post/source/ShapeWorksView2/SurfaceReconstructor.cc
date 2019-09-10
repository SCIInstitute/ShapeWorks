/*
 * Shapeworks license
 */

#include <SurfaceReconstructor.h>

#include <itkImage.h>
#include <itkNrrdImageIOFactory.h>
#include <itkMetaImageIOFactory.h>

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
void SurfaceReconstructor::generate_mean_dense()
{

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
  global_pts = this->reconstructor_.computeSparseMean(local_pts, commonCenter, false, false);
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
  vtkSmartPointer<vtkPolyData> denseMean = this->reconstructor_.getDenseMean(local_pts, global_pts,
                                                                             distance_transforms);
  this->surface_reconstruction_available_ = true;
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::get_surface_reconstruction_avaiable()
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

  return this->reconstructor_.getMesh(points);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
