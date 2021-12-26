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

#include "Image.h"

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

namespace shapeworks {
class Reconstruction {
public:
    using TCoordRep = double;
    template < typename TCoordRep, unsigned > class TTransformType;

    typedef itk::GradientImageFilter<Image::ImageType, Image::PixelType>               GradientFilterType;
    typedef itk::GradientMagnitudeImageFilter<Image::ImageType, Image::ImageType > GradientMagnitudeFilterType;
    typedef itk::Image< itk::CovariantVector< Image::PixelType, 3 >, 3 >        GradientImageType;
    typedef itk::ImageRegionIterator< GradientImageType >            GradientImageIteratorType;
    typedef itk::ImageRegionIterator< Image::ImageType >                    ImageIteratorType;

    typedef itk::ImageFileWriter< Image::ImageType >  WriterType;

    typedef itk::ImageToVTKImageFilter<Image::ImageType>                    ITK2VTKConnectorType;
    typedef itk::AddImageFilter <Image::ImageType, Image::ImageType >              AddImageFilterType;
    typedef itk::ResampleImageFilter<Image::ImageType, Image::ImageType >          ResampleFilterType;

    typedef itk::MultiplyImageFilter <Image::ImageType, Image::ImageType, Image::ImageType>  MultiplyByConstantImageFilterType;

    typedef itk::ImageDuplicator< Image::ImageType >                           DuplicatorType;

    typedef itk::CompactlySupportedRBFSparseKernelTransform<double, 3>                            TransformType;

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

    std::vector< PointArrayType >  computeSparseMean(std::vector< PointArrayType > local_pts, // consolidate
                                                     itk::Point<TCoordRep>& common_center,
                                                     bool do_procrustes = true,
                                                     bool do_procrustes_scaling = false);

    void setOrigin(typename Image::ImageType::PointType origin) // remove
    {
        use_origin = true;
        origin_[0] = origin[0];
        origin_[1] = origin[1];
        origin_[2] = origin[2];
    }

private:
    int ComputeMedianShape(std::vector<vnl_matrix<double>> & shapeList); // consolidate
    void computeDenseMean( // consolidate
            std::vector< PointArrayType > local_pts,
            std::vector< PointArrayType > global_pts,
            std::vector<std::string> distance_transform);
    vnl_matrix<double> computeParticlesNormals( // consolidate
            vtkSmartPointer< vtkPoints > particles,
            typename Image::ImageType::Pointer distance_transform);
    void generateWarpedMeshes(typename TransformType::Pointer transform, // consolidate
                              vtkSmartPointer<vtkPolyData>& outputMesh);
    double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points, // consolidate
                                             std::vector<int> particles_indices);
    void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts, // consolidate
                      vtkSmartPointer<vtkPoints> targetPts,
                      typename TransformType::Pointer transform,
                      vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                      double & rms, double & rms_wo_mapping, double & maxmDist);

    vtkSmartPointer<vtkPoints> convertToImageCoordinates( // remove
            vtkSmartPointer<vtkPoints> particles, int number_of_particles,
            const itk::Image< float, 3 >::SpacingType& spacing,
            const itk::Image< float, 3 >::PointType& origin);
    vtkSmartPointer<vtkPoints> convertToPhysicalCoordinates( // remove
            vtkSmartPointer<vtkPoints> particles, int number_of_particles,
            const itk::Image< float, 3 >::SpacingType& spacing,
            const itk::Image< float, 3 >::PointType& origin);

    vtkSmartPointer<vtkPolyData> extractIsosurface( // consolidate
            vtkSmartPointer<vtkImageData> volData,
            float levelsetValue        = 0.0f,
            float targetReduction      = 0.1f,
            float featureAngle         = 30,
            int lsSmootherIterations   = 1,
            int meshSmootherIterations = 1,
            bool preserveTopology      = true);
    vtkSmartPointer<vtkPolyData> MeshQC( // maybe remove
            vtkSmartPointer<vtkPolyData> meshIn);

    typename Image::ImageType::Pointer loadImage(std::string filename); // remove

    void performKMeansClustering( // consolidate
            std::vector< PointArrayType > global_pts,
            unsigned int number_of_particles,
            std::vector<int> & centroidIndices);

    void writePLY(char* filename, vtkSmartPointer<vtkPolyData> meshIn); // remove
    void writeVTK(char* filename, vtkSmartPointer<vtkPolyData> meshIn); // remove

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
    typename Image::ImageType::PointType origin_;
    bool use_origin;
    std::string out_prefix_;
    bool output_enabled_ = true;
    bool usePairwiseNormalsDifferencesForGoodBad_ = false;
    bool mean_before_warp_enabled_ = true;
};

// #include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

#endif // !__RECONSTRUCTION_H__
}