
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "Params/InputParams.h"
#include "Reconstruction.h"
#include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

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
  //ctc: just commenting these lines out and passing "foo" as first arg in order to test instantiations below
  // params.readParams(argv[1], 3); // 3 - WarpToMeanSpaceWithPreviewMeshQC
  //std::cout << "Number of input sparse shapes: " << params.localPointsFilenames.size() << std::endl;

  //------------- typedefs ---------------
  const int Dimension = 3;
  typedef float      PixelType;
  typedef double CoordinateRepType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;
  typedef itk::ImageFileWriter< ImageType >   WriterType;

  // transformation
  typedef itk::CompactlySupportedRBFSparseKernelTransform < CoordinateRepType,Dimension>     RBFTransformType;
  typedef itk::ThinPlateSplineKernelTransform2< CoordinateRepType,Dimension>                 ThinPlateSplineType;

  // interpolation
  typedef itk::LinearInterpolateImageFunction<ImageType, double >          LinearInterpolatorType;
  typedef itk::BSplineInterpolateImageFunction<ImageType, double, double > BSplineInterpolatorType;

  //------------- end typedefs ---------------

  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> reconstructor(0.5, 60.);
  reconstructor.reset();

  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> &reconstructor_ref(reconstructor);
  reconstructor_ref.reset();

  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> *reconstructor_ptr = new Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction>(0.5, 60.);
  reconstructor_ptr->reset();

  typedef Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> myReconstruction;

  {
    myReconstruction reconstructor(0.5, 60.);
    reconstructor.reset();

    myReconstruction &reconstructor_ref(reconstructor);
    reconstructor_ref.reset();

    myReconstruction *reconstructor_ptr = new myReconstruction(0.5, 60.);
    reconstructor_ptr->reset();
  }

  return 0;
}
