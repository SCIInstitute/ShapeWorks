

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

    if(params.worldPointsFilenames.size() == 0)
    {
        // if no world points are given, they will be estimated using procrustes
        // define mean sparse shape -- this is considered as target points in the warp
        std::cout << "Computing mean sparse shape .... \n ";

        PointType commonCenter;
        global_pts = reconstructor.computeSparseMean(local_pts, commonCenter,
                                                     params.do_procrustes, params.do_procrustes_scaling);
    }
    else
    {
        // read given world points
        for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
        {
            std::cout << "Loading world points file: " << params.worldPointsFilenames[shapeNo].c_str() << std::endl;

            PointArrayType curShape;
            Utils::readSparseShape(curShape, const_cast<char*> (params.worldPointsFilenames[shapeNo].c_str()));

            global_pts.push_back(curShape);
        }

        // finding image origin that is consistent with the given world coordinates and adjusted using the origin of images and point clouds in the local space
        typename ImageType::PointType  origin_local;
        typename ImageType::PointType  origin_global;

        // the bounding box of the local points
        double min_x_local, min_y_local, min_z_local;
        double max_x_local, max_y_local, max_z_local;

        // the bounding box of the global points
        double min_x_global, min_y_global, min_z_global;
        double max_x_global, max_y_global, max_z_global;

        // compute the center of mass for both local and global points
        Utils::computeCenterOfMassForShapeEnsemble(local_pts,  origin_local);
        Utils::computeCenterOfMassForShapeEnsemble(global_pts, origin_global);

        std::cout << "Center of mass of local points:" << std::endl;
        std::cout << "origin_local(0) = " << origin_local[0] << ", "
                  << "origin_local(1) = " << origin_local[1] << ", "
                  << "origin_local(2) = " << origin_local[2] << std::endl;

        std::cout << "Center of mass of global points:" << std::endl;
        std::cout << "origin_global(0) = " << origin_global[0] << ", "
                  << "origin_global(1) = " << origin_global[1] << ", "
                  << "origin_global(2) = " << origin_global[2] << std::endl;

        // find the bounding box of both local and global points
        Utils::getBoundingBoxForShapeEnsemble(local_pts,
                                              min_x_local, min_y_local, min_z_local,
                                              max_x_local, max_y_local, max_z_local);

        std::cout << "Local points bounding box:" << std::endl;
        std::cout << "min_x_local = " << min_x_local << ", "
                  << "min_y_local = " << min_y_local << ", "
                  << "min_z_local = " << min_z_local << std::endl;

        std::cout << "max_x_local = " << max_x_local << ", "
                  << "max_y_local = " << max_y_local << ", "
                  << "max_z_local = " << max_z_local << std::endl;

        Utils::getBoundingBoxForShapeEnsemble(global_pts,
                                              min_x_global, min_y_global, min_z_global,
                                              max_x_global, max_y_global, max_z_global);

        std::cout << "Global points bounding box:" << std::endl;
        std::cout << "min_x_global = " << min_x_global << ", "
                  << "min_y_global = " << min_y_global << ", "
                  << "min_z_global = " << min_z_global << std::endl;

        std::cout << "max_x_global = " << max_x_global << ", "
                  << "max_y_global = " << max_y_global << ", "
                  << "max_z_global = " << max_z_global << std::endl;

        // compute the image origin (corner) based on the center of mass
        double x_width_local = max_x_local - min_x_local;
        double y_width_local = max_y_local - min_y_local;
        double z_width_local = max_z_local - min_z_local;

        double x_width_global = max_x_global - min_x_global;
        double y_width_global = max_y_global - min_y_global;
        double z_width_global = max_z_global - min_z_global;

        origin_local[0] = origin_local[0] - (x_width_local/2.0);
        origin_local[1] = origin_local[1] - (y_width_local/2.0);
        origin_local[2] = origin_local[2] - (z_width_local/2.0);

        origin_global[0] = origin_global[0] - (x_width_global/2.0);
        origin_global[1] = origin_global[1] - (y_width_global/2.0);
        origin_global[2] = origin_global[2] - (z_width_global/2.0);

        std::cout << "Image origin (corner) for local points: " << std::endl;
        std::cout << "origin_local(0) = " << origin_local[0] << ", "
                  << "origin_local(1) = " << origin_local[1] << ", "
                  << "origin_local(2) = " << origin_local[2] << std::endl;

        std::cout << "Image origin (corner) for global points: " << std::endl;
        std::cout << "origin_global(0) = " << origin_global[0] << ", "
                  << "origin_global(1) = " << origin_global[1] << ", "
                  << "origin_global(2) = " << origin_global[2] << std::endl;

        // origin of the distance transforms (assume all dts are sharing the same origin)
        typename ImageType::PointType  origin_dt = distance_transforms[0]->GetOrigin();

        double offset_x = origin_dt[0] - origin_local[0];
        double offset_y = origin_dt[1] - origin_local[1];
        double offset_z = origin_dt[2] - origin_local[2];

        // adjust global origin based on local offset
        origin_global[0] = origin_global[0] + offset_x;
        origin_global[1] = origin_global[1] + offset_y;
        origin_global[2] = origin_global[2] + offset_z;

        std::cout << "Image origin (corner) for global points - adjusted: " << std::endl;
        std::cout << "origin_global(0) = " << origin_global[0] << ", "
                  << "origin_global(1) = " << origin_global[1] << ", "
                  << "origin_global(2) = " << origin_global[2] << std::endl;

        reconstructor.setOrigin(origin_global);
    }

    // write global points to be use for pca modes and also local points
    int mkdirStatus;
    std::string global_points_path = params.out_path + "/global_particles";
    std::string local_points_path = params.out_path + "/local_particles";
#ifdef WIN32
            mkdirStatus = _mkdir(global_points_path.c_str());
            mkdirStatus = _mkdir(local_points_path.c_str());
#else
            mkdirStatus = mkdir(global_points_path.c_str(), S_IRWXU);
            mkdirStatus = mkdir(local_points_path.c_str(), S_IRWXU);
#endif
    for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
    {
       std::string curfilename = global_points_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_global.particles";
       Utils::writeSparseShape((char*)curfilename.c_str(), global_pts[shapeNo]);

       std::string curfilename_local = local_points_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_local.particles";
       Utils::writeSparseShape((char*)curfilename_local.c_str(), local_pts[shapeNo]);
    }

    if(params.display)
        Vis::visParticles(reconstructor.SparseMean(),params.glyph_radius,std::string("Mean Sparse Shape"));

    // compute the dense shape
    std::cout << "Reconstructing dense mean mesh with number of clusters = " << params.K << std::endl;
    vtkSmartPointer<vtkPolyData> denseMean = reconstructor.getDenseMean(local_pts, global_pts, distance_transforms);

    // write output
    reconstructor.writeMeanInfo(params.out_prefix);

    // write out good and bad particles for each subject file
    std::string local_good_bad_path = params.out_path + "/local_good_bad";
    std::string global_good_bad_path = params.out_path + "/global_good_bad";
#ifdef WIN32
            mkdirStatus = _mkdir(local_good_bad_path.c_str());
            mkdirStatus = _mkdir(global_good_bad_path.c_str());
#else
            mkdirStatus = mkdir(local_good_bad_path.c_str(), S_IRWXU);
            mkdirStatus = mkdir(global_good_bad_path.c_str(), S_IRWXU);
#endif

    std::vector<bool> goodPoints          = reconstructor.GoodPoints();
    for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
    {
        std::string outfilenameGood;
        std::string outfilenameBad;
        std::ofstream ofsG, ofsB;

        outfilenameGood = local_good_bad_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_local-good.particles";
        outfilenameBad  = local_good_bad_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_local-bad.particles";

        ofsG.open(outfilenameGood.c_str());
        ofsB.open(outfilenameBad.c_str());

        PointArrayType curShape_local = local_pts[shapeNo];
        for (unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
        {
            auto pt = curShape_local[ii];
            if(goodPoints[ii])
                ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
            else
                ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
        }
        ofsG.close();
        ofsB.close();

        outfilenameGood = global_good_bad_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_global-good.particles";
        outfilenameBad  = global_good_bad_path + "/" + Utils::removeExtension(Utils::getFilename(params.localPointsFilenames[shapeNo])) + "_global-bad.particles";

        ofsG.open(outfilenameGood.c_str());
        ofsB.open(outfilenameBad.c_str());

        PointArrayType curShape_global = global_pts[shapeNo];
        for (unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
        {
            auto pt = curShape_global[ii];
            if(goodPoints[ii])
                ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
            else
                ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
        }
        ofsG.close();
        ofsB.close();
    }

    if(params.display)
    {
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
