
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "Params/InputParams.h"
#include "Reconstruction.h"
#include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

#include <type_traits>  // for condition typedefs


template < template < typename TCoordRep, unsigned > class TTransformType,
           template < typename ImageType, typename TCoordRep > class TInterpolatorType,
           typename TCoordRep, typename PixelType, typename ImageType>
int DoIt(InputParams params)
{
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    typedef itk::ImageFileWriter< ImageType >   WriterType;

    Reconstruction < TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType> reconstructor(0.5,60);
    reconstructor.reset();
}


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
  const int Dimension = 3;
  typedef float      PixelType;
  typedef double CoordinateRepType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  //------------- end typedefs ---------------

  int status;
  if(params.use_tps_transform){
      if(params.use_bspline_interpolation){
          status = DoIt<itk::ThinPlateSplineKernelTransform2,
                  itk::BSplineInterpolateImageFunctionWithDoubleCoefficents,
                  CoordinateRepType, PixelType, ImageType>(params);
      }
      else{
          status = DoIt<itk::ThinPlateSplineKernelTransform2,
                  itk::LinearInterpolateImageFunction,
                  CoordinateRepType, PixelType, ImageType>(params);
      }
  }
  else {
      if(params.use_bspline_interpolation){
          status = DoIt<itk::CompactlySupportedRBFSparseKernelTransform,
                  itk::BSplineInterpolateImageFunctionWithDoubleCoefficents,
                  CoordinateRepType, PixelType, ImageType>(params);
      }
      else{
          status = DoIt<itk::CompactlySupportedRBFSparseKernelTransform,
                  itk::LinearInterpolateImageFunction,
                  CoordinateRepType, PixelType, ImageType>(params);
      }
  }

//  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> reconstructor(0.5, 60.);
//  reconstructor.reset();

//  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> &reconstructor_ref(reconstructor);
//  reconstructor_ref.reset();

//  Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> *reconstructor_ptr = new Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction>(0.5, 60.);
//  reconstructor_ptr->reset();

//  typedef Reconstruction<itk::CompactlySupportedRBFSparseKernelTransform, itk::LinearInterpolateImageFunction> myReconstruction;

//  {
//    myReconstruction reconstructor(0.5, 60.);
//    reconstructor.reset();

//    myReconstruction &reconstructor_ref(reconstructor);
//    reconstructor_ref.reset();

//    myReconstruction *reconstructor_ptr = new myReconstruction(0.5, 60.);
//    reconstructor_ptr->reset();
//  }

  return 0;
}
