
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
    typedef typename ReconstructionType::PointType                                               PointType;
    typedef typename ReconstructionType::PointArrayType                                          PointArrayType;

    // TODO: the below parameters should be obtained from the input ones with good enough defaults
    double maxAngleDegrees   = params.normalAngle *(180.0 / params.pi);

    ReconstructionType reconstructor(params.qcDecimationPercentage, maxAngleDegrees, params.K,
                                     params.qcFixWinding,
                                     params.qcDoLaplacianSmoothingBeforeDecimation,
                                     params.qcDoLaplacianSmoothingAfterDecimation,
                                     params.qcSmoothingLambda, params.qcSmoothingIterations);
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
        std::cout << "Reading distance transform file : " << filename << std::endl;
        reader->SetFileName( filename.c_str() );
        reader->Update();
        distance_transforms.push_back(reader->GetOutput());
    }

    // define mean sparse shape -- this is considered as target points in the warp
    PointType commonCenter;
    if(params.worldPointsFilenames.size() == 0)
    {
        // if no world points are given, they will be estimated using procrustes
        std::cout << "Computing mean sparse shape .... \n ";
        global_pts = reconstructor.computeSparseMean(local_pts, commonCenter,
                                                     params.do_procrustes, params.do_procrustes_scaling);
    }
    else
    {
        std::cout << "Computing mean sparse shape .... \n ";
        global_pts = reconstructor.computeSparseMean(local_pts, commonCenter, false, false);
        global_pts.clear(); // clear

        // read given world points
        for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
        {
            std::cout << "Loading world points file: " << params.worldPointsFilenames[shapeNo].c_str() << std::endl;

            PointArrayType curShape;
            Utils::readSparseShape(curShape, const_cast<char*> (params.worldPointsFilenames[shapeNo].c_str()));

            for(unsigned int ii = 0; ii < curShape.size(); ii++)
            {
                curShape[ii][0] -= commonCenter[0];
                curShape[ii][1] -= commonCenter[1];
                curShape[ii][2] -= commonCenter[2];
            }

            global_pts.push_back(curShape);
        }
    }

    if(params.display)
        Vis::visParticles(reconstructor.SparseMean(),params.glyph_radius,std::string("Mean Sparse Shape"));

    // compute the dense shape
    vtkSmartPointer<vtkPolyData> denseMean = reconstructor.getDenseMean(local_pts, global_pts, distance_transforms);

    // write output
    reconstructor.writeMeanInfo(params.out_prefix);

    if(params.display)
    {
        std::vector<bool> goodPoints          = reconstructor.GoodPoints();
        vtkSmartPointer<vtkPoints> sparseMean = reconstructor.SparseMean();

        // now fill vtkpoints with the two lists to visualize
        vtkSmartPointer<vtkPoints> bad_ones  = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> good_ones = vtkSmartPointer<vtkPoints>::New();
        for(unsigned int kk = 0 ; kk < goodPoints.size(); kk++)
        {
            double p[3];
            sparseMean->GetPoint(kk, p);
            if (goodPoints[kk])
                good_ones->InsertNextPoint(p);
            else
                bad_ones->InsertNextPoint(p);
        }

        // now visualize
        Vis::visParticles(bad_ones, good_ones,params.glyph_radius, std::string("bad particles in red and good ones are in green")); // bad in red and good in green
        Vis::visMeshWithParticles(denseMean, sparseMean, params.glyph_radius, std::string("dense shape in red and mean sparse shape in white"));
    }

    return 0;
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

  return 0;
}
