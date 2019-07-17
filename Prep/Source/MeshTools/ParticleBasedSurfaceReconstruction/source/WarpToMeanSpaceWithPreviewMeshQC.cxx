
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2015/11/22 4:07:20 $
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
#include <time.h>

//#define VIS_DEBUG
//#define USE_TPS

//--------------- ITK --------------------------
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAffineTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>

#include <itkImageRegionIterator.h>
#include <itkCovariantVector.h>
#include <itkGradientImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkCovariantVector.h>

#include <itkAddImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkTransform.h>
#include "itkMultiplyByConstantImageFilter.h"
#include "itkNrrdImageIOFactory.h"
#include "itkMetaImageIOFactory.h"

//-------------- adapted ITK filters --------------------------
#include "Transforms/itkThinPlateSplineKernelTransform2.h"
#include "Transforms/itkCompactlySupportedRBFSparseKernelTransform.h"
#include "itkResampleImageFilter.h"

//------------- VTK ---------------------
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>

#include <vtkIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkUnsignedCharArray.h>

#include <vtkContourFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataConnectivityFilter.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPLYWriter.h>

#include <vtkIdList.h>
#include <vtkKdTreePointLocator.h>
#include <vtkKdTree.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkProbeFilter.h>

//--------- ITK <-> VTK ---------------
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"

//--------- our stuff ----------------
#include "Params/InputParams.h"
#include "Utils/utils.h"
#include "Utils/vis.h"
#include "Procrustes3D.h"


// --------- preview mesh qc -------------
#include "FEVTKImport.h"
#include "FEVTKExport.h"
#include "FEFixMesh.h"
#include "FEMeshSmoothingModifier.h"
#include "FECVDDecimationModifier.h"

using namespace std;

//------------- typedefs ---------------
const unsigned int Dimension = 3;
typedef float      InputPixelType;
typedef float      OutputPixelType;

// basic types
typedef itk::Image< float,  Dimension >   FloatImageType;
typedef itk::ImageRegionIterator< FloatImageType > ImageIteratorType;

typedef itk::Image< InputPixelType, Dimension >  InputImageType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

typedef itk::Matrix<double, Dimension, Dimension> itkMatrix;
typedef itk::Vector<double,Dimension>             itkVector;

// IO
typedef itk::ImageFileReader< InputImageType >   ReaderType;
typedef itk::ImageFileWriter< OutputImageType >  WriterType;

// for mean distance transform computation
typedef itk::AddImageFilter <InputImageType, InputImageType >                     AddImageFilterType;
typedef itk::MultiplyByConstantImageFilter<InputImageType, float, InputImageType> MultiplyByConstantImageFilterType;
typedef itk::ImageDuplicator< InputImageType > DuplicatorType;

// connectors
typedef itk::ImageToVTKImageFilter<InputImageType>  ITK2VTKConnectorType;
typedef itk::VTKImageToImageFilter<InputImageType>  VTK2ITKConnectorType;

// interpolation
//typedef itk::LinearInterpolateImageFunction<InputImageType, double > InterpolatorType;
typedef itk::BSplineInterpolateImageFunction<InputImageType, double, double > InterpolatorType;

// resampling
typedef itk::ResampleImageFilter<InputImageType, OutputImageType > ResampleFilterType;

// warping ----------
typedef itk::AffineTransform< double, Dimension > TransformType;
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

// for normals ------
typedef itk::GradientImageFilter<FloatImageType, float>  GradientFilterType;
typedef itk::GradientMagnitudeImageFilter<FloatImageType, FloatImageType >  GradientMagnitudeFilterType;

typedef itk::Image< itk::CovariantVector< float, 3 >, 3 > GradientImageType;
typedef itk::ImageRegionIterator< GradientImageType > GradientImageIteratorType;

//------------------------ auxillary functions ---------------------------------------------------
//------------------------debugging -------------------------------
#ifdef USE_TPS
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  ThinPlateSplineType::Pointer tpsTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double &maxmDist);
#else
void CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                  vtkSmartPointer<vtkPoints> targetPts,
                  RBFTransformType::Pointer rbfTransform,
                  vtkSmartPointer<vtkPoints> & mappedCorrespondences,
                  double & rms, double & rms_wo_mapping, double &maxmDist);
#endif

//-------------------------------------------------------------------------------------------
vtkSmartPointer< vtkPoints > computeMeanSparseShape(std::vector< std::string > localPointsFilenames, std::string out_path,
                                                    int number_of_particles, int& medianShapeIndex, bool do_procrustes = true, bool do_procrustes_scaling = false);


double computeAverageDistanceToNeighbors(vtkSmartPointer<vtkPoints> points, std::vector<int> particles_indices);
vtkSmartPointer<vtkPolyData> extractIsosurface(vtkSmartPointer<vtkImageData> volData,float levelsetValue,
                                               float targetReduction,
                                               float featureAngle,
                                               int lsSmootherIterations,
                                               int meshSmootherIterations,
                                               bool preserveTopology);


vnl_matrix<double> computeParticlesNormals(string localPointsFilename,
                                           string distanceTransformFilename,
                                           int number_of_particles);

void performKMeansClustering(std::vector< std::string > worldPointsFilenames, unsigned int number_of_particles, int K, std::vector<int> & centroidIndices);
// ---------------------------------------------------------------------------

int MeshQC(std::string inFilename, std::string outFilename, bool fixWinding,  bool doLaplacianSmoothingBeforeDecimation, bool doLaplacianSmoothingAfterDecimation,
           float smoothingLambda, int smoothingIterations,  float decimationPercentage);

int main( int argc , char* argv[] )
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }
    //    std::cout<<"HelloHelloHello"<<std::endl;
    InputParams params;
    params.readParams(argv[1], 3); // 3 - WarpToMeanSpaceWithPreviewMeshQC

    std::cout << "Number of input sparse shapes: " << params.localPointsFilenames.size() << std::endl;

    //----------------------------------- let the fun begins --------------------------

    // (1) define mean sparse shape -- this is considered as target points in the warp
    int medianShapeIndex;
    vtkSmartPointer< vtkPoints > meanSparseShape;
    if(params.worldPointsFilenames.size() == 0)
    {
        meanSparseShape = computeMeanSparseShape(params.localPointsFilenames, params.out_path, // to write aligned shapes for subsequent statistical analysis
                                                 params.number_of_particles, medianShapeIndex,
                                                 params.do_procrustes,
                                                 params.do_procrustes_scaling);
    }
    else
    {
        meanSparseShape = computeMeanSparseShape(params.worldPointsFilenames, params.out_path, // to write aligned shapes for subsequent statistical analysis
                                                 params.number_of_particles, medianShapeIndex,
                                                 false,
                                                 false);
    }

#ifdef VIS_DEBUG
    if(params.display)
        visParticles(meanSparseShape,params.glyph_radius,std::string("mean sparse shape"));
#endif

    // debugging: making sure of the correspondences through computing normals across shapes for each particle
    std::vector< vnl_matrix<double> > normals;
    normals.clear();
    for (unsigned int shapeNo = 0; shapeNo < params.localPointsFilenames.size(); shapeNo++)
    {
        std::cout << "Processing for normals computation: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;
        //            std::cout<<"HelloHelloHello"<<std::endl;
        vnl_matrix<double> cur_normals = computeParticlesNormals(params.localPointsFilenames[shapeNo],
                                                                 params.distanceTransformFilenames[shapeNo],
                                                                 params.number_of_particles);
        //            std::cout<<"HelloHelloHello"<<std::endl;
        normals.push_back(cur_normals);

    }

    // now decide whether each particle is a good (it normals are in the same direction accross shapes) or
    // bad (there is discrepency in the normal directions across shapes)
    std::vector<int> bad_ids;  bad_ids.clear();
    std::vector<int> good_ids; good_ids.clear();

    std::string outfilenameGood = params.out_prefix  + "goodMeanShape.wpts";
    std::string outfilenameBad = params.out_prefix  + "badMeanShape.wpts";
    std::ofstream ofsG, ofsB;
    ofsG.open(outfilenameGood.c_str());
    ofsB.open(outfilenameBad.c_str());

    for (unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
    {
        double pt[3];
        bool isGood = true;

        // the normal of the first shape
        double nx_jj = normals[0](ii,0);
        double ny_jj = normals[0](ii,1);
        double nz_jj = normals[0](ii,2);

        // start from the second
        for (unsigned int shapeNo_kk = 1; shapeNo_kk < params.localPointsFilenames.size(); shapeNo_kk++)
        {
            double nx_kk = normals[shapeNo_kk](ii,0);
            double ny_kk = normals[shapeNo_kk](ii,1);
            double nz_kk = normals[shapeNo_kk](ii,2);

            if((nx_jj*nx_kk + ny_jj*ny_kk + nz_jj*nz_kk) < std::cos(params.normalAngle))
            {
                isGood = false;
                break;
            }
        }

        meanSparseShape->GetPoint(ii, pt);
        // append it to the decided list
        if(!isGood)
        {
            bad_ids.push_back(ii);
            ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
        }
        else
        {
            good_ids.push_back(ii);
            ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
        }
    }
    ofsG.close();
    ofsB.close();

    string badfilename = params.out_prefix + ".BadParticleIds.txt";
    utils::writeParticleIds((char*) badfilename.c_str(), bad_ids);

    string goodfilename = params.out_prefix + ".GoodParticleIds.txt";
    utils::writeParticleIds((char*) goodfilename.c_str(), good_ids);

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


    // The parameters of the output image are taken from the input image.
    // NOTE: all distance transforms were generated throughout shapeworks pipeline
    // as such they have the same parameters
    auto filename = params.distanceTransformFilenames[0];
    if (filename.find(".nrrd") != std::string::npos) {
        itk::NrrdImageIOFactory::RegisterOneFactory();
    } else if (filename.find(".mha") != std::string::npos) {
        itk::MetaImageIOFactory::RegisterOneFactory();
    }
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( params.distanceTransformFilenames[0].c_str() );
    reader->Update();
    const InputImageType::SpacingType& spacing     = reader->GetOutput()->GetSpacing();
    const InputImageType::PointType& origin        = reader->GetOutput()->GetOrigin();
    const InputImageType::DirectionType& direction = reader->GetOutput()->GetDirection();
    InputImageType::SizeType size                  = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    InputImageType::RegionType region              = reader->GetOutput()->GetBufferedRegion();

    // decide which correspondences will be used to build the warp
    std::vector<int> particles_indices;
    particles_indices.clear();
    for(unsigned int kk = 0 ; kk < good_ids.size(); kk++)
        particles_indices.push_back(good_ids[kk]);

    // (3) define the mean dense shape (mean distance transform)
    InputImageType::Pointer meanDistanceTransform = InputImageType::New();
    meanDistanceTransform->SetOrigin(origin);
    meanDistanceTransform->SetSpacing(spacing);
    meanDistanceTransform->SetDirection(direction);
    meanDistanceTransform->SetLargestPossibleRegion(size);

    InputImageType::Pointer meanDistanceTransformBeforeWarp = InputImageType::New();
    meanDistanceTransformBeforeWarp->SetOrigin(origin);
    meanDistanceTransformBeforeWarp->SetSpacing(spacing);
    meanDistanceTransformBeforeWarp->SetDirection(direction);
    meanDistanceTransformBeforeWarp->SetLargestPossibleRegion(size);

    //    if (params.number_of_attributes > 0)
    //    {
    //        std::vector<InputImageType::Pointer> meanAttributeVolumes;
    //        for(unsigned int attr = 0; attr < params.number_of_attributes; attr++)
    //        {
    //            meanAttributeVolumes.push_back( InputImageType::New() );
    //            meanAttributeVolumes[attr]->SetOrigin(origin);
    //            meanAttributeVolumes[attr]->SetSpacing(spacing);
    //            meanAttributeVolumes[attr]->SetDirection(direction);
    //            meanAttributeVolumes[attr]->SetLargestPossibleRegion(size);
    //        }
    //    }

    AddImageFilterType::Pointer sumfilter           = AddImageFilterType::New();
    AddImageFilterType::Pointer sumfilterBeforeWarp = AddImageFilterType::New();

    // using radial basis function transform to deform the current distance transform based on the correspondence between
    // the current sparse shape and the procrustes mean

    // Landmarks correspondances may be associated with the SplineKernelTransforms
    // via Point Set containers. Let us define containers for the landmarks.

    // Define container for source landmarks that corresponds to the mean space, this is
    // fixed where the target (each individual shape) will be warped to
    // NOTE that this is inverse warping to avoid holes in the warped distance transforms
    PointSetType::Pointer sourceLandMarks  = PointSetType::New();
    PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
    PointType ps;
    PointIdType id = itk::NumericTraits< PointIdType >::Zero;
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

    double averageDistToNeighbors = computeAverageDistanceToNeighbors(meanSparseShape,  particles_indices);
    std::cout << "averageDistToNeighbors: " << averageDistToNeighbors << std::endl;

    double sigma = averageDistToNeighbors;
    std::cout << "sigma: " << sigma << std::endl;

    // the roles of the source and target are reversed to simulate a reverse warping
    // without explicitly invert the warp in order to avoid holes in the warping result
    RBFTransformType::Pointer rbfTransform = RBFTransformType::New();
    rbfTransform->SetSigma(sigma); // smaller means more sparse
    //rbfTransform->SetStiffness(0.25*sigma);
    rbfTransform->SetStiffness(1e-10);
#else
    ThinPlateSplineType::Pointer tpsTransform = ThinPlateSplineType::New();
    tpsTransform->SetStiffness( 1e-10 );
#endif

    std::cout << "setting source landmarks ..." << std::endl ;
#ifdef USE_TPS
    tpsTransform->SetSourceLandmarks(sourceLandMarks);
#else
    rbfTransform->SetSourceLandmarks(sourceLandMarks);
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Praful - get the shape indices corresponding to cetroids of kmeans clusters and run the following loop on only those shapes
    //
    // Read input shapes from file list
    std::vector<int> centroidIndices;
    if (params.K > 0)
     {   performKMeansClustering(params.worldPointsFilenames, params.number_of_particles, params.K, centroidIndices);
//        std::cout<<"Indices from KMeans++:"<<std::endl;
//        for (int ii1 = 0; ii1<params.K; ii1++)
//        {
//            std::cout<<centroidIndices[ii1]<<std::endl;
//        }
    }
    else
    {
        params.K = params.distanceTransformFilenames.size();
        centroidIndices.resize(params.distanceTransformFilenames.size());
        for (unsigned int shapeNo = 0; shapeNo < params.distanceTransformFilenames.size(); shapeNo++)
        {
            centroidIndices[shapeNo]=shapeNo;
        }
//        for (int ii1 = 0; ii1<params.K; ii1++)
//        {
//            std::cout<<centroidIndices[ii1]<<std::endl;
//        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //    for (unsigned int shapeNo = 0; shapeNo < params.distanceTransformFilenames.size(); shapeNo++)
    //    {

    //Praful - clustering
    for (unsigned int cnt = 0; cnt<params.K; cnt++)
    {
        unsigned int shapeNo = centroidIndices[cnt];

        std::string basename = utils::getFilename(params.distanceTransformFilenames[shapeNo]);

        std::cout << "Processing: " << params.distanceTransformFilenames[shapeNo].c_str() << std::endl;

        // read the current distance transform
        auto filename = params.distanceTransformFilenames[shapeNo];
        if (filename.find(".nrrd") != std::string::npos) {
            itk::NrrdImageIOFactory::RegisterOneFactory();
        } else if (filename.find(".mha") != std::string::npos) {
            itk::MetaImageIOFactory::RegisterOneFactory();
        }
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( params.distanceTransformFilenames[shapeNo].c_str() );
        reader->Update();

        // Define container for target landmarks corresponds to the current shape that will be warped to the mean space
        // load the original local points to be aligned to the mean sparse
        std::cout << "loading particles ..." << std::endl ;
        vtkSmartPointer< vtkPoints > targetPts = utils::readSparseShape((char*) params.localPointsFilenames[shapeNo].c_str(),
                                                                        params.number_of_particles);

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
                targetPts->GetPoint(ii,p);
                pt[0] = p[0];
                pt[1] = p[1];
                pt[2] = p[2];
                targetLandMarkContainer->InsertElement( id++, pt );
                nt++;
            }
        }
        std::cout << "number of target particles: " << nt << std::endl;
#ifdef VIS_DEBUG
        if(params.display)
            visParticles(targetPts,meanSparseShape,params.glyph_radius, std::string(basename + ": target (subject) particles in red and mean sparse shape in green"));
#endif

        std::cout << "setting target landmarks ..." << std::endl ;
#ifndef USE_TPS
        rbfTransform->SetTargetLandmarks(targetLandMarks);
#else
        tpsTransform->SetTargetLandmarks(targetLandMarks);
#endif

        std::cout << "done setting target landmarks ..." << std::endl ;

        // check the mapping (inverse here)
        // this mean source points (mean space) should be warped to the target (current sample's space)
        vtkSmartPointer<vtkPoints> mappedCorrespondences = vtkSmartPointer<vtkPoints>::New();
        double rms;
        double rms_wo_mapping;
        double maxmDist;
#ifdef USE_TPS
        CheckMapping(meanSparseShape,targetPts, tpsTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#else
        CheckMapping(meanSparseShape,targetPts, rbfTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
#endif


        std::cout << "rms_wo_mapping = " << rms_wo_mapping << std::endl;
        std::cout << "rms = " << rms << std::endl;
        std::cout << "maximum Distance = " << maxmDist << std::endl;
#ifdef VIS_DEBUG
        if(params.display)
            visParticles(targetPts,mappedCorrespondences, params.glyph_radius, std::string(basename + ": target particles in red and mapped correspondences in green"));
#endif

        // The image is then resampled to produce an output image as defined by the
        // transform. Here we use a LinearInterpolator.

        // Set the resampler params
        //InputIm1ageType::ConstPointer inputImage = reader->GetOutput();
        itk::TimeProbe clock;
        clock.Start();
        ResampleFilterType::Pointer   resampler = ResampleFilterType::New();
        InterpolatorType::Pointer interpolator  = InterpolatorType::New();
        interpolator->SetSplineOrder(3);
        resampler->SetInterpolator( interpolator );

        resampler->SetOutputSpacing( spacing );
        resampler->SetOutputDirection( direction );
        resampler->SetOutputOrigin(  origin  );
        resampler->SetSize( size );
#ifndef USE_TPS
        resampler->SetTransform( rbfTransform );
#else
        resampler->SetTransform( tpsTransform );
#endif
        resampler->SetDefaultPixelValue((InputPixelType) -100.0);
        resampler->SetOutputStartIndex(  region.GetIndex() );
        resampler->SetInput( reader->GetOutput() );
        resampler->Update();

        clock.Stop();
        std::cout << "Resampling:" << std::endl;
        std::cout << "Mean: " << clock.GetMean() << std::endl;
        std::cout << "Total: " << clock.GetTotal() << std::endl;

        // transform attribute files
        itk::TimeProbe clock2;
        clock2.Start();
        for (int j = 0; j < params.number_of_attributes; j++) {
            auto filename = params.attribute_labels[shapeNo + j];
            if (filename.find(".nrrd") != std::string::npos) {
                itk::NrrdImageIOFactory::RegisterOneFactory();
            } else if (filename.find(".mha") != std::string::npos) {
                itk::MetaImageIOFactory::RegisterOneFactory();
            }
            ReaderType::Pointer readerAttr = ReaderType::New();
            std::cout << "Transform attribute file : " << params.attribute_labels[shapeNo+j] << std::endl;
            readerAttr->SetFileName( params.attribute_labels[shapeNo+j].c_str() );
            readerAttr->Update();

            const InputImageType::SpacingType& spacingAttr     = readerAttr->GetOutput()->GetSpacing();
            const InputImageType::PointType& originAttr        = readerAttr->GetOutput()->GetOrigin();
            const InputImageType::DirectionType& directionAttr = readerAttr->GetOutput()->GetDirection();
            InputImageType::SizeType sizeAttr                  = readerAttr->GetOutput()->GetLargestPossibleRegion().GetSize();
            InputImageType::RegionType regionAttr              = readerAttr->GetOutput()->GetBufferedRegion();

            ResampleFilterType::Pointer resamplerAttr = ResampleFilterType::New();
            resamplerAttr->SetInterpolator( interpolator );

            resamplerAttr->SetOutputSpacing( spacingAttr );
            resamplerAttr->SetOutputDirection( directionAttr );
            resamplerAttr->SetOutputOrigin(  originAttr  );
            resamplerAttr->SetSize( sizeAttr );
#ifndef USE_TPS
            resamplerAttr->SetTransform( rbfTransform );
#else
            resamplerAttr->SetTransform( tpsTransform );
#endif
            resamplerAttr->SetDefaultPixelValue((InputPixelType) 0.0);
            resamplerAttr->SetOutputStartIndex(  regionAttr.GetIndex() );
            resamplerAttr->SetInput( readerAttr->GetOutput() );
            resamplerAttr->Update();

            std::string outfilename;
            if (filename.find(".nrrd") != std::string::npos)
                outfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.attribute_labels[shapeNo+j])) + ".warped.nrrd";
            else if (filename.find(".mha") != std::string::npos)
                outfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.attribute_labels[shapeNo+j])) + ".warped.mha";

            std::cout << "Writing: " << outfilename << std::endl;

            WriterType::Pointer writer = WriterType::New();
            writer->SetFileName( outfilename.c_str());
            writer->SetInput( resamplerAttr->GetOutput() );
            writer->Update();
        }
        clock2.Stop();
        std::cout << "Attribute files --> Resampling:" << std::endl;
        std::cout << "Mean: " << clock.GetMean() << std::endl;
        std::cout << "Total: " << clock.GetTotal() << std::endl;

        if(params.display)
        {
            ITK2VTKConnectorType::Pointer itk2vtkConnector = ITK2VTKConnectorType::New();
            itk2vtkConnector->SetInput(resampler->GetOutput());
            itk2vtkConnector->Update();
            vtkSmartPointer<vtkPolyData> denseShape = extractIsosurface(itk2vtkConnector->GetOutput(),
                                                                        params.levelsetValue, params.targetReduction, params.featureAngle,
                                                                        params.lsSmootherIterations, params.meshSmootherIterations, params.preserveTopology);

            visMeshWithParticles(denseShape, meanSparseShape, params.glyph_radius, std::string(basename + ": dense shape in red and mean sparse shape in white"));


        }

        std::string outfilename;
        if (filename.find(".nrrd") != std::string::npos)
            outfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.distanceTransformFilenames[shapeNo])) + ".warped.nrrd";
        else if (filename.find(".mha") != std::string::npos)
            outfilename = params.out_path + '/'+ utils::removeExtension(utils::getFilename(params.distanceTransformFilenames[shapeNo])) + ".warped.mha";

        std::cout << "Writing: " << outfilename << std::endl;

        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outfilename.c_str());
        writer->SetInput( resampler->GetOutput() );
        writer->Update();

        //return EXIT_SUCCESS;

//        if(shapeNo==0)
        if(cnt==0) //Praful - clustering
        {
            // after warp
            DuplicatorType::Pointer duplicator = DuplicatorType::New();
            duplicator->SetInputImage(resampler->GetOutput());
            duplicator->Update();
            meanDistanceTransform = duplicator->GetOutput();

            // before warp
            DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
            duplicator2->SetInputImage(reader->GetOutput());
            duplicator2->Update();
            meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
        }
        else
        {
            // after warp
            sumfilter->SetInput1(meanDistanceTransform);
            sumfilter->SetInput2(resampler->GetOutput());
            sumfilter->Update();

            DuplicatorType::Pointer duplicator = DuplicatorType::New();
            duplicator->SetInputImage(sumfilter->GetOutput());
            duplicator->Update();
            meanDistanceTransform = duplicator->GetOutput();

            // before warp
            sumfilterBeforeWarp->SetInput1(meanDistanceTransformBeforeWarp);
            sumfilterBeforeWarp->SetInput2(reader->GetOutput());
            sumfilterBeforeWarp->Update();

            DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
            duplicator2->SetInputImage(sumfilterBeforeWarp->GetOutput());
            duplicator2->Update();
            meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
        }
    }

    MultiplyByConstantImageFilterType::Pointer multiplyImageFilter = MultiplyByConstantImageFilterType::New();
    multiplyImageFilter->SetInput(meanDistanceTransform);

//    multiplyImageFilter->SetConstant(1.0/params.distanceTransformFilenames.size());
    multiplyImageFilter->SetConstant(1.0/params.K); //Praful - clustering

    multiplyImageFilter->Update();

    MultiplyByConstantImageFilterType::Pointer multiplyImageFilterBeforeWarp = MultiplyByConstantImageFilterType::New();
    multiplyImageFilterBeforeWarp->SetInput(meanDistanceTransformBeforeWarp);

//    multiplyImageFilterBeforeWarp->SetConstant(1.0/params.distanceTransformFilenames.size());
    multiplyImageFilterBeforeWarp->SetConstant(1.0/params.K); //Praful - clustering

    multiplyImageFilterBeforeWarp->Update();

    std::string meanDT_filename;
    std::string meanDTBeforeWarp_filename;

    if (filename.find(".nrrd") != std::string::npos)
    {
        meanDT_filename = params.out_prefix + ".meanDT.nrrd" ;
        meanDTBeforeWarp_filename = params.out_prefix + ".meanDT_beforeWarp.nrrd" ;
    }
    else if (filename.find(".mha") != std::string::npos)
    {
        meanDT_filename = params.out_prefix + ".meanDT.mha" ;
        meanDTBeforeWarp_filename = params.out_prefix + ".meanDT_beforeWarp.mha" ;
    }

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( meanDT_filename.c_str());
    writer->SetInput( multiplyImageFilter->GetOutput() );
    writer->Update();

    writer->SetFileName( meanDTBeforeWarp_filename.c_str());
    writer->SetInput( multiplyImageFilterBeforeWarp->GetOutput() );
    writer->Update();

    // going to vtk to extract the template mesh (mean dense shape)
    // to be deformed for each sparse shape
    ITK2VTKConnectorType::Pointer itk2vtkConnector = ITK2VTKConnectorType::New();
    itk2vtkConnector->SetInput(multiplyImageFilter->GetOutput());
    itk2vtkConnector->Update();
    vtkSmartPointer<vtkPolyData> meanDenseShape = extractIsosurface(itk2vtkConnector->GetOutput(),
                                                                    params.levelsetValue, params.targetReduction, params.featureAngle,
                                                                    params.lsSmootherIterations, params.meshSmootherIterations, params.preserveTopology);

    std::string meanShape_filename = params.out_prefix + ".meanSparseShape.lpts" ;
    utils::writeSparseShape( (char*) meanShape_filename.c_str(), meanSparseShape);

    // Write the file
    std::string outfilename = params.out_prefix + ".meanDenseShapeNoQC.vtk" ;
    vtkPolyDataWriter * polywriter =   vtkPolyDataWriter::New();
    polywriter->SetFileName(outfilename.c_str());
#if (VTK_MAJOR_VERSION < 6)
    polywriter->SetInput(meanDenseShape);
#else
    polywriter->SetInputData(meanDenseShape);
#endif
    polywriter->Update();

    std::string outfilename2 = params.out_prefix + ".meanDenseShapeNoQC.ply" ;
    vtkPLYWriter *plyWriter=vtkPLYWriter::New();
#if (VTK_MAJOR_VERSION < 6)
    plyWriter->SetInput(meanDenseShape);
#else
    plyWriter->SetInputData(meanDenseShape);
#endif
    plyWriter->SetFileName(outfilename2.c_str());
    plyWriter->Update();

    std::string inFilename  = outfilename;
    std::string outFilename = params.out_prefix + ".meanDenseShape.vtk" ;
    MeshQC(inFilename, outFilename, params.qcFixWinding,  params.qcDoLaplacianSmoothingBeforeDecimation, params.qcDoLaplacianSmoothingAfterDecimation,
           params.qcSmoothingLambda, params.qcSmoothingIterations,  params.qcDecimationPercentage);

    vtkPolyDataReader * polyreader = vtkPolyDataReader::New();
    polyreader->SetFileName(outFilename.c_str());
    polyreader->Update();
    vtkSmartPointer<vtkPolyData> meanDenseShapeQCed = polyreader->GetOutput();

    std::string outFilename2 = params.out_prefix + ".meanDenseShape.ply" ;
#if (VTK_MAJOR_VERSION < 6)
    plyWriter->SetInput(meanDenseShapeQCed);
#else
    plyWriter->SetInputData(meanDenseShapeQCed);
#endif
    plyWriter->SetFileName(outFilename2.c_str());
    plyWriter->Update();

    // display the template mesh with the spare mean shape
    if (params.display)
        visMeshWithParticles(meanDenseShape,meanSparseShape, params.glyph_radius, std::string("mean dense shape in red and mean sparse shape in white"));

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

void performKMeansClustering(std::vector<std::string> worldPointsFilenames, unsigned int number_of_particles, int K, std::vector<int> & centroidIndices)
{
    unsigned int number_of_shapes = worldPointsFilenames.size();

//    std::list< vnl_matrix_fixed<double, number_of_particles, 3>> shapeList = new std::list< vnl_matrix_fixed<double, number_of_particles, 3> >[number_of_shapes];
    std::vector<vnl_matrix<double>> shapeList;
//    shapeList.resize(number_of_shapes);
    vnl_matrix<double> shapeVector(number_of_particles, 3, 0.0);

    // Read input shapes from file list
    for (unsigned int shapeNo = 0; shapeNo < number_of_shapes; shapeNo++)
    {
//        shapeVector.clear();
        std::ifstream ifs;
        ifs.open(worldPointsFilenames[shapeNo].c_str());
        for(unsigned int ii = 0 ; ii < number_of_particles; ii++)
        {
            double p[3];
            ifs >> p[0] >> p[1] >> p[2];

            shapeVector[ii][0] = p[0];
            shapeVector[ii][1] = p[1];
            shapeVector[ii][2] = p[2];
        }
        shapeList.push_back(shapeVector);
        ifs.close();
    }

    std::vector<int> centers(K, 0);
    unsigned int seed = unsigned (std::time(0));
    std::srand(seed);
    centers[0] = rand() % number_of_shapes;
    std::cout<<"Setting center[0] to shape #"<<centers[0]<<std::endl;
    int countCenters = 1;
    while(countCenters<K)
    {
        vnl_matrix<double> distMat(number_of_shapes, countCenters, 0.0);
        vnl_vector<double> minDists(number_of_shapes, 0.0);
        vnl_vector<double> probs(number_of_shapes, 0.0);
        for (int s = 0; s<number_of_shapes; s++)
        {
            for (int c = 0; c<countCenters; c++)
            {
                if (s==centers[c])
                {
                    distMat.set_row(s, 0.0);
                    break;
                }
                shapeVector = shapeList[s]-shapeList[centers[c]];
                distMat(s,c) = shapeVector.fro_norm();
            }
            minDists(s) = distMat.get_row(s).min_value();
            probs(s) = minDists(s)*minDists(s);
        }
        probs.operator /=(probs.sum());
        vnl_vector<double> cumProbs(number_of_shapes, 0.0);

        for (int s = 0; s<number_of_shapes; s++)
            cumProbs[s] = probs.extract(s+1, 0).sum();

        double r = (double) (rand() % 10000);
        r = r/10000.0;
        for (int s = 0; s<number_of_shapes; s++)
        {
            if (r<cumProbs[s])
            {    if (probs[s]==0.0)
                    continue;
                else
                {
                    centers[countCenters] = s;
                    countCenters += 1;
                    break;
                }
            }
        }
        std::cout<<"Setting center["<< countCenters-1 <<"] to shape #"<<centers[countCenters-1]<<std::endl;
    }
    std::cout<<"KMeans++ finished...."<<std::endl;
    centroidIndices = centers;
}

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

vtkSmartPointer<vtkPolyData> extractIsosurface(vtkSmartPointer<vtkImageData> volData,float levelsetValue,
                                               float targetReduction,
                                               float featureAngle,
                                               int lsSmootherIterations,
                                               int meshSmootherIterations,
                                               bool preserveTopology)
{
    // (1) isosurface generation
    vtkSmartPointer<vtkContourFilter> ls = vtkSmartPointer<vtkContourFilter>::New();
    //vtkSmartPointer<vtkMarchingCubes> ls = vtkSmartPointer<vtkMarchingCubes>::New();
    //ls->SetInputConnection( reader->GetOutputPort() );
#if (VTK_MAJOR_VERSION < 6)
    ls->SetInput(volData);
#else
    ls->SetInputData(volData);
#endif
    ls->SetValue(0, levelsetValue);
    ls->Update();
    std::cout << "..";

    // (2) laplacian smoothing
    // Description:
    // Construct object with number of iterations as input; relaxation factor .01;
    // feature edge smoothing turned off; feature
    // angle 45 degrees; edge angle 15 degrees; and boundary smoothing turned
    // on. The convergence criterion is 0.0 of the bounding box diagonal.
    vtkSmartPointer<vtkSmoothPolyDataFilter> lsSmoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    lsSmoother->SetInputConnection( ls->GetOutputPort() );
    lsSmoother->SetNumberOfIterations(lsSmootherIterations);
    lsSmoother->Update();
    std::cout << "..";

    // (3) largest connected component (assuming only a single domain shape)
    vtkSmartPointer<vtkPolyDataConnectivityFilter> conn = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    conn->SetExtractionModeToLargestRegion();
#if (VTK_MAJOR_VERSION < 6)
    conn->SetInput(lsSmoother->GetOutput());
#else
    conn->SetInputData(lsSmoother->GetOutput());
#endif
    conn->Update();

    // (4) mesh decimation
    vtkSmartPointer<vtkDecimatePro> decimator = vtkSmartPointer<vtkDecimatePro>::New();
    decimator->SetInputConnection( conn->GetOutputPort() );
    decimator->SetTargetReduction(targetReduction);
    decimator->SetFeatureAngle(featureAngle);
    preserveTopology == true ? decimator->PreserveTopologyOn() : decimator->PreserveTopologyOff();
    decimator->BoundaryVertexDeletionOn();
    decimator->Update();
    std::cout << "..";

    // (5) mesh relaxation
    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    smoother->SetInputConnection(decimator->GetOutputPort());
    smoother->SetNumberOfIterations(meshSmootherIterations);
    smoother->Update();
    std::cout << "..";

    vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
    denseShape = smoother->GetOutput();

    return denseShape;
}

vtkSmartPointer< vtkPoints > computeMeanSparseShape(std::vector< std::string > localPointsFilenames,std::string out_path,
                                                    int number_of_particles, int& medianShapeIndex, bool do_procrustes, bool do_procrustes_scaling)
{
    // (1) define mean sparse shape:
    //          run generalized procrustes on the local points to align all shapes to a common coordinate frame
    Procrustes3D::ShapeListType shapelist;
    Procrustes3D::ShapeType     shapevector;
    Procrustes3D::ShapeType     meanSparseShape;
    Procrustes3D::PointType     point;

    // Read input shapes from file list
    for (unsigned int shapeNo = 0; shapeNo < localPointsFilenames.size(); shapeNo++)
    {
        std::cout << "Loading for MeanSparseShape: " << localPointsFilenames[shapeNo].c_str() << std::endl;
        shapevector.clear();
        std::ifstream ifs;
        ifs.open(localPointsFilenames[shapeNo].c_str());
        if (!ifs)
        {
            std::cerr << "Error loading file: " << localPointsFilenames[shapeNo].c_str() << std::endl;
            throw 1;
        }
        for(unsigned int ii = 0 ; ii < number_of_particles; ii++)
        {
            double p[3];
            ifs >> p[0] >> p[1] >> p[2];

            point(0) = p[0];
            point(1) = p[1];
            point(2) = p[2];

            shapevector.push_back(point);
        }

        shapelist.push_back(shapevector);
        ifs.close();
        std::cout << shapevector.size() << " correspondences read!!!" << std::endl;
    }

    Procrustes3D procrustes;
    Procrustes3D::PointType commonCenter;
    if(do_procrustes)
    {
        // Run alignment
        Procrustes3D::SimilarityTransformListType transforms;
        procrustes.AlignShapes(transforms, shapelist, do_procrustes_scaling); // shapes are actually aligned (modified) and transforms are returned

        // this is the center which needed for translation of the shapes to coincide on the image origin
        // so that the whole object is in the image and won't go outside
        procrustes.ComputeCommonCenter(transforms, commonCenter);

        // Construct transform matrices for each particle system.
        // Procrustes3D::TransformMatrixListType transformMatrices;
        // procrustes.ConstructTransformMatrices(transforms,transformMatrices, do_procrustes_scaling); // note that tranforms scale is updated here if do_scaling ==1
    }
    else
    {
        commonCenter(0) = 0; commonCenter(1) = 0; commonCenter(2) = 0;
    }

    // compute the average sparse shape
    procrustes.ComputeMeanShape(meanSparseShape , shapelist);
    medianShapeIndex = procrustes.ComputeMedianShape(shapelist);

    vtkSmartPointer< vtkPoints > meanSparsePts = vtkSmartPointer< vtkPoints >::New(); // for visualization and estimating kernel support
    for(unsigned int ii = 0 ; ii < number_of_particles; ii++)
    {
        double pt[3];

        pt[0] = meanSparseShape[ii](0) - commonCenter(0);
        pt[1] = meanSparseShape[ii](1) - commonCenter(1);
        pt[2] = meanSparseShape[ii](2) - commonCenter(2);

        meanSparsePts->InsertNextPoint(pt[0], pt[1], pt[2]);
    }

    // write aligned shapes for subsequent statistical analysis
    for (unsigned int shapeNo = 0; shapeNo < localPointsFilenames.size(); shapeNo++)
    {
        std::string outfilename = out_path + "/" + utils::removeExtension(utils::getFilename(localPointsFilenames[shapeNo])) + ".wpts";

        shapevector = shapelist[shapeNo];

        std::ofstream ofs;
        ofs.open(outfilename.c_str());
        for(unsigned int ii = 0 ; ii < number_of_particles; ii++)
        {
            double pt[3];

            pt[0] = shapevector[ii](0) - commonCenter(0);
            pt[1] = shapevector[ii](1) - commonCenter(1);
            pt[2] = shapevector[ii](2) - commonCenter(2);

            ofs << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;

        }
        ofs.close();
    }

    return meanSparsePts;
}

vnl_matrix<double> computeParticlesNormals(string localPointsFilename,
                                           string distanceTransformFilename,
                                           int number_of_particles)
{

    // (1) read the current distance tranform and get the isosurface where particles should live
    auto filename = distanceTransformFilename;

    //    std::cout<<distanceTransformFilename.c_str()<<std::endl;
    //    std::cout<<"HelloHelloHello"<<std::endl;

    if (filename.find(".nrrd") != std::string::npos) {
        itk::NrrdImageIOFactory::RegisterOneFactory();
    } else if (filename.find(".mha") != std::string::npos) {
        itk::MetaImageIOFactory::RegisterOneFactory();
    }
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( distanceTransformFilename.c_str() );
    reader->Update();

    //    std::cout<<"HelloHelloHello"<<std::endl;

    const FloatImageType::SpacingType& spacing     = reader->GetOutput()->GetSpacing();
    const FloatImageType::PointType& origin        = reader->GetOutput()->GetOrigin();

    // (2) get normals at each voxel from the implicit representation
    // Create and setup a gradient filter
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput( reader->GetOutput() );
    gradientFilter->Update();

    GradientMagnitudeFilterType::Pointer gradientMagFilter = GradientMagnitudeFilterType::New();
    gradientMagFilter->SetInput( reader->GetOutput() );
    gradientMagFilter->Update();

    FloatImageType::Pointer gradMagImage = FloatImageType::New();
    gradMagImage = gradientMagFilter->GetOutput();

    GradientImageType::Pointer  gradientImage = GradientImageType::New();
    gradientImage  = gradientFilter->GetOutput();

    // iterate through the gradient to convert it to normal = -gradient/mag(gradient)
    GradientImageIteratorType gradIter(gradientImage, gradientImage->GetRequestedRegion());
    ImageIteratorType magIter(gradMagImage, gradMagImage->GetRequestedRegion());

    // initialize the images that will hold the normal components
    FloatImageType::Pointer nxImage = FloatImageType::New();
    nxImage->SetRegions(reader->GetOutput()->GetLargestPossibleRegion());
    nxImage->Allocate();
    ImageIteratorType nxIter(nxImage, nxImage->GetRequestedRegion());

    FloatImageType::Pointer nyImage = FloatImageType::New();
    nyImage->SetRegions(reader->GetOutput()->GetLargestPossibleRegion());
    nyImage->Allocate();
    ImageIteratorType nyIter(nyImage, nyImage->GetRequestedRegion());

    FloatImageType::Pointer nzImage = FloatImageType::New();
    nzImage->SetRegions(reader->GetOutput()->GetLargestPossibleRegion());
    nzImage->Allocate();
    ImageIteratorType nzIter(nzImage, nzImage->GetRequestedRegion());

    for(gradIter.GoToBegin(), magIter.GoToBegin(), nxIter.GoToBegin(), nyIter.GoToBegin(), nzIter.GoToBegin();
        !gradIter.IsAtEnd();
        ++gradIter, ++magIter,  ++nxIter, ++nyIter, ++nzIter)
    {
        itk::CovariantVector<float,3> grad = gradIter.Get();
        float                      gradMag = magIter.Get();

        float nx = -1.0*grad[0]/(1e-10+ gradMag);
        float ny = -1.0*grad[1]/(1e-10+ gradMag);
        float nz = -1.0*grad[2]/(1e-10+ gradMag);

        nxIter.Set(nx);
        nyIter.Set(ny);
        nzIter.Set(nz);

        //        std::cout << nxIter.Get() << ", " << nyIter.Get() << ", " << nzIter.Get() << std::endl;

    }
    //    std::cout<<"======================================"<<std::endl;

    // going to vtk for probing ...
    ITK2VTKConnectorType::Pointer connector_x = ITK2VTKConnectorType::New();
    connector_x->SetInput(nxImage);
    connector_x->Update();
    vtkSmartPointer<vtkImageData> Nx = vtkSmartPointer<vtkImageData>::New();
    Nx = connector_x->GetOutput();

    ITK2VTKConnectorType::Pointer connector_y = ITK2VTKConnectorType::New();
    connector_y->SetInput(nyImage);
    connector_y->Update();
    vtkSmartPointer<vtkImageData> Ny = vtkSmartPointer<vtkImageData>::New();
    Ny = connector_y->GetOutput();

    ITK2VTKConnectorType::Pointer connector_z = ITK2VTKConnectorType::New();
    connector_z->SetInput(nzImage);
    connector_z->Update();
    vtkSmartPointer<vtkImageData> Nz = vtkSmartPointer<vtkImageData>::New();
    Nz = connector_z->GetOutput();

    //    std::cout<<"HelloHelloHello!!!"<<std::endl;
    vtkSmartPointer<vtkPoints> particles =
            utils::readSparseShape((char*) localPointsFilename.c_str(),  number_of_particles);

    vtkSmartPointer<vtkPolyData> particlesData =
            vtkSmartPointer<vtkPolyData>::New();
    particlesData->SetPoints(particles);

    vtkSmartPointer<vtkPoints> pts = utils::convertToImageCoordinates(particlesData->GetPoints(),
                                                                      particlesData->GetPoints()->GetNumberOfPoints(), spacing, origin);
    particlesData->SetPoints(pts);



    //    std::cout<<"HelloHelloHello@@@"<<std::endl;
    // (4) get the normals by probing the DT-based normal computation
    vtkSmartPointer<vtkProbeFilter> probe_x = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_x->SetInput(particlesData);
    probe_x->SetSource(Nx);
#else
    probe_x->SetInputData(particlesData);
    probe_x->SetSourceData(Nx);
#endif
    probe_x->Update();

    vtkSmartPointer<vtkProbeFilter> probe_y = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_y->SetInput(particlesData);
    probe_y->SetSource(Ny);
#else
    probe_y->SetInputData(particlesData);
    probe_y->SetSourceData(Ny);
#endif
    probe_y->Update();

    vtkSmartPointer<vtkProbeFilter> probe_z = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_z->SetInput(particlesData);
    probe_z->SetSource(Nz);
#else
    probe_z->SetInputData(particlesData);
    probe_z->SetSourceData(Nz);
#endif
    probe_z->Update();

    //std::cout<<"HelloHelloHello222"<<std::endl;

    //    vtkFloatArray* nx = vtkFloatArray::SafeDownCast (probe_x->GetPolyDataOutput()->GetPointData()->GetArray("scalars"));
    //    vtkFloatArray* ny = vtkFloatArray::SafeDownCast (probe_y->GetPolyDataOutput()->GetPointData()->GetArray("scalars"));
    //    vtkFloatArray* nz = vtkFloatArray::SafeDownCast (probe_z->GetPolyDataOutput()->GetPointData()->GetArray("scalars"));

    vtkFloatArray* nx = vtkFloatArray::SafeDownCast (probe_x->GetOutput()->GetPointData()->GetScalars());
    vtkFloatArray* ny = vtkFloatArray::SafeDownCast (probe_y->GetOutput()->GetPointData()->GetScalars());
    vtkFloatArray* nz = vtkFloatArray::SafeDownCast (probe_z->GetOutput()->GetPointData()->GetScalars());

    //    std::cout<<probe_x->GetPolyDataOutput()->GetPointData()->GetScalars()->GetNumberOfComponents()<<std::endl;
    //    std::cout<<probe_x->GetOutput()->GetPointData()->GetScalars()<<std::endl;
    //    std::cout<<nx->GetNumberOfComponents()<<std::endl;

    // Set point normals
    vtkSmartPointer<vtkDoubleArray> pointNormalsArray =
            vtkSmartPointer<vtkDoubleArray>::New();
    pointNormalsArray->SetNumberOfComponents(3); //3d normals (ie x,y,z)
    pointNormalsArray->SetNumberOfTuples(particlesData->GetNumberOfPoints());

    //    std::cout<<"HelloHelloHello222.5"<<std::endl;
    //    std::cout<<nx->GetSize() <<" "<<ny->GetSize()<<" "<<nz->GetSize()<<std::endl;
    vnl_matrix<double> particlesNormals(number_of_particles, 3);

    //    std::cout<<number_of_particles<<"  "<<particlesData->GetNumberOfPoints()<<std::endl;

    //    std::cout<<nx->GetSize() <<" "<<ny->GetSize()<<" "<<nz->GetSize()<<std::endl;

    //    std::cout<<"HelloHelloHello222.6"<<std::endl;

    for(unsigned int ii = 0; ii < particlesData->GetNumberOfPoints(); ii++)
    {
        double pN[3];

        // getting the normals at particles
        pN[0] = nx->GetValue(ii);
        pN[1] = ny->GetValue(ii);
        pN[2] = nz->GetValue(ii);

        // making sure this is a unit vector
        double norm = sqrt(pN[0]*pN[0] + pN[1]*pN[1] + pN[2]*pN[2]);
        pN[0]       = pN[0] / norm;
        pN[1]       = pN[1] / norm;
        pN[2]       = pN[2] / norm;

        particlesNormals(ii, 0) = pN[0];
        particlesNormals(ii, 1) = pN[1];
        particlesNormals(ii, 2) = pN[2];

        double p[3];
        particlesData->GetPoint(ii,p);

        // Add the data to the normals array
        pointNormalsArray->SetTuple(ii, pN) ;

        //        std::cout << "Point " << ii << " : (" << p[0] << " " << p[1] << " " << p[2] << ")" << std::endl;
        //        std::cout << "Point normal " << ii << ": "
        //                  << pN[0] << " " << pN[1] << " " << pN[2] << std::endl;

    }

    //    std::cout<<"HelloHelloHello223"<<std::endl;

    // Add the normals to the points in the polydata
    particlesData->GetPointData()->SetNormals(pointNormalsArray);

    //    std::cout<<"HelloHelloHello333"<<std::endl;

    vtkSmartPointer<vtkPoints> pts2 = utils::convertToPhysicalCoordinates(particlesData->GetPoints(),
                                                                          particlesData->GetPoints()->GetNumberOfPoints(), spacing, origin);
    particlesData->SetPoints(pts2);

    //    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    //    writer->SetInput(particlesData);
    //    writer->SetFileName("normals.vtu");
    //    writer->Update();


    // to be loaded in matlab and do quiver just to make sure it is working
    // Write all of the coordinates of the points in the vtkPolyData to an ascii file.
    //    std::string txtname = std::string("normals.txt" );
    //    std::ofstream ofs;
    //    ofs.open(txtname.c_str());
    //    for(vtkIdType i = 0; i < particlesData->GetNumberOfPoints(); i++)
    //    {
    //        double p[3];
    //        double pN[3];

    //        particlesData->GetPoint(i,p);
    //        // This is identical to:
    //        //polydata->GetPoints()->GetPoint(i,p);

    //        pointNormalsArray->GetTuple(i, pN);

    //        //            std::cout << "Point " << i << " : (" << p[0] << " " << p[1] << " " << p[2] << ")" << std::endl;
    //        //            std::cout << "Point normal " << i << ": "
    //        //                      << pN[0] << " " << pN[1] << " " << pN[2] << std::endl;

    //        ofs << p[0] << " " << p[1] << " " << p[2] << " " << pN[0] << " " << pN[1] << " " << pN[2] << std::endl;
    //    }
    //    ofs.close();
    //    std::cout<<"HelloHelloHello444"<<std::endl;
    return particlesNormals;

}

int MeshQC(std::string inFilename, std::string outFilename, bool fixWinding,  bool doLaplacianSmoothingBeforeDecimation, bool doLaplacianSmoothingAfterDecimation,
           float smoothingLambda, int smoothingIterations,  float decimationPercentage)
{
    // read a VTK file
    FEVTKimport vtk_in;
    FEMesh* pm = vtk_in.Load(inFilename.c_str());

    // make sure we were able to read the file
    if (pm == 0)
    {
        fprintf(stderr, "Does not compute ...\n\n");
        return 0;
    }

    // fix the element winding
    FEFixMesh fix;
    FEMesh* pm_fix;
    if (fixWinding)
        pm_fix = fix.FixElementWinding(pm);

    // do a Laplacian smoothing before decimation
    if (doLaplacianSmoothingBeforeDecimation)
    {
        FEMeshSmoothingModifier lap;
        lap.m_threshold1 = smoothingLambda;
        lap.m_iteration = smoothingIterations;
        pm_fix = lap.Apply(pm_fix);
    }

    // do a CVD decimation
    FECVDDecimationModifier cvd;
    cvd.m_pct = decimationPercentage;
    cvd.m_gradient = 1; // uniform decimation
    pm_fix = cvd.Apply(pm_fix);

    // do a Laplacian smoothing after decimation
    if (doLaplacianSmoothingAfterDecimation)
    {
        FEMeshSmoothingModifier lap;
        lap.m_threshold1 = smoothingLambda;
        lap.m_iteration = smoothingIterations;
        pm_fix = lap.Apply(pm_fix);
    }

    // export to another vtk file
    FEVTKExport vtk_out;
    if (vtk_out.Export(*pm_fix, outFilename.c_str()) == false)
    {
        fprintf(stderr, "Does not compute ...\n\n");
        return 0;
    }

    // don't forget to clean-up
    delete pm_fix;
    delete pm;
    return 1;
}
