/*
 * Shapeworks license
 */

/**
 * @file SurfaceReconstructor.h
 * @brief Surface Reconstruction Layer
 *
 * The SurfaceReconstructor wraps the surface reconstruction method
 */

#pragma once

#include <vector>
#include <string>

#include <itkImageFileReader.h>

#include <Reconstruction.h>

class SurfaceReconstructor
{

public:

  typedef float PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;

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

  SurfaceReconstructor();

  void set_filenames(std::vector< std::string > distance_transform_filenames,
                     std::vector< std::string > local_point_filenames,
                     std::vector< std::string > world_point_filenames,
                     int num_domains);

  void set_number_of_clusters(int num_clusters);
  void set_normal_angle(double angle);
  void set_decimation_percent(double decimation);

  void generate_mean_dense();

  bool get_surface_reconstruction_available();

  vtkSmartPointer<vtkPolyData> build_mesh(const vnl_vector<double>& shape, int domain);

private:
  std::vector<ReconstructionType> reconstructor_;

  bool surface_reconstruction_available_ = false;
  int num_domains_ = 1;

  std::vector< std::string > distance_transform_filenames_;
  std::vector< std::string > world_point_filenames_;
  std::vector< std::string > local_point_filenames_;

  int num_clusters_ = -1;
  double decimation_percent_ = 0.3f;
  double normal_angle_ = 45.0f;
};
