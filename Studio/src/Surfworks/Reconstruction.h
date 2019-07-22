#ifndef __RECONSTRUCTION_H__
#define __RECONSTRUCTION_H__



#include "itkCompactlySupportedRBFSparseKernelTransform.h"
#include "ShapeWorksGroom.h"
#include "ITKVTK/itkImageToVTKImageFilter.h"
#include "ITKVTK/itkVTKImageToImageFilter.h"
#include <vtkPolyData.h>
#include <itkAddImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMultiplyImageFilter.h>
#include "itkImageRegionConstIterator.h"
#include <itkImageDuplicator.h>
#include <vtkSmartPointer.h>

#ifdef assert
#undef assert
#define assert(a) { if (!static_cast<bool>(a)) { throw std::runtime_error("a"); } }
#endif

class Reconstruction {
  typedef itk::GradientImageFilter<ImageType, float> 
                                                     GradientFilterType;
  typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType >
                                                     GradientMagnitudeFilterType;
  typedef itk::Image< itk::CovariantVector< float, 3 >, 3 > 
                                                     GradientImageType;
  typedef itk::ImageRegionIterator< GradientImageType > 
                                                     GradientImageIteratorType;
  typedef itk::ImageRegionIterator< ImageType >      ImageIteratorType;

  typedef itk::ImageToVTKImageFilter<ImageType>      ITK2VTKConnectorType;
  typedef itk::AddImageFilter <ImageType, ImageType >
                                                     AddImageFilterType;
  typedef itk::ResampleImageFilter<ImageType, ImageType >
                                                     ResampleFilterType;
  typedef itk::BSplineInterpolateImageFunction<ImageType, double, double > 
                                                     InterpolatorType;
  typedef itk::MultiplyImageFilter <ImageType, ImageType, ImageType>
                                                     MultiplyByConstantImageFilterType;

  typedef itk::ImageDuplicator< ImageType >          DuplicatorType;
  typedef double                                     CoordinateRepType;
  typedef itk::CompactlySupportedRBFSparseKernelTransform 
    <CoordinateRepType, 3>                    RBFTransformType;
  typedef itk::Point< CoordinateRepType, 3 >  PointType;
  typedef std::vector< PointType >                    PointArrayType;
  typedef RBFTransformType::PointSetType              PointSetType;
  typedef PointSetType::PointIdentifier               PointIdType;
public:
  Reconstruction(float decimationPercent = 0.3, double angleThresh = 45.);
  ~Reconstruction();
  vtkSmartPointer<vtkPolyData> getDenseMean(
    std::vector<std::vector<itk::Point<float> > > local_pts =
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<std::vector<itk::Point<float> > > global_pts =
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<ImageType::Pointer> distance_transform = 
    std::vector<ImageType::Pointer>() );
  void reset();
  void setDecimation(float dec);
  void setNumClusters(int num);
  void setMaxAngle(double angleDegrees);
  vtkSmartPointer<vtkPolyData> 
    getMesh(std::vector<itk::Point<float> > local_pts);
  void readMeanInfo(std::string dense, 
    std::string sparse, std::string goodPoints);
  bool denseDone();
  void writeMeanInfo(std::string nameBase);
private:
  void computeDenseMean(
    std::vector<std::vector<itk::Point<float> > > local_pts,
    std::vector<std::vector<itk::Point<float> > > global_pts,
    std::vector<ImageType::Pointer> distance_transform);
  vnl_matrix<double> computeParticlesNormals(
    vtkSmartPointer< vtkPoints > particles,
    ImageType::Pointer distance_transform);
  void generateWarpedMeshes(RBFTransformType::Pointer transform,
    vtkSmartPointer<vtkPolyData>& outputMesh);
  double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points, 
    std::vector<int> particles_indices);
  void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
    vtkSmartPointer<vtkPoints> targetPts,
    RBFTransformType::Pointer rbfTransform,
    vtkSmartPointer<vtkPoints> & mappedCorrespondences,
    double & rms, double & rms_wo_mapping, double & maxmDist);
  vtkSmartPointer<vtkPoints> convertToImageCoordinates(
    vtkSmartPointer<vtkPoints> particles, int number_of_particles,
    const itk::Image< float, 3 >::SpacingType& spacing,
    const itk::Image< float, 3 >::PointType& origin);
  vtkSmartPointer<vtkPoints> convertToPhysicalCoordinates(
    vtkSmartPointer<vtkPoints> particles, int number_of_particles,
    const itk::Image< float, 3 >::SpacingType& spacing,
    const itk::Image< float, 3 >::PointType& origin);
  vtkSmartPointer<vtkPolyData> extractIsosurface(
    vtkSmartPointer<vtkImageData> volData);
  vtkSmartPointer<vtkPolyData> MeshQC(
    vtkSmartPointer<vtkPolyData> meshIn);
  void performKMeansClustering(
    std::vector<std::vector<itk::Point<float> > > global_pts,
    unsigned int number_of_particles, 
    std::vector<int> & centroidIndices);
  //members.
  vtkSmartPointer<vtkPoints> sparseMean_;
  vtkSmartPointer<vtkPolyData> denseMean_;
  std::vector<bool> goodPoints_;
  bool denseDone_;
  float decimationPercent_;
  double maxAngleDegrees_;
  size_t numClusters_;
};
#endif // !__RECONSTRUCTION_H__
