#pragma once

/**
 * @file SurfaceReconstructor.h
 * @brief Surface Reconstruction Layer
 *
 * The SurfaceReconstructor wraps the surface reconstruction method
 */

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

  //**********************************************//
  //************Imported From Studio *************//

  void initializeReconstruction(std::vector<std::vector<itk::Point<double>>> local_pts,
                                std::vector<std::vector<itk::Point<double>>> global_pts,
                                std::vector<std::string> distance_transforms,
                                double maxAngle,
                                float decimationPercent,
                                int numClusters);
  bool hasDenseMean();
  void setMean(vtkSmartPointer<vtkPoints> sparseMean,
               vtkSmartPointer<vtkPolyData> denseMean,
               std::vector<bool> goodPoints);

  void writeMeanInfo(std::string baseName);
  void readMeanInfo(std::string dense,
                    std::string sparse, std::string goodPoints);
  void resetReconstruct();
  //**********************************************//

  void set_number_of_clusters(int num_clusters);
  void set_normal_angle(double angle);
  void set_decimation_percent(double decimation);

  bool get_surface_reconstruction_available();

  vtkSmartPointer<vtkPolyData> build_mesh(const vnl_vector<double>& shape);

private:
  ReconstructionType reconstructor_;

  bool surface_reconstruction_available_ = false;

  int num_clusters_ = -1;
  double decimation_percent_ = 0.3f;
  double normal_angle_ = 45.0f;
};
