#pragma once

#include "Procrustes3D.h"
#include "Image.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "itkThinPlateSplineKernelTransform2.h"
#include "itkCompactlySupportedRBFSparseKernelTransform.h"

#include <itkAddImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMultiplyImageFilter.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageDuplicator.h>

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

    using AddImageFilter = itk::AddImageFilter<Image::ImageType, Image::ImageType>;
    using MultiplyByConstantImageFilter = itk::MultiplyImageFilter<Image::ImageType, Image::ImageType, Image::ImageType>;
    using ImageDuplicator = itk::ImageDuplicator<Image::ImageType>;
    using PointArray = std::vector<Point3>;

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

    void setMeanBeforeWarpEnabled(bool enabled);

    vtkSmartPointer<vtkPolyData> getMesh(PointArrayType local_pts);
    void readMeanInfo(std::string dense, std::string sparse, std::string goodPoints); // remove
    bool sparseDone();
    bool denseDone();
    void writeMeanInfo(std::string nameBase);

    vtkSmartPointer<vtkPoints>   SparseMean(){return sparseMean_;}
    vtkSmartPointer<vtkPolyData> DenseMean() {return denseMean_;}

    std::vector<bool> GoodPoints(){return goodPoints_;}

    std::string OutPrefix(){return out_prefix_;}
    void setOutPrefix(std::string out_prefix){out_prefix_ = out_prefix;}

    std::vector< PointArrayType >  computeSparseMean(std::vector< PointArrayType > local_pts, // consolidate
                                                     itk::Point<double>& common_center,
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