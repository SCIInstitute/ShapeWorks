#ifndef __RECONSTRUCTION_H__
#define __RECONSTRUCTION_H__

#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Sparse>

#include "itkThinPlateSplineKernelTransform2.h"
#include "itkCompactlySupportedRBFSparseKernelTransform.h"

#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>

#include <vtkPolyData.h>
#include <itkAddImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkResampleImageFilter.h>

#include <itkLinearInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>

#include <itkMultiplyImageFilter.h>
#include "itkImageRegionConstIterator.h"
#include <itkImageDuplicator.h>
#include <vtkSmartPointer.h>

#include "Procrustes3D.h"

#ifdef assert
#undef assert
#define assert(a) { if (!static_cast<bool>(a)) { throw std::runtime_error("a"); } }
#endif

namespace itk
{
template<typename TImageType, typename TCoordRep = double>
class ITK_TEMPLATE_EXPORT BSplineInterpolateImageFunctionWithDoubleCoefficents
        : public BSplineInterpolateImageFunction <TImageType, TCoordRep, double>
{};
}

template < template < typename TCoordRep, unsigned > class TTransformType = itk::CompactlySupportedRBFSparseKernelTransform,
           template < typename ImageType, typename TCoordRep > class TInterpolatorType = itk::LinearInterpolateImageFunction,
           typename TCoordRep = double, typename PixelType = float, typename ImageType = itk::Image<PixelType, 3>>
class Reconstruction {
public:
    typedef itk::GradientImageFilter<ImageType, PixelType>               GradientFilterType;
    typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType > GradientMagnitudeFilterType;
    typedef itk::Image< itk::CovariantVector< PixelType, 3 >, 3 >        GradientImageType;
    typedef itk::ImageRegionIterator< GradientImageType >            GradientImageIteratorType;
    typedef itk::ImageRegionIterator< ImageType >                    ImageIteratorType;

    typedef itk::ImageToVTKImageFilter<ImageType>                    ITK2VTKConnectorType;
    typedef itk::AddImageFilter <ImageType, ImageType >              AddImageFilterType;
    typedef itk::ResampleImageFilter<ImageType, ImageType >          ResampleFilterType;

    typedef TInterpolatorType < ImageType, TCoordRep >                  InterpolatorType;
    typedef itk::MultiplyImageFilter <ImageType, ImageType, ImageType>  MultiplyByConstantImageFilterType;

    typedef itk::ImageDuplicator< ImageType >                           DuplicatorType;
    typedef TTransformType < TCoordRep, 3 >                             TransformType;

    typedef itk::Point< TCoordRep, 3 >                  PointType;
    typedef std::vector< PointType >                    PointArrayType;
    typedef typename TransformType::PointSetType        PointSetType;
    typedef typename PointSetType::PointIdentifier      PointIdType;

    Reconstruction(float decimationPercent = 0.3f,
                   double angleThresh = 45.0f,
                   size_t numClusters = 5);
    ~Reconstruction();
    vtkSmartPointer<vtkPolyData> getDenseMean(
            std::vector< PointArrayType > local_pts =
            std::vector< PointArrayType >(),
            std::vector< PointArrayType > global_pts =
            std::vector< PointArrayType >(),
            std::vector<typename ImageType::Pointer> distance_transform =
            std::vector<typename ImageType::Pointer>() );
    void reset();
    void setDecimation(float dec);
    void setNumClusters(int num);
    void setMaxAngle(double angleDegrees);
    vtkSmartPointer<vtkPolyData> getMesh(PointArrayType local_pts);
    void readMeanInfo(std::string dense,
                      std::string sparse, std::string goodPoints);
    bool sparseDone();
    bool denseDone();
    void writeMeanInfo(std::string nameBase);
private:
    std::vector< PointArrayType >  computeSparseMean(std::vector< PointArrayType > local_pts,
                                                                     bool do_procrustes = true, bool do_procrustes_scaling = false);
    void computeDenseMean(
            std::vector< PointArrayType > local_pts,
            std::vector< PointArrayType > global_pts,
            std::vector<typename ImageType::Pointer> distance_transform);
    vnl_matrix<double> computeParticlesNormals(
            vtkSmartPointer< vtkPoints > particles,
            typename ImageType::Pointer distance_transform);
    void generateWarpedMeshes(typename TransformType::Pointer transform,
                              vtkSmartPointer<vtkPolyData>& outputMesh);
    double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points,
                                             std::vector<int> particles_indices);
    void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                      vtkSmartPointer<vtkPoints> targetPts,
                      typename TransformType::Pointer transform,
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
            vtkSmartPointer<vtkImageData> volData,
            float levelsetValue        = 0.0f,
            float targetReduction      = 0.1f,
            float featureAngle         = 30,
            int lsSmootherIterations   = 1,
            int meshSmootherIterations = 1,
            bool preserveTopology      = true);
    vtkSmartPointer<vtkPolyData> MeshQC(
            vtkSmartPointer<vtkPolyData> meshIn,
            bool fixWinding = true,
            bool doLaplacianSmoothingBeforeDecimation = true,
            bool doLaplacianSmoothingAfterDecimation = true,
            float smoothingLambda = 0.5f,
            int smoothingIterations = 1);
    void performKMeansClustering(
            std::vector< PointArrayType > global_pts,
            unsigned int number_of_particles,
            std::vector<int> & centroidIndices);
    //members.
    vtkSmartPointer<vtkPoints> sparseMean_;
    vtkSmartPointer<vtkPolyData> denseMean_;
    std::vector<bool> goodPoints_;
    bool sparseDone_;
    bool denseDone_;
    float decimationPercent_;
    double maxAngleDegrees_;
    size_t numClusters_;
    int medianShapeIndex_;
};

#endif // !__RECONSTRUCTION_H__
