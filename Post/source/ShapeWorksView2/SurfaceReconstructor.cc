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

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_filenames(std::vector<string> distance_transform_filenames,
                                         std::vector<string> local_point_filenames,
                                         std::vector<string> world_point_filenames, int num_domains)
{
  this->distance_transform_filenames_ = distance_transform_filenames;
  this->local_point_filenames_ = local_point_filenames;
  this->world_point_filenames_ = world_point_filenames;
  this->num_domains_ = num_domains;

  for (int i = 0; i < this->num_domains_; i++) {
    this->reconstructor_.push_back(ReconstructionType());
    this->reconstructor_[i].setOutputEnabled(false);
    this->reconstructor_[i].setNumClusters(this->num_clusters_);
    this->reconstructor_[i].setMaxAngle(this->normal_angle_);
    this->reconstructor_[i].setDecimation(this->decimation_percent_);
  }
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_number_of_clusters(int num_clusters)
{
  this->num_clusters_ = num_clusters;
  for (int i = 0; i < this->reconstructor_.size(); i++) {
    this->reconstructor_[i].setNumClusters(this->num_clusters_);
  }
  std::cerr << "Setting number of reconstruction clusters to " << this->num_clusters_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_normal_angle(double angle)
{
  this->normal_angle_ = angle;
  for (int i = 0; i < this->reconstructor_.size(); i++) {
    this->reconstructor_[i].setMaxAngle(this->normal_angle_);
  }
  std::cerr << "Setting normal angle to " << this->normal_angle_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::set_decimation_percent(double decimation)
{
  this->decimation_percent_ = decimation;
  for (int i = 0; i < this->reconstructor_.size(); i++) {
    this->reconstructor_[i].setDecimation(this->decimation_percent_);
  }
  std::cerr << "Setting decimation percent to " << this->decimation_percent_ << "\n";
}

//---------------------------------------------------------------------------
void SurfaceReconstructor::generate_mean_dense()
{

  std::cerr << "Generate Mean Dense!\n";


  int num_samples = this->local_point_filenames_.size() / this->num_domains_;

  for (int domain = 0; domain < this->num_domains_; domain++) {

    // read local points and world points if given
    std::vector< PointArrayType > local_pts;  local_pts.clear();
    std::vector< PointArrayType > global_pts; global_pts.clear();

    // read local points
    for (unsigned int shapeNo = 0; shapeNo < num_samples; shapeNo++) {

      int file_index = shapeNo * this->num_domains_ + domain;

      std::cout << "Loading local points file: " <<
        this->local_point_filenames_[file_index].c_str() <<
        std::endl;

      PointArrayType curShape;
      Utils::readSparseShape(curShape,
                             const_cast<char*> (this->local_point_filenames_[file_index].c_str()));

      local_pts.push_back(curShape);
    }

    // read distance transforms
    std::vector<typename ImageType::Pointer> distance_transforms;
    for (unsigned int shapeNo = 0; shapeNo < num_samples; shapeNo++) {
      int file_index = shapeNo * this->num_domains_ + domain;
      std::string filename = this->distance_transform_filenames_[file_index];

      if (filename.find(".nrrd") != std::string::npos) {
        itk::NrrdImageIOFactory::RegisterOneFactory();
      }
      else if (filename.find(".mha") != std::string::npos) {
        itk::MetaImageIOFactory::RegisterOneFactory();
      }
      typename ReaderType::Pointer reader = ReaderType::New();
      std::cout << "Reading distance transform file : " << filename << std::endl;
      reader->SetFileName(filename.c_str());
      reader->Update();
      distance_transforms.push_back(reader->GetOutput());
    }

    /*
    std::cout << "Computing mean sparse shape .... \n ";
    PointType commonCenter;
    global_pts = this->reconstructor_[domain].computeSparseMean(local_pts, commonCenter, false,
                                                                false);
    global_pts.clear(); // clear
*/


    global_pts.clear(); // clear

    // read given world points
    for (unsigned int shapeNo = 0; shapeNo < num_samples; shapeNo++) {
      int file_index = shapeNo * this->num_domains_ + domain;
      std::cout << "Loading world points file: " <<
        this->world_point_filenames_[file_index].c_str() <<
        std::endl;

      PointArrayType curShape;
      Utils::readSparseShape(curShape,
                             const_cast<char*> (this->world_point_filenames_[file_index].c_str()));

      global_pts.push_back(curShape);
    }



    // finding image origin that is consistent with the given world coordinates and adjusted using the origin of images and point clouds in the local space
    typename ImageType::PointType  origin_local;
    typename ImageType::PointType  origin_global;

    // the bounding box of the local points
    double min_x_local, min_y_local, min_z_local;
    double max_x_local, max_y_local, max_z_local;

    // the bounding box of the global points
    double min_x_global, min_y_global, min_z_global;
    double max_x_global, max_y_global, max_z_global;

    // compute the center of mass for both local and global points
    Utils::computeCenterOfMassForShapeEnsemble(local_pts,  origin_local);
    Utils::computeCenterOfMassForShapeEnsemble(global_pts, origin_global);

    std::cout << "Center of mass of local points:" << std::endl;
    std::cout << "origin_local(0) = " << origin_local[0] << ", "
              << "origin_local(1) = " << origin_local[1] << ", "
              << "origin_local(2) = " << origin_local[2] << std::endl;

    std::cout << "Center of mass of global points:" << std::endl;
    std::cout << "origin_global(0) = " << origin_global[0] << ", "
              << "origin_global(1) = " << origin_global[1] << ", "
              << "origin_global(2) = " << origin_global[2] << std::endl;

    // find the bounding box of both local and global points
    Utils::getBoundingBoxForShapeEnsemble(local_pts,
                                          min_x_local, min_y_local, min_z_local,
                                          max_x_local, max_y_local, max_z_local);

    std::cout << "Local points bounding box:" << std::endl;
    std::cout << "min_x_local = " << min_x_local << ", "
              << "min_y_local = " << min_y_local << ", "
              << "min_z_local = " << min_z_local << std::endl;

    std::cout << "max_x_local = " << max_x_local << ", "
              << "max_y_local = " << max_y_local << ", "
              << "max_z_local = " << max_z_local << std::endl;

    Utils::getBoundingBoxForShapeEnsemble(global_pts,
                                          min_x_global, min_y_global, min_z_global,
                                          max_x_global, max_y_global, max_z_global);

    std::cout << "Global points bounding box:" << std::endl;
    std::cout << "min_x_global = " << min_x_global << ", "
              << "min_y_global = " << min_y_global << ", "
              << "min_z_global = " << min_z_global << std::endl;

    std::cout << "max_x_global = " << max_x_global << ", "
              << "max_y_global = " << max_y_global << ", "
              << "max_z_global = " << max_z_global << std::endl;

    // compute the image origin (corner) based on the center of mass
    double x_width_local = max_x_local - min_x_local;
    double y_width_local = max_y_local - min_y_local;
    double z_width_local = max_z_local - min_z_local;

    double x_width_global = max_x_global - min_x_global;
    double y_width_global = max_y_global - min_y_global;
    double z_width_global = max_z_global - min_z_global;

    origin_local[0] = origin_local[0] - (x_width_local/2.0);
    origin_local[1] = origin_local[1] - (y_width_local/2.0);
    origin_local[2] = origin_local[2] - (z_width_local/2.0);

    origin_global[0] = origin_global[0] - (x_width_global/2.0);
    origin_global[1] = origin_global[1] - (y_width_global/2.0);
    origin_global[2] = origin_global[2] - (z_width_global/2.0);

    std::cout << "Image origin (corner) for local points: " << std::endl;
    std::cout << "origin_local(0) = " << origin_local[0] << ", "
              << "origin_local(1) = " << origin_local[1] << ", "
              << "origin_local(2) = " << origin_local[2] << std::endl;

    std::cout << "Image origin (corner) for global points: " << std::endl;
    std::cout << "origin_global(0) = " << origin_global[0] << ", "
              << "origin_global(1) = " << origin_global[1] << ", "
              << "origin_global(2) = " << origin_global[2] << std::endl;

    // origin of the distance transforms (assume all dts are sharing the same origin)
    typename ImageType::PointType  origin_dt = distance_transforms[0]->GetOrigin();

    double offset_x = origin_dt[0] - origin_local[0];
    double offset_y = origin_dt[1] - origin_local[1];
    double offset_z = origin_dt[2] - origin_local[2];

    // adjust global origin based on local offset
    origin_global[0] = origin_global[0] + offset_x;
    origin_global[1] = origin_global[1] + offset_y;
    origin_global[2] = origin_global[2] + offset_z;

    std::cout << "Image origin (corner) for global points - adjusted: " << std::endl;
    std::cout << "origin_global(0) = " << origin_global[0] << ", "
              << "origin_global(1) = " << origin_global[1] << ", "
              << "origin_global(2) = " << origin_global[2] << std::endl;

    this->reconstructor_[domain].setOrigin(origin_global);






    // compute the dense shape
    vtkSmartPointer<vtkPolyData> denseMean = this->reconstructor_[domain].getDenseMean(local_pts,
                                                                                       global_pts,
                                                                                       distance_transforms);
  }
  this->surface_reconstruction_available_ = true;
}

//---------------------------------------------------------------------------
bool SurfaceReconstructor::get_surface_reconstruction_available()
{
  return this->surface_reconstruction_available_;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> SurfaceReconstructor::build_mesh(const vnl_vector<double> &shape,
                                                              int domain)
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

  return this->reconstructor_[domain].getMesh(points);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
