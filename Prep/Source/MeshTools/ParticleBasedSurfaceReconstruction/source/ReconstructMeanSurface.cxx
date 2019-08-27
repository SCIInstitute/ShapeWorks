
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "Params/InputParams.h"
#include "Reconstruction.h"

#include <type_traits>  // for condition typedefs
int main( int argc , char* argv[] )
{
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    InputParams params;
    params.readParams(argv[1], 3); // 3 - WarpToMeanSpaceWithPreviewMeshQC

    std::cout << "Number of input sparse shapes: " << params.localPointsFilenames.size() << std::endl;

    //------------- typedefs ---------------
    const  int Dimension = 3;
    typedef float      PixelType;

    typedef itk::Image< PixelType, Dimension >  ImageType;
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;

    // transformation
    typedef   double CoordinateRepType;
    typedef   itk::CompactlySupportedRBFSparseKernelTransform < CoordinateRepType,Dimension>     RBFTransformType;
    typedef   itk::ThinPlateSplineKernelTransform2< CoordinateRepType,Dimension>                 ThinPlateSplineType;

    // interpolation
    typedef itk::LinearInterpolateImageFunction<ImageType, double >          LinearInterpolatorType;
    typedef itk::BSplineInterpolateImageFunction<ImageType, double, double > BSplineInterpolatorType;

    //------------- end typedefs ---------------

    //Reconstruction reconstructor;

    return 0;
}
