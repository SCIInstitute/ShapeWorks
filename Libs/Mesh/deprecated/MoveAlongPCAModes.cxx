/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2015/11/22 10:34:10 $
  Version:   $Revision: 1.1 $
  Author:    $Author: elhabian $
  Purpose:   particle-based sampling of shape space with a dense surface reconstruction, (use output of WarpToMeanSpace)
                (1) using the wpts from WarpToMeanSpace, compute the PCA model
                (2) move along dominant components (using scree test) +/- factor * std using the sparse correspondence model
                (3) warp the mean dense shape (mesh) constructed from WarpToMeanSpace to each individual sample shape along pca modes

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

//-------------- vnl --------------------------
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
//#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

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

// statistical analysis (pca)

// a vnl double matrix.
typedef vnl_matrix<double> MatrixOfDoubleType;

// a vnl double vector.
typedef vnl_vector<double> VectorOfDoubleType;

// vnl decomposition
typedef vnl_symmetric_eigensystem<double> MatrixDecompositionType;
//typedef vnl_svd<double> MatrixDecompositionType;

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
    params.readParams(argv[1], 2); // 2 - MoveAlongPCAModes
    
    std::cout << "Number of input sparse shapes: " << params.localPointsFilenames.size() << std::endl;
    
    //----------------------------------- let the fun begins --------------------------
    
    // (1) define mean sparse shape -- this is considered as source points in the warp
    std::string meanShape_filename = params.mean_prefix + ".meanSparseShape.lpts" ;
    vtkSmartPointer< vtkPoints > meanSparseShape = utils::readSparseShape((char*) meanShape_filename.c_str());

    // define mean dense shape to be warped
    std::string meanDense_filename = params.mean_prefix + ".meanDenseShape.vtk" ;
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

    // (4) read-in the world coordinates of the given sparse correspondence model (using the output of WarpToMeanSpace)
    unsigned int NumberOfShapes = params.localPointsFilenames.size();
    MatrixOfDoubleType shapesMatrix(params.number_of_particles*Dimension, NumberOfShapes, 0.0);
    VectorOfDoubleType meanShape(params.number_of_particles*Dimension, 0.0);

    // Read input shapes from file list
    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
    {
        std::string shapefilename = params.mean_path + "/" + utils::removeExtension(utils::getFilename(params.localPointsFilenames[shapeNo])) + ".wpts";
        std::ifstream ifs;
        ifs.open(shapefilename.c_str());
        for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
        {
            double p[3];
            ifs >> p[0] >> p[1] >> p[2];

            for (unsigned int d = 0 ; d < Dimension; d++)
            {
                shapesMatrix(ii*Dimension + d, shapeNo) = p[d];
                //meanShape(ii*Dimension + d)            += p[d];
            }
        }
        ifs.close();
    }

    // (5) mean computation and centering the data matrix
    // finalize mean computation
    for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
    {
        double pt[3];
        meanSparseShape->GetPoint(ii, pt);

        for (unsigned int d = 0 ; d < Dimension; d++)
        {
            //meanShape(ii*Dimension + d)   /= (double)NumberOfShapes;
            meanShape(ii*Dimension + d)   = pt[d];
        }
    }
    // std::cout << "meanShape: " << meanShape << std::endl;

    // center the shapes matrix
    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
        for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
            for (unsigned int d = 0 ; d < Dimension; d++)
                shapesMatrix(ii*Dimension + d, shapeNo) -= meanShape(ii*Dimension + d);

    // (6) compute the covariance matrix in the dual space
    MatrixOfDoubleType covMatrix = shapesMatrix.transpose() * shapesMatrix * (1.0/((double)(NumberOfShapes-1)));

    //std::cout << "covMatrix: " << covMatrix << std::endl;

    // (7) covariance matrix decomposition
    // note that for symmetric matrices, singular values and eigen values coincide
    MatrixDecompositionType covMatrixDecomposition(covMatrix);

    std::cout << "eigen values: " << covMatrixDecomposition.D << std::endl;

    // (8) get the numbder of dominant modes (scree test)
    unsigned int NumberOfModes = 0;
    double total_sum    = 0.0;
    for (int shapeNo = NumberOfShapes-1; shapeNo >= 0; shapeNo--)
        total_sum += covMatrixDecomposition.D(shapeNo);

    double cumsum = 0.0;
    for (int shapeNo = NumberOfShapes-1; shapeNo >= 0; shapeNo--)
    {
        cumsum += covMatrixDecomposition.D(shapeNo);
        if((cumsum/total_sum) >= params.maximum_variance_captured)
        {
            NumberOfModes = NumberOfShapes - shapeNo;
            break;
        }
    }

    if (NumberOfModes == 0)
    {
        std::cerr << "No dominant modes detected !!!!!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << NumberOfModes << " dominant modes is found to capture " << params.maximum_variance_captured*100 << "% of total variation ..." << std::endl;

    // (9) start sampling along each mode
    std::string prefix = utils::getFilename(params.out_prefix);
    for (int modeId = 0 ; modeId < NumberOfModes; modeId ++)
    {
        std::string modeStr = utils::int2str(modeId, 2);

        double eigenValue      = covMatrixDecomposition.D(NumberOfShapes - modeId - 1);
        double sqrt_eigenValue = sqrt(eigenValue);

        // mode in the dual space
        VectorOfDoubleType curModeDual = covMatrixDecomposition.get_eigenvector(NumberOfShapes - modeId - 1);

        // mode in the data space
        VectorOfDoubleType curMode = (1.0/sqrt_eigenValue) * shapesMatrix * curModeDual;

        //std::cout << "curMode: " << curMode << std::endl;

        double min_std = -1 * params.maximum_std_dev * sqrt_eigenValue;
        double max_std = +1 * params.maximum_std_dev * sqrt_eigenValue;

        std::cout << "mode #" << modeId << ": min_std = " << min_std << ", max_std = " << max_std  << std::endl;

        std::vector<double> std_store = utils::linspace(min_std, max_std, params.number_of_samples_per_mode);

        std::cout << "std_store: " ;
        for (unsigned int sid = 0 ; sid < std_store.size(); sid++)
            std::cout << std_store[sid] << ", " ;
        std::cout << std::endl;

        for(unsigned int sampleId = 0 ; sampleId < std_store.size(); sampleId++)
        {

            std::string sampleStr = utils::int2str(sampleId, 3);

            std::string basename = prefix + ".mode." + modeStr + ".sample." + sampleStr ;

            std::cout << "generating mode #" + utils::num2str((float)modeId) + ", sample #" + utils::num2str((float)sampleId) << std::endl;
            // generate the sparse shape of the current sample
            double cur_std = std_store[sampleId];

            std::cout << "cur_std: " << cur_std << std::endl;

            VectorOfDoubleType curSample = meanShape + cur_std * curMode;

            //std::cout << "curSample: " << curSample << std::endl;

            // fill-in the vtkpoints structure to perform warping for dense reconstruction of the current sample
            vtkSmartPointer< vtkPoints > curSamplePts = vtkSmartPointer< vtkPoints >::New();
            for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
            {
                double pt[3];
                for (unsigned int d = 0 ; d < Dimension; d++)
                    pt[d] = curSample(ii*Dimension + d);

                curSamplePts->InsertNextPoint(pt);
            }

#ifdef VIS_DEBUG
            if(params.display)
                visParticles(meanSparseShape, curSamplePts, params.glyph_radius, std::string(basename + ": mean sparse shape in red and sample particles in green"));
#endif

            // Define container for target landmarks corresponds to the sample shape
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
                    curSamplePts->GetPoint(ii,p);
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
            CheckMapping(meanSparseShape, curSamplePts, rbfTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#else
            CheckMapping(meanSparseShape, curSamplePts, tpsTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#endif

            std::cout << "rms_wo_mapping = " << rms_wo_mapping << std::endl;
            std::cout << "rms = " << rms << std::endl;
            std::cout << "maximum Distance = " << maxmDist << std::endl;
#ifdef VIS_DEBUG
            if(params.display)
                visParticles(curSamplePts,mappedCorrespondences, params.glyph_radius, std::string(basename + ": subject particles in red and mapped correspondences in green"));
#endif

            vtkSmartPointer<vtkPolyData> denseShape                     = vtkSmartPointer<vtkPolyData>::New();
            denseShape->DeepCopy( meanDenseShape );

#ifndef USE_TPS
            generateWarpedMeshes( rbfTransform, denseShape );
#else
            generateWarpedMeshes( tpsTransform, denseShape );
#endif

            if(params.display)
                visMeshWithParticles(denseShape, curSamplePts, params.glyph_radius, std::string(basename + ": dense shape in red and subject sparse shape in white"));

            std::string outfilename = params.out_path + '/'+ basename + ".vtk";
            std::cout << "Writing: " << outfilename << std::endl;

            vtkPolyDataWriter * polywriter =   vtkPolyDataWriter::New();
            polywriter->SetFileName(outfilename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            polywriter->SetInput(denseShape);
#else
            polywriter->SetInputData(denseShape);
#endif
            polywriter->Update();

            std::string outfilename2 = params.out_path + '/'+ basename + ".ply";
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

            std::string ptsfilename = params.out_path + '/'+ basename + ".wpts";
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
