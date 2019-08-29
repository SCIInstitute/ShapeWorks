
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "Params/InputParams.h"
#include "Vis.h"
#include "Reconstruction.h"
#include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

#include "itkNrrdImageIOFactory.h"
#include "itkMetaImageIOFactory.h"

template < template < typename TCoordRep, unsigned > class TTransformType,
           template < typename ImageType, typename TCoordRep > class TInterpolatorType,
           typename TCoordRep, typename PixelType, typename ImageType>
int DoIt(InputParams params)
{
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    typedef itk::ImageFileWriter< ImageType >   WriterType;
    typedef Reconstruction < TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType> ReconstructionType;
    typedef typename ReconstructionType::PointArrayType                                          PointArrayType;

    // TODO: the below parameters should be obtained from the input ones with good enough defaults
    float decimationPercent  = 0.3f;
    double maxAngleDegrees   = 45.0;
    size_t numClusters       = 5;

    ReconstructionType reconstructor(decimationPercent, maxAngleDegrees, numClusters);
    reconstructor.reset();

    // read local points and world points if given
    std::vector< PointArrayType > local_pts;  local_pts.clear();
    std::vector< PointArrayType > global_pts; global_pts.clear();

    // read local points
    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
    {
        std::cout << "Loading local points file: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;

        PointArrayType curShape;
        Utils::readSparseShape(curShape, const_cast<char*> (params.localPointsFilenames[shapeNo].c_str()));

        local_pts.push_back(curShape);
    }


    // define mean sparse shape -- this is considered as target points in the warp
    if(params.worldPointsFilenames.size() == 0)
    {
        // if no world points are given, they will be estimated using procrustes
        std::cout << "Computing mean sparse shape .... \n ";
        global_pts = reconstructor.computeSparseMean(local_pts, params.do_procrustes, params.do_procrustes_scaling);
    }
    else
    {
        std::cout << "Computing mean sparse shape .... \n ";
        global_pts = reconstructor.computeSparseMean(local_pts, false, false);
        global_pts.clear(); // clear

        // read given world points
        for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
        {
            std::cout << "Loading world points file: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;

            PointArrayType curShape;
            Utils::readSparseShape(curShape, const_cast<char*> (params.worldPointsFilenames[shapeNo].c_str()));

            global_pts.push_back(curShape);
        }
    }

    if(params.display)
        Vis::visParticles(reconstructor.SparseMean(),params.glyph_radius,std::string("Mean Sparse Shape"));

    // read distance transforms
    std::vector<typename ImageType::Pointer> distance_transforms;
    for (unsigned int shapeNo = 0; shapeNo < params.distanceTransformFilenames.size(); shapeNo++)
    {
        std::string filename = params.distanceTransformFilenames[shapeNo];

        if (filename.find(".nrrd") != std::string::npos) {
            itk::NrrdImageIOFactory::RegisterOneFactory();
        } else if (filename.find(".mha") != std::string::npos) {
            itk::MetaImageIOFactory::RegisterOneFactory();
        }
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( filename.c_str() );
        reader->Update();
        distance_transforms.push_back(reader->GetOutput());
    }

    // compute the dense shape
    vtkSmartPointer<vtkPolyData> denseMean = reconstructor.getDenseMean(local_pts, global_pts, distance_transforms);
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
