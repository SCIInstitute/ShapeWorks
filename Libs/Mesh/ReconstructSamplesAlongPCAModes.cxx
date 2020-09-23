

/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2019/08/31 17:10:25 $
  Version:   $Revision: 1.1 $
  Author:    $Author: elhabian $
  Purpose:   particle-based sampling of shape space with a dense surface reconstruction, (use output of ReconstructMeanSurface)
                (1) using the wpts from ReconstructMeanSurface, compute the PCA model
                (2) move along dominant components (using scree test) +/- factor * std using the sparse correspondence model
                (3) warp the mean dense shape (mesh) constructed from WarpToMeanSpace to each individual sample shape along pca modes

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "ParticleBasedSurfaceReconstruction_InputParams.h"
#include "Vis.h"
#include "Reconstruction.h"

#include "itkNrrdImageIOFactory.h"
#include "itkMetaImageIOFactory.h"

// for pca modes
#include "itkParticleShapeStatistics.h"

template < template < typename TCoordRep, unsigned > class TTransformType,
           template < typename ImageType, typename TCoordRep > class TInterpolatorType,
           typename TCoordRep, typename PixelType, typename ImageType>
int DoIt(InputParams params)
{
    int domainsPerShape    = 1;
    const unsigned int Dimension = 3;

    typedef Reconstruction < TTransformType, TInterpolatorType, TCoordRep, PixelType, ImageType> ReconstructionType;
    typedef typename ReconstructionType::PointArrayType                                          PointArrayType;
    typedef typename ReconstructionType::PointType                                               PointType;

    ReconstructionType reconstructor;
    ParticleShapeStatistics shapeStats;

    std::string denseFilename      ;
    std::string sparseFilename     ;
    std::string goodPointsFilename ;

    if(params.use_template_mesh)
    {
        denseFilename      = params.template_dense_mesh;
        sparseFilename     = params.template_sparse_points;
        goodPointsFilename = ""; // assume that all are good points
        if (!params.template_good_indices.empty())
            goodPointsFilename = params.template_good_indices;
    }
    else
    {
        denseFilename      = params.mean_prefix + "_dense.vtk";
        sparseFilename     = params.mean_prefix + "_sparse.particles";
        goodPointsFilename = params.mean_prefix + "_goodPoints.txt";
    }

    std::cout << "denseFilename: "  << denseFilename << std::endl;
    std::cout << "sparseFilename: " << sparseFilename << std::endl;
    std::cout << "goodPointsFilename: " << goodPointsFilename << std::endl;

    reconstructor.readMeanInfo(denseFilename, sparseFilename, goodPointsFilename);

    // read-in the world/global coordinates of the given sparse correspondence model (using the output of ReconstructMeanSurface)
    std::vector< std::vector<PointType> > global_pts;
    for (unsigned int shapeNo = 0; shapeNo < params.worldPointsFilenames.size(); shapeNo++)
    {
        std::vector<PointType> curShape; curShape.clear();

        std::string curfilename = params.worldPointsFilenames[shapeNo];
        std::cout << "curfilename: " << curfilename << std::endl;

        Utils::readSparseShape(curShape,(char*)curfilename.c_str());

        std::cout << "curShape.size() = " << curShape.size() << "-------------\n";
        global_pts.push_back(curShape);
    }

    // perform PCA on the global points that were used to compute the dense mean mesh
    shapeStats.DoPCA(global_pts, domainsPerShape);

    std::vector<double> percentVarByMode = shapeStats.PercentVarByMode();
    int TotalNumberOfModes = percentVarByMode.size();

    int NumberOfModes = 0; bool singleModeToBeGen = false;
    if ((params.mode_index >= 0) && (params.mode_index < TotalNumberOfModes))
    {
        NumberOfModes = 1; singleModeToBeGen = true;
        std::cout << "Mode #" << params.mode_index << " is requested to be generated  ..." << std::endl;
    }
    else {
        if (params.number_of_modes > 0 ){
            NumberOfModes = std::min(params.number_of_modes, TotalNumberOfModes);
            std::cout << NumberOfModes << " dominant modes are requested to be generated  ..." << std::endl;
        }
        else {
            // detect number of modes
            bool found = false;
            for (int n = TotalNumberOfModes-1; n >=0; n--)
            {
                if (percentVarByMode[n] >= params.maximum_variance_captured && found==false)
                {
                    NumberOfModes = n;
                    found = true;
                }
            }

            if(!found)
                NumberOfModes = percentVarByMode.size();

            if (NumberOfModes == 0)
            {
                std::cerr << "No dominant modes detected !!!!!" << std::endl;
                return EXIT_FAILURE;
            }
        }
        std::cout << NumberOfModes << " dominant modes is found to capture " << params.maximum_variance_captured*100 << "% of total variation ..." << std::endl;
    }

    // start sampling along each mode
    vnl_vector<double> eigenValues  = shapeStats.Eigenvalues();
    vnl_matrix<double> eigenVectors = shapeStats.Eigenvectors();
    vnl_vector<double> mean         = shapeStats.Mean();

    vtkSmartPointer< vtkPoints > meanPts = vtkSmartPointer< vtkPoints >::New();
    for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
    {
        double pt[3];
        for (unsigned int d = 0 ; d < Dimension; d++)
            pt[d] = mean(ii*Dimension + d);

        meanPts->InsertNextPoint(pt);
    }

    if (params.display)
        Vis::visParticles(meanPts, reconstructor.SparseMean(), params.glyph_radius, "PCA mean sparse shape in red and ReconstructMean sparse in green");

    std::cout << "eigenValues: " ;
    for (unsigned int sid = 0 ; sid < eigenValues.size(); sid++)
        std::cout << eigenValues[sid] << ", " ;
    std::cout << std::endl;

    std::string prefix = Utils::getFilename(params.out_prefix);
    if(!prefix.empty())
        prefix = prefix + "_";

    for (int modeId = 0 ; modeId < TotalNumberOfModes; modeId ++)
    {
        if (singleModeToBeGen && (modeId != params.mode_index))
            continue;

        if ((!singleModeToBeGen) && (modeId >= NumberOfModes))
            break;

        std::string modeStr  = Utils::int2str(modeId, 2);
        std::string cur_path = params.out_path + "/mode-" + modeStr;
        int mkdirStatus;
#ifdef WIN32
        mkdirStatus = _mkdir(cur_path.c_str());
#else
        mkdirStatus = mkdir(cur_path.c_str(), S_IRWXU);
#endif

        double sqrt_eigenValue = sqrt(eigenValues[TotalNumberOfModes - modeId - 1]);

        double min_std_factor = -1 * params.maximum_std_dev ;
        double max_std_factor = +1 * params.maximum_std_dev ;
        std::vector<double> std_factor_store = Utils::linspace(min_std_factor, max_std_factor, params.number_of_samples_per_mode);

        //double min_std = -1 * params.maximum_std_dev * sqrt_eigenValue;
        //double max_std = +1 * params.maximum_std_dev * sqrt_eigenValue;
        //std::vector<double> std_store = Utils::linspace(min_std, max_std, params.number_of_samples_per_mode);

        vnl_vector<double> curMode = eigenVectors.get_column(TotalNumberOfModes - modeId - 1);

        std::vector<double> std_store;
        std::cout << "std_store: " ;
        for (unsigned int sid = 0 ; sid < std_factor_store.size(); sid++)
        {
            std_store.push_back(std_factor_store[sid]*sqrt_eigenValue);
            std::cout << std_store[sid] << ", " ;
        }
        std::cout << std::endl;

        // writing stds on file
        std::string stdfilename = cur_path + "/" + prefix + "mode-" + modeStr + "_stds.txt";
        ofstream ofs(stdfilename.c_str());

        if ( !ofs )
            throw std::runtime_error("Could not open file for output: " + stdfilename);

        for (unsigned int sid = 0 ; sid < std_factor_store.size(); sid++)
            ofs << std_factor_store[sid] << "\n" ;
        ofs.close();

        for(unsigned int sampleId = 0 ; sampleId < std_store.size(); sampleId++)
        {
            std::string sampleStr = Utils::int2str(int(sampleId), 3);

            std::string basename = prefix + "mode-" + modeStr + "_sample-" + sampleStr ;
            //std::string basename =  "_mode-" + modeStr + "_sample-" + sampleStr ;

            std::cout << "generating mode #" + Utils::num2str((float)modeId) + ", sample #" + Utils::num2str((float)sampleId) << std::endl;

            // generate the sparse shape of the current sample
            double cur_std = std_store[sampleId];

            std::cout << "cur_std: " << cur_std << std::endl;

            vnl_vector<double> curSample = mean + cur_std * curMode;

            // fill-in the vtkpoints structure to perform warping for dense reconstruction of the current sample
            vtkSmartPointer< vtkPoints > curSamplePts = vtkSmartPointer< vtkPoints >::New();
            PointArrayType curSparse;
            for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
            {
                double pt[3];
                for (unsigned int d = 0 ; d < Dimension; d++)
                    pt[d] = curSample(ii*Dimension + d);

                PointType p;
                p[0] = pt[0]; p[1] = pt[1]; p[2] = pt[2];

                curSamplePts->InsertNextPoint(pt);
                curSparse.push_back(p);
            }

            if(params.display)
                Vis::visParticles(meanPts, curSamplePts, params.glyph_radius, std::string(basename + ": mean sparse shape in red and sample particles in green"));

            vtkSmartPointer<vtkPolyData> curDense = reconstructor.getMesh(curSparse);

            std::string outfilename = cur_path + "/" + basename + "_dense.vtk";
            std::cout << "Writing: " << outfilename << std::endl;

            vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
            writer->SetFileName(outfilename.c_str());
            writer->SetInputData(curDense);
            writer->Update();

            vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
            vertices->DeepCopy( curDense->GetPoints() );

            std::string ptsfilename = cur_path + "/" + basename + "_dense.particles";
            Utils::writeSparseShape((char*) ptsfilename.c_str(), vertices);

            ptsfilename = cur_path + "/" + basename + "_sparse.particles";
            Utils::writeSparseShape((char*) ptsfilename.c_str(), curSamplePts);

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
    params.readParams(argv[1], 2); // 2 - MoveAlongPCAModes
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
