#ifndef __RECONSTRUCTION_H__
#define __RECONSTRUCTION_H__

#include <Eigen/Dense>
#include <Eigen/Sparse>

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

#include <itkImageFileWriter.h>
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

    typedef itk::ImageFileWriter< ImageType >  WriterType;

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

    Reconstruction(std::string out_prefix = "",
                   float decimationPercent = 0.3f,
                   double angleThresh = 45.0f,
                   size_t numClusters = 5,
                   bool fixWinding = true,
                   bool doLaplacianSmoothingBeforeDecimation = true,
                   bool doLaplacianSmoothingAfterDecimation = true,
                   float smoothingLambda = 0.5f,
                   int smoothingIterations = 1,
                   bool usePairwiseNormalsDifferencesForGoodBad = false);
    ~Reconstruction();
    vtkSmartPointer<vtkPolyData> getDenseMean(
            std::vector< PointArrayType > local_pts =
            std::vector< PointArrayType >(),
            std::vector< PointArrayType > global_pts =
            std::vector< PointArrayType >(),
            std::vector<std::string> distance_transform =
            std::vector<std::string>() );
    void reset();

    void setDecimation(float dec);
    void setNumClusters(int num);
    void setMaxAngle(double angleDegrees);
    void setFixWinding(bool fixWinding);
    void setLaplacianSmoothingBeforeDecimation(bool doLaplacianSmoothingBeforeDecimation);
    void setLaplacianSmoothingAfterDecimation(bool doLaplacianSmoothingAfterDecimation);
    void setSmoothingLambda(float smoothingLambda);
    void setSmoothingIterations(int smoothingIterations);
    void setOutputEnabled(bool enabled);

    //! Set if the mean DT before warp is enabled or not
    //! Disabling this allows Reconstruction to use DTs that are of
    //! different sizes and with different origins
    void setMeanBeforeWarpEnabled(bool enabled);

    vtkSmartPointer<vtkPolyData> getMesh(PointArrayType local_pts);
    void readMeanInfo(std::string dense,
                      std::string sparse, std::string goodPoints);
    bool sparseDone();
    bool denseDone();
    void writeMeanInfo(std::string nameBase);

    vtkSmartPointer<vtkPoints>   SparseMean(){return sparseMean_;}
    vtkSmartPointer<vtkPolyData> DenseMean() {return denseMean_;}

    std::vector<bool> GoodPoints(){return goodPoints_;}

    std::string OutPrefix(){return out_prefix_;}
    void setOutPrefix(std::string out_prefix){out_prefix_ = out_prefix;}

    std::vector< PointArrayType >  computeSparseMean(std::vector< PointArrayType > local_pts,
                                                     itk::Point<TCoordRep>& common_center,
                                                     bool do_procrustes = true,
                                                     bool do_procrustes_scaling = false);

    void setOrigin(typename ImageType::PointType origin)
    {
        use_origin = true;
        origin_[0] = origin[0];
        origin_[1] = origin[1];
        origin_[2] = origin[2];
    }

    void EnablePairwiseNormalsDifferencesForGoodBad(){usePairwiseNormalsDifferencesForGoodBad_ = true;}
    void DisablePairwiseNormalsDifferencesForGoodBad(){usePairwiseNormalsDifferencesForGoodBad_ = false;}

private:
    int ComputeMedianShape(std::vector<vnl_matrix<double>> & shapeList);
    void computeDenseMean(
            std::vector< PointArrayType > local_pts,
            std::vector< PointArrayType > global_pts,
            std::vector<std::string> distance_transform);
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
            vtkSmartPointer<vtkPolyData> meshIn);

    typename ImageType::Pointer loadImage(std::string filename);

    void performKMeansClustering(
            std::vector< PointArrayType > global_pts,
            unsigned int number_of_particles,
            std::vector<int> & centroidIndices);

    void writePLY(char* filename, vtkSmartPointer<vtkPolyData> meshIn);
    void writeVTK(char* filename, vtkSmartPointer<vtkPolyData> meshIn);

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

    bool fixWinding_;
    bool doLaplacianSmoothingBeforeDecimation_;
    bool doLaplacianSmoothingAfterDecimation_;
    float smoothingLambda_;
    int smoothingIterations_;

    typename ImageType::PointType origin_;
    bool use_origin;

    std::string out_prefix_; // to save intermediate files in case needed
    bool output_enabled_ = true;
    bool usePairwiseNormalsDifferencesForGoodBad_ = false;

    bool mean_before_warp_enabled_ = true;
};

#include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

#endif // !__RECONSTRUCTION_H__