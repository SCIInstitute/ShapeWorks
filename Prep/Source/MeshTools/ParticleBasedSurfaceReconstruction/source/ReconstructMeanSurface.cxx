

/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2019/08/30 17:10:25 $
  Version:   $Revision: 1.1 $
  Author:    $Author: elhabian $

  Purpose:   particle-based mean dense surface reconstruction,
                (1) generalized procrustes alignment of a correspondence model (from ShapeWorksRun)
                (2) save aligned shapes for subsequent statistical analysis
                (3) compute mean sparse shape (procrustes mean)
                (4) define non-rigid warping function that would map the distance transform of each subject (in subject space) to the mean space
                (5) construct a dense mean shape (i.e. mesh) (just isosurface with vtk-based smoothing and decimation)
                (6) mesh quality control using preview (mesh relaxation + mesh decimation)

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
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    typedef itk::ImageFileWriter< ImageType >   WriterType;
    typedef Reconstruction < TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType> ReconstructionType;
    typedef typename ReconstructionType::PointType                                               PointType;
    typedef typename ReconstructionType::PointArrayType                                          PointArrayType;

    double maxAngleDegrees   = params.normalAngle *(180.0 / params.pi);

    ReconstructionType reconstructor(params.out_prefix,
                                     params.qcDecimationPercentage, maxAngleDegrees, params.K,
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
    std::cout << "Computing mean sparse shape .... \n ";
    if(params.worldPointsFilenames.size() == 0)
    {
        // if no world points are given, they will be estimated using procrustes
        PointType commonCenter;
        global_pts = reconstructor.computeSparseMean(local_pts, commonCenter,
                                                     params.do_procrustes, params.do_procrustes_scaling);
    }
    else
    {
        // finding image origin that is consistent with the given world coordinates
        typename ImageType::PointType  origin; origin[0] = 0; origin[1] = 0; origin[2] = 0;
        double min_x = 1e10,  min_y = 1e10,  min_z = 1e10;
        double max_x = -1e10, max_y = -1e10, max_z = -1e10;

        // read given world points
        for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
        {
            std::cout << "Loading world points file: " << params.worldPointsFilenames[shapeNo].c_str() << std::endl;

            PointArrayType curShape;
            Utils::readSparseShape(curShape, const_cast<char*> (params.worldPointsFilenames[shapeNo].c_str()));

            PointType cur_origin; cur_origin[0] = 0; cur_origin[1] = 0; cur_origin[2] = 0;
            for(unsigned int ii = 0; ii < curShape.size(); ii++)
            {
                cur_origin[0] += curShape[ii][0];
                cur_origin[1] += curShape[ii][1];
                cur_origin[2] += curShape[ii][2];

                if(curShape[ii][0] > max_x)
                    max_x = curShape[ii][0];

                if(curShape[ii][1] > max_y)
                    max_y = curShape[ii][1];

                if(curShape[ii][2] > max_z)
                    max_z = curShape[ii][2];

                if(curShape[ii][0] < min_x)
                    min_x = curShape[ii][0];

                if(curShape[ii][1] < min_y)
                    min_y = curShape[ii][1];

                if(curShape[ii][2] < min_z)
                    min_z = curShape[ii][2];
            }

            cur_origin[0] /= double(curShape.size());
            cur_origin[1] /= double(curShape.size());
            cur_origin[2] /= double(curShape.size());

            origin[0] += cur_origin[0];
            origin[1] += cur_origin[1];
            origin[2] += cur_origin[2];

            global_pts.push_back(curShape);
        }

        origin[0] /= double(global_pts.size());
        origin[1] /= double(global_pts.size());
        origin[2] /= double(global_pts.size());

        std::cout << "origin(0) = " << origin[0] << ", "
                  << "origin(1) = " << origin[1] << ", "
                  << "origin(2) = " << origin[2] << std::endl;

        std::cout << "min_x = " << min_x << ", "
                  << "min_y = " << min_y << ", "
                  << "min_z = " << min_z << std::endl;

        double x_width = max_x - min_x;
        double y_width = max_y - min_y;
        double z_width = max_z - min_z;

        origin[0] = origin[0] - (x_width/2.0);
        origin[1] = origin[1] - (y_width/2.0);
        origin[2] = origin[2] - (z_width/2.0);

        std::cout << "origin(0) = " << origin[0] << ", "
                  << "origin(1) = " << origin[1] << ", "
                  << "origin(2) = " << origin[2] << std::endl;

        reconstructor.setOrigin(origin);
    }

    // write global points to be use for pca modes
    for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
    {
       std::string curfilename = params.out_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + ".global.pts";
       Utils::writeSparseShape((char*)curfilename.c_str(), global_pts[shapeNo]);
    }

    if(params.display)
        Vis::visParticles(reconstructor.SparseMean(),params.glyph_radius,std::string("Mean Sparse Shape"));

    // compute the dense shape
    std::cout << "Reconstructing dense mean mesh with number of clusters = " << params.K << std::endl;
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
