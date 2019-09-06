

/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2019/08/30 17:10:25 $
  Version:   $Revision: 1.1 $
  Author:    $Author: elhabian $
  Purpose:   particle-based subject-specific dense surface reconstruction, (use output of ReconstructMeanSurface)
             warp the mean dense shape (mesh) constructed from WarpToMeanSpace to each individual subject space using the sparse correspondence model from ShapeWorksRun

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


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

    typedef Reconstruction < TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType> ReconstructionType;
    typedef typename ReconstructionType::PointArrayType                                          PointArrayType;

    double maxAngleDegrees   = params.normalAngle *(180.0 / params.pi);

    ReconstructionType reconstructor;

    std::string denseFilename      = params.mean_prefix + "_dense.vtk";
    std::string sparseFilename     = params.mean_prefix + "_sparse.particles";
    std::string goodPointsFilename = params.mean_prefix + "_goodPoints.txt";

    std::cout << "denseFilename: "  << denseFilename << std::endl;
    std::cout << "sparseFilename: " << sparseFilename << std::endl;
    std::cout << "goodPointsFilename: " << goodPointsFilename << std::endl;

    reconstructor.readMeanInfo(denseFilename, sparseFilename, goodPointsFilename);

    if(params.display)
    {
        vtkSmartPointer<vtkPolyData> denseMean = reconstructor.DenseMean();
        vtkSmartPointer<vtkPoints> sparseMean  = reconstructor.SparseMean();

        Vis::visParticles(sparseMean,params.glyph_radius,std::string("mean sparse shape"));
        Vis::visMesh(denseMean, "mean dense shape");
    }

    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
    {
        std::string basename = Utils::getFilename(params.localPointsFilenames[shapeNo]);
        std::cout << "Processing: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;

        PointArrayType curSparse;
        Utils::readSparseShape(curSparse, const_cast<char*> (params.localPointsFilenames[shapeNo].c_str()));

        vtkSmartPointer<vtkPolyData> curDense = reconstructor.getMesh(curSparse);

        std::string outfilename = params.out_path + '/'+ Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_dense.vtk";
        std::cout << "Writing: " << outfilename << std::endl;

        vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
        writer->SetFileName(outfilename.c_str());
        writer->SetInputData(curDense);
        writer->Update();

        vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
        vertices->DeepCopy( curDense->GetPoints() );

        std::string ptsfilename = params.out_path + '/'+ Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_dense.particles";
        Utils::writeSparseShape((char*) ptsfilename.c_str(), vertices);

        vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
        Utils::readSparseShape(curSparse_, const_cast<char*> (params.localPointsFilenames[shapeNo].c_str()));

        ptsfilename = params.out_path + '/'+ Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_sparse.particles";
        Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);

        if(params.display)
        {
            Vis::visMeshWithParticles(curDense, curSparse_, params.glyph_radius,
                                      std::string(basename + ": dense shape in red and subject sparse shape in white"));
        }
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
  params.readParams(argv[1], 1); // 1 - WarpToSubjectSpace
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
