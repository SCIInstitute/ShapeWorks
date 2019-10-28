
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2015/11/21 17:10:25 $
  Version:   $Revision: 1.1 $
  Author:    $Author: elhabian $
  Purpose:   particle-based subject-specific dense surface reconstruction, (use output of WarpToMeanSpace)
             warp the mean dense shape (mesh) constructed from WarpToMeanSpace to each individual subject space using the sparse correspondence model from ShapeWorksRun

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>

//#define VIS_DEBUG
//#define USE_TPS

//-------------- adapted ITK filters --------------------------
#include "Transforms/itkThinPlateSplineKernelTransform2.h"
#include "Transforms/itkCompactlySupportedRBFSparseKernelTransform.h"

//------------- VTK ---------------------
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPLYWriter.h>

//#include <vtkIdList.h>
#include <vtkKdTreePointLocator.h>
#include <vtkKdTree.h>
#include <vtkDijkstraGraphGeodesicPath.h>

//--------- our stuff ----------------
#include "ParticleBasedSurfaceReconstruction_InputParams.h"
#include "Utils/utils.h"
#include "Utils/vis.h"

using namespace std;

//------------- typedefs ---------------
const unsigned int Dimension = 3;

// warping ----------
typedef   double CoordinateRepType;
typedef   itk::CompactlySupportedRBFSparseKernelTransform < CoordinateRepType,Dimension>     RBFTransformType;
typedef   itk::ThinPlateSplineKernelTransform2< CoordinateRepType,Dimension>  ThinPlateSplineType;
typedef   itk::Point< CoordinateRepType,Dimension >  PointType;
typedef   std::vector< PointType >                   PointArrayType;
#ifdef USE_TPS
typedef   ThinPlateSplineType::PointSetType          PointSetType;
#else
typedef   RBFTransformType::PointSetType             PointSetType;
#endif
typedef   PointSetType::Pointer                      PointSetPointer;
typedef   PointSetType::PointIdentifier              PointIdType;


//------------------------ auxillary functions ---------------------------------------------------
//------------------------debugging -------------------------------
#ifdef USE_TPS
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  ThinPlateSplineType::Pointer tpsTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double &maxmDist);

void generateWarpedMeshes( ThinPlateSplineType::Pointer transform,
                           vtkSmartPointer<vtkPolyData> &outputMesh );
#else
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  RBFTransformType::Pointer rbfTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double &maxmDist);

void generateWarpedMeshes( RBFTransformType::Pointer transform,
                           vtkSmartPointer<vtkPolyData> &outputMesh );

#endif

//-------------------------------------------------------------------------------------------
double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points, std::vector<int> particles_indices);
//-------------------------------------------------------------------------------------------

int main( int argc , char* argv[] )
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }
    
    InputParams params;
    params.readParams(argv[1], 1); // 1 - WarpToSubjectSpace
    
    std::cout << "Number of input sparse shapes: " << params.localPointsFilenames.size() << std::endl;
    
    //----------------------------------- let the fun begins --------------------------
    
    // (1) define mean sparse shape -- this is considered as source points in the warp
    std::string meanShape_filename = params.mean_prefix + ".meanSparseShape.lpts" ;
    std::cout << "reading mean sparse shape:" << meanShape_filename << std::endl;
    vtkSmartPointer< vtkPoints > meanSparseShape = utils::readSparseShape((char*) meanShape_filename.c_str());

    // define mean dense shape to be warped
    std::string meanDense_filename = params.mean_prefix + ".meanDenseShape.vtk" ;
    std::cout << "reading mean dense shape:" << meanDense_filename << std::endl;
    vtkPolyDataReader * polyreader =   vtkPolyDataReader::New();
    polyreader->SetFileName(meanDense_filename.c_str());
    polyreader->Update();
    vtkSmartPointer<vtkPolyData> meanDenseShape = polyreader->GetOutput();

#ifdef VIS_DEBUG
    if(params.display)
    {
        visParticles(meanSparseShape,params.glyph_radius,std::string("mean sparse shape"));
        visMesh(meanDenseShape, "mean dense shape");
    }
#endif

    // (2) get the good/bad particles detected via WarpToMeanSpace
    std::cout << "reading bad particle indices ...\n";
    string badfilename       = params.mean_prefix + ".BadParticleIds.txt";
    std::vector<int> bad_ids = utils::readParticleIds((char*) badfilename.c_str());

    std::cout << "reading good particle indices ...\n";
    string goodfilename       = params.mean_prefix + ".GoodParticleIds.txt";
    std::vector<int> good_ids = utils::readParticleIds((char*) goodfilename.c_str());

    std::cout << "number of bad particles : " << bad_ids.size() << std::endl;
    std::cout << "bad particle indices...\n";
    for(unsigned int i = 0; i < bad_ids.size(); i++)
        std::cout << bad_ids[i] << ", ";
    std::cout << std::endl;

#ifdef VIS_DEBUG
    // now fill vtkpoints with the two lists to visualize
    vtkSmartPointer<vtkPoints> bad_ones  = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> good_ones = vtkSmartPointer<vtkPoints>::New();
    for(unsigned int kk = 0 ; kk < bad_ids.size(); kk++)
    {
        double p[3];
        meanSparseShape->GetPoint(bad_ids[kk], p);
        bad_ones->InsertNextPoint(p);
    }
    for(unsigned int kk = 0 ; kk < good_ids.size(); kk++)
    {
        double p[3];
        meanSparseShape->GetPoint(good_ids[kk], p);
        good_ones->InsertNextPoint(p);
    }

    // now visualize
    if (params.display)
        visParticles(bad_ones, good_ones,params.glyph_radius, std::string("bad particles in red and good ones are in green")); // bad in red and good in green
#endif

    std::cout << "number of bad correspondences (will be excluded): " << bad_ids.size() << " out of " << params.number_of_particles << std::endl;
    

    // decide which correspondences will be used to build the warp, may implement a ranking process for the good ones to choose the best subset to build the warp
    std::vector<int> particles_indices;
    particles_indices.clear();
    for(unsigned int kk = 0 ; kk < good_ids.size(); kk++)
        particles_indices.push_back(good_ids[kk]);

#ifndef USE_TPS
    // compute the rbf kernel width
    double averageDistToNeighbors = computeAverageDistanceToNeighbors(meanSparseShape,  particles_indices);
    std::cout << "averageDistToNeighbors: " << averageDistToNeighbors << std::endl;

    double sigma = averageDistToNeighbors;
    std::cout << "sigma: " << sigma << std::endl;
#endif

    // (3) set up the common source points for all warps

    // Landmarks correspondances may be associated with the SplineKernelTransforms
    // via Point Set containers. Let us define containers for the landmarks.
    PointIdType id;

    // NOTE that, since we are not resampling images, this warping is a forward warping from mean space to subject space
    // Define container for source landmarks that corresponds to the mean space, this is
    // the moving mesh which will be warped to each individual subject
    PointSetType::Pointer sourceLandMarks  = PointSetType::New();
    PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
    PointType ps;
    id = itk::NumericTraits< PointIdType >::Zero;
    int ns = 0;
    for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
    {
        if (std::find(particles_indices.begin(), particles_indices.end(), ii)!= particles_indices.end())
        {
            double p[3];
            meanSparseShape->GetPoint(ii,p);
            ps[0] = p[0];
            ps[1] = p[1];
            ps[2] = p[2];
            sourceLandMarkContainer->InsertElement( id++, ps );
            ns++;
        }
    }
    std::cout << "number of source particles: " << ns << std::endl;

#ifndef USE_TPS
        RBFTransformType::Pointer rbfTransform = RBFTransformType::New();
        rbfTransform->SetSigma(sigma); // smaller means more sparse
        //rbfTransform->SetStiffness(0.25*sigma);
        rbfTransform->SetStiffness(1e-10);
#else
        ThinPlateSplineType::Pointer tpsTransform = ThinPlateSplineType::New();
        tpsTransform->SetStiffness( 1e-10 );
#endif

        std::cout << "setting source landmarks ..." << std::endl ;
#ifndef USE_TPS
        rbfTransform->SetSourceLandmarks(sourceLandMarks);
#else
        tpsTransform->SetSourceLandmarks(sourceLandMarks);
#endif

    // (4) build individual warps from the mean space to each subject space
    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
    {
        std::string basename = utils::getFilename(params.localPointsFilenames[shapeNo]);

        std::cout << "Processing: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;

        // load the original local points to be aligned to the mean sparse
        std::cout << "loading particles ..." << std::endl ;
        vtkSmartPointer< vtkPoints > subjectPts = utils::readSparseShape((char*) params.localPointsFilenames[shapeNo].c_str(),
                                                                        params.number_of_particles);

#ifdef VIS_DEBUG
        if(params.display)
            visParticles(meanSparseShape, subjectPts, params.glyph_radius, std::string(basename + ": mean sparse shape in red and subject particles in green"));
#endif

        // Define container for target landmarks corresponds to the subject shape
        PointSetType::Pointer targetLandMarks = PointSetType::New();
        PointType pt;
        PointSetType::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();
        id   = itk::NumericTraits< PointIdType >::Zero;
        int nt = 0;
        for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
        {
            if (std::find(particles_indices.begin(), particles_indices.end(), ii)!= particles_indices.end())
            {
                double p[3];
                subjectPts->GetPoint(ii,p);
                pt[0] = p[0];
                pt[1] = p[1];
                pt[2] = p[2];
                targetLandMarkContainer->InsertElement( id++, pt );
                nt++;
            }
        }
        std::cout << "number of target particles: " << nt << std::endl;
        
        std::cout << "setting target landmarks ..." << std::endl ;

#ifndef USE_TPS
        rbfTransform->SetTargetLandmarks(targetLandMarks);
#else
        tpsTransform->SetTargetLandmarks(targetLandMarks);
#endif
        
        std::cout << "done setting target landmarks ..." << std::endl ;
        
        // check the mapping (inverse here)
        // this means source points (current sample's space) should be warped to the target (mean space)
        vtkSmartPointer<vtkPoints> mappedCorrespondences = vtkSmartPointer<vtkPoints>::New();
        double rms;
        double rms_wo_mapping;
        double maxmDist;
#ifndef USE_TPS
        CheckMapping(meanSparseShape, subjectPts, rbfTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#else
        CheckMapping(meanSparseShape, subjectPts, tpsTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#endif
        
        std::cout << "rms_wo_mapping = " << rms_wo_mapping << std::endl;
        std::cout << "rms = " << rms << std::endl;
        std::cout << "maximum Distance = " << maxmDist << std::endl;
#ifdef VIS_DEBUG
        if(params.display)
            visParticles(subjectPts,mappedCorrespondences, params.glyph_radius, std::string(basename + ": subject particles in red and mapped correspondences in green"));
#endif

        vtkSmartPointer<vtkPolyData> denseShape                     = vtkSmartPointer<vtkPolyData>::New();
        denseShape->DeepCopy( meanDenseShape );

#ifndef USE_TPS
        generateWarpedMeshes( rbfTransform, denseShape );
#else
        generateWarpedMeshes( tpsTransform, denseShape );
#endif

        if(params.display)
            visMeshWithParticles(denseShape, subjectPts, params.glyph_radius, std::string(basename + ": dense shape in red and subject sparse shape in white"));
        
        std::string outfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.localPointsFilenames[shapeNo])) + ".dense.vtk";
        std::cout << "Writing: " << outfilename << std::endl;

        vtkPolyDataWriter * polywriter =   vtkPolyDataWriter::New();
        polywriter->SetFileName(outfilename.c_str());
#if (VTK_MAJOR_VERSION < 6)
        polywriter->SetInput(denseShape);
#else
        polywriter->SetInputData(denseShape);
#endif
        polywriter->Update();

        std::string outfilename2 = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.localPointsFilenames[shapeNo])) + ".dense.ply";
        vtkPLYWriter *plyWriter=vtkPLYWriter::New();
#if (VTK_MAJOR_VERSION < 6)
        plyWriter->SetInput(denseShape);
#else
        plyWriter->SetInputData(denseShape);
#endif
        plyWriter->SetFileName(outfilename2.c_str());
        plyWriter->Update();

        vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
        vertices->DeepCopy( denseShape->GetPoints() );

        std::string ptsfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.localPointsFilenames[shapeNo])) + ".dense.lpts";
        unsigned int numPoints = vertices->GetNumberOfPoints();
        ofstream ofs;
        ofs.open(ptsfilename.c_str());
        for(unsigned int i = 0; i < numPoints; i++)
        {
            double meshPoint[3];
            vertices->GetPoint(i, meshPoint);
            ofs << meshPoint[0] << " " << meshPoint[1] << " " << meshPoint[2] << std::endl;
        }
        ofs.close();

    }
    

    return EXIT_SUCCESS;
    
}

//------------------------ auxillary functions ---------------------------------------------------

//------------------------debugging -------------------------------
#ifndef USE_TPS
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  RBFTransformType::Pointer rbfTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double & maxmDist)
{
    // source should be warped to the target
    rms = 0.0;
    rms_wo_mapping = 0.0;
    for(unsigned int ii = 0 ; ii < sourcePts->GetNumberOfPoints(); ii++)
    {
        double ps[3]; // source
        double pt[3]; // target
        double pw[3]; // warped
        
        sourcePts->GetPoint(ii, ps);
        targetPts->GetPoint(ii, pt);
        
        itk::Point<double,3> ps_;
        itk::Point<double,3> pt_;
        itk::Point<double,3> pw_;
        
        ps_[0] = ps[0]; ps_[1] = ps[1]; ps_[2] = ps[2];
        pt_[0] = pt[0]; pt_[1] = pt[1]; pt_[2] = pt[2];
        
        pw_ = rbfTransform->TransformPoint(ps_);
        
        double cur_rms            = pw_.EuclideanDistanceTo(pt_);
        double cur_rms_wo_mapping = ps_.EuclideanDistanceTo(pt_);
        
        
        //double cur_rms = sqrt(pow(ps[0]-pw[0],2.0) + pow(ps[1]-pw[1],2.0) + pow(ps[2]-pw[2],2.0));
        
        //                std::cout << "ps = (" << ps_[0] << " , " << ps_[1] << " , " << ps_[2] << std::endl;
        //                std::cout << "pw = (" << pw_[0] << " , " << pw_[1] << " , " << pw_[2] << std::endl;
        //                std::cout << "cur_rms = " << cur_rms << std::endl;
        
        rms += cur_rms;
        rms_wo_mapping += cur_rms_wo_mapping;
        
        pw[0] = pw_[0]; pw[1] = pw_[1]; pw[2] = pw_[2];
        mappedCorrespondences->InsertNextPoint(pw);
        
    }
    maxmDist = -10000.0f;
    for(unsigned int ii = 0; ii < mappedCorrespondences->GetNumberOfPoints(); ii++) {
        double pi[3];
        mappedCorrespondences->GetPoint(ii, pi);
        itk::Point<double, 3> pi_;
        pi_[0] = pi[0]; pi_[1] = pi[1]; pi_[2] = pi[2];
        for(unsigned int jj = 0; jj < mappedCorrespondences->GetNumberOfPoints(); jj++) {
            double pj[3];
            mappedCorrespondences->GetPoint(jj, pj);
            itk::Point<double, 3> pj_;
            pj_[0] = pj[0]; pj_[1] = pj[1]; pj_[2] = pj[2];
            double dist = pi_.EuclideanDistanceTo(pj_);
            if(dist > maxmDist) {
                maxmDist = dist;
            }
        }
    }
    rms /= sourcePts->GetNumberOfPoints();
    rms_wo_mapping /= sourcePts->GetNumberOfPoints();
}
#else
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  ThinPlateSplineType::Pointer tpsTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double & maxmDist)
{
    // source should be warped to the target
    rms = 0.0;
    rms_wo_mapping = 0.0;
    for(unsigned int ii = 0 ; ii < sourcePts->GetNumberOfPoints(); ii++)
    {
        double ps[3]; // source
        double pt[3]; // target
        double pw[3]; // warped
        
        sourcePts->GetPoint(ii, ps);
        targetPts->GetPoint(ii, pt);
        
        itk::Point<double,3> ps_;
        itk::Point<double,3> pt_;
        itk::Point<double,3> pw_;
        
        ps_[0] = ps[0]; ps_[1] = ps[1]; ps_[2] = ps[2];
        pt_[0] = pt[0]; pt_[1] = pt[1]; pt_[2] = pt[2];
        
        pw_ = tpsTransform->TransformPoint(ps_);
        
        double cur_rms            = pw_.EuclideanDistanceTo(pt_);
        double cur_rms_wo_mapping = ps_.EuclideanDistanceTo(pt_);
        
        
        //double cur_rms = sqrt(pow(ps[0]-pw[0],2.0) + pow(ps[1]-pw[1],2.0) + pow(ps[2]-pw[2],2.0));
        
        //                std::cout << "ps = (" << ps_[0] << " , " << ps_[1] << " , " << ps_[2] << std::endl;
        //                std::cout << "pw = (" << pw_[0] << " , " << pw_[1] << " , " << pw_[2] << std::endl;
        //                std::cout << "cur_rms = " << cur_rms << std::endl;
        
        rms += cur_rms;
        rms_wo_mapping += cur_rms_wo_mapping;
        
        pw[0] = pw_[0]; pw[1] = pw_[1]; pw[2] = pw_[2];
        mappedCorrespondences->InsertNextPoint(pw);
        
    }
    
    maxmDist = -10000.0f;
    for(unsigned int ii = 0; ii < mappedCorrespondences->GetNumberOfPoints(); ii++) {
        double pi[3];
        mappedCorrespondences->GetPoint(ii, pi);
        itk::Point<double, 3> pi_;
        pi_[0] = pi[0]; pi_[1] = pi[1]; pi_[2] = pi[2];
        for(unsigned int jj = 0; jj < mappedCorrespondences->GetNumberOfPoints(); jj++) {
            double pj[3];
            mappedCorrespondences->GetPoint(jj, pj);
            itk::Point<double, 3> pj_;
            pj_[0] = pj[0]; pj_[1] = pj[1]; pj_[2] = pj[2];
            double dist = pi_.EuclideanDistanceTo(pj_);
            if(dist > maxmDist) {
                maxmDist = dist;
            }
        }
    }
    
    std::cout << "maximum Distance = " << maxmDist << std::endl;
    rms /= sourcePts->GetNumberOfPoints();
    rms_wo_mapping /= sourcePts->GetNumberOfPoints();
}

#endif

//-------------------------------------------------------------------------------------------

double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points, std::vector<int> particles_indices)
{
    int K = 6; // hexagonal ring - one jump
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    
    // Create the tree
    vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
    kDTree->SetDataSet(polydata);
    kDTree->BuildLocator();
    
    // Find the closest points to each particle
    double avgDist = 0;
    for(unsigned int ii = 0 ; ii < particles_indices.size(); ii++)
    {
        double p[3];
        points->GetPoint(particles_indices[ii], p);
        
        vtkSmartPointer<vtkIdList> result = vtkSmartPointer<vtkIdList>::New();
        kDTree->FindClosestNPoints( K+1, p, result ); // +1 to exclude myself
        
        double meanDist = 0;
        for(vtkIdType k = 0; k < K+1; k++)
        {
            vtkIdType pid = result->GetId(k);
            
            if (pid == particles_indices[ii]) // close to myself
                continue;
            
            double pk[3];
            points->GetPoint(pid, pk);
            
            double curDist = sqrt(pow(p[0]-pk[0],2.0) + pow(p[1]-pk[1],2.0) + pow(p[2]-pk[2],2.0));
            
            meanDist += curDist;
        }
        meanDist /= K;
        
        avgDist+= meanDist;
    }
    avgDist /= particles_indices.size();
    return avgDist;
}




#ifndef USE_TPS
void generateWarpedMeshes( RBFTransformType::Pointer transform,
                           vtkSmartPointer<vtkPolyData> &outputMesh )
{
    // generate warped meshes
    vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
    vertices->DeepCopy( outputMesh->GetPoints() );

    unsigned int numPointsToTransform = vertices->GetNumberOfPoints();
    for(unsigned int i = 0; i < numPointsToTransform; i++)
    {
        double meshPoint[3];
        vertices->GetPoint(i, meshPoint);

        itk::Point<double,3> pm_;
        itk::Point<double,3> pw_;

        pm_[0] = meshPoint[0]; pm_[1] = meshPoint[1]; pm_[2] = meshPoint[2];

        pw_ = transform->TransformPoint(pm_);

        vertices->SetPoint(i, pw_[0], pw_[1], pw_[2]);
    }

    outputMesh->SetPoints( vertices );
    outputMesh->Modified();
}
#else
void generateWarpedMeshes( ThinPlateSplineType::Pointer transform,
                           vtkSmartPointer<vtkPolyData> &outputMesh )
{
    // generate warped meshes
    vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
    vertices->DeepCopy( outputMesh->GetPoints() );

    unsigned int numPointsToTransform = vertices->GetNumberOfPoints();
    for(unsigned int i = 0; i < numPointsToTransform; i++)
    {
        double meshPoint[3];
        vertices->GetPoint(i, meshPoint);

        itk::Point<double,3> pm_;
        itk::Point<double,3> pw_;

        pm_[0] = meshPoint[0]; pm_[1] = meshPoint[1]; pm_[2] = meshPoint[2];

        pw_ = transform->TransformPoint(pm_);

        vertices->SetPoint(i, pw_[0], pw_[1], pw_[2]);
    }

    outputMesh->SetPoints( vertices );
    outputMesh->Modified();
    outputMesh->Update();
}

#endif
