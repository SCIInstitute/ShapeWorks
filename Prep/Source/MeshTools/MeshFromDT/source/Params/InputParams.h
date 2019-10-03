#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

#include "tinyxml.h"
#include "Utils.h"

class InputParams
{
public:

    // parameters for mesh QC via preview
    bool qcFixWinding;
    bool qcDoLaplacianSmoothingBeforeDecimation;
    bool qcDoLaplacianSmoothingAfterDecimation;
    float qcSmoothingLambda;
    int qcSmoothingIterations;
    float qcDecimationPercentage;

    float K;
    std::string out_prefix;
    // isosurface extraction parameters
    float levelsetValue ;
    float targetReduction;
    float featureAngle   ;
    int   lsSmootherIterations;
    int   meshSmootherIterations;
    bool  preserveTopology    ;
    int   subdivision    ;
    bool  butterfly_subdivision ;

    std::vector< std::string > distanceTransformFilenames;
    std::vector< std::string > meshFilenames;

    InputParams(){

        // correspondence model parameters
        K = -1;
        // isosurface parameters
        levelsetValue          = 0.5; // to make sure no holes (don't use strict 0.0 since this will isosurface a mean distance transform - i.e. having non-zero isosurface)
        targetReduction        = 0.01; // percentage to decimate
        featureAngle           = 30; // in degrees
        lsSmootherIterations   = 1; // number of iterations to smooth the levelset
        meshSmootherIterations = 1; // number of iterations to smooth the initial mesh
        preserveTopology       = true;
        subdivision            = 0;
        butterfly_subdivision = true;

        qcFixWinding = true;
        qcDoLaplacianSmoothingBeforeDecimation = true;
        qcDoLaplacianSmoothingAfterDecimation = true;
        qcSmoothingLambda = 0.5;
        qcSmoothingIterations = 1;
        qcDecimationPercentage = 0.5;

        distanceTransformFilenames.clear();
        meshFilenames.clear();
    }

    int readParams(char* infilename)
    {
        // read parameters
        TiXmlDocument doc(infilename);
        bool loadOkay = doc.LoadFile();

        if (loadOkay)
        {
            TiXmlHandle docHandle( &doc );
            TiXmlElement *elem;
            std::istringstream inputsBuffer;
            std::string filename("/dev/null\0");

            elem = docHandle.FirstChild( "out_prefix" ).Element();
            if (elem)
            {
                inputsBuffer.str(elem->GetText());
                inputsBuffer >> out_prefix;
                inputsBuffer.clear();
                inputsBuffer.str("");
            }
            else
            {
                std::cerr << "No out_prefix provided ...!" << std::endl;
                return EXIT_FAILURE;
            }

            elem = docHandle.FirstChild( "distance_transform_files" ).Element();
            if (elem)
            {
                inputsBuffer.str(elem->GetText());
                while (inputsBuffer >> filename)
                {
                    distanceTransformFilenames.push_back(filename);
                }
                inputsBuffer.clear();
                inputsBuffer.str("");
            }
            else
            {
                std::cerr << "No distance transforms to process!" << std::endl;
                return EXIT_FAILURE;
            }

            elem = docHandle.FirstChild( "output_mesh_files" ).Element();
            if (elem)
            {
                inputsBuffer.str(elem->GetText());
                while (inputsBuffer >> filename)
                {
                    meshFilenames.push_back(filename);
                }
                inputsBuffer.clear();
                inputsBuffer.str("");
            }
            else
            {
                std::cerr << "No output filenames to write!" << std::endl;
                return EXIT_FAILURE;
            }

            // read levelset value if given
            elem = docHandle.FirstChild( "levelsetValue" ).Element();
            if (elem)
            {
                levelsetValue = atof( elem->GetText() );
            }

            // read target reduction value if given
            elem = docHandle.FirstChild( "targetReduction" ).Element();
            if (elem)
            {
                targetReduction = atof( elem->GetText() );
            }

            // read feature angle value if given
            elem = docHandle.FirstChild( "featureAngle" ).Element();
            if (elem)
            {
                featureAngle = atof( elem->GetText() );
            }

            // read number of iterations for levelset smoother if given
            elem = docHandle.FirstChild( "lsSmootherIterations" ).Element();
            if (elem)
            {
                lsSmootherIterations = atoi( elem->GetText() );
            }

            // read number of iterations for mesh smoother if given
            elem = docHandle.FirstChild( "meshSmootherIterations" ).Element();
            if (elem)
            {
                meshSmootherIterations = atoi( elem->GetText() );
            }

            // check if topology changes are allowed
            elem = docHandle.FirstChild( "preserveTopology" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? preserveTopology = true : preserveTopology = false;
            }

            elem = docHandle.FirstChild( "subdivision" ).Element();
            if (elem)
            {
                subdivision = atoi( elem->GetText() );
            }

            elem = docHandle.FirstChild( "butterfly_subdivision" ).Element();
            if (elem)
            {
                butterfly_subdivision = atoi(elem->GetText()) > 0 ? true : false;
            }

            elem = docHandle.FirstChild( "levelsetValue" ).Element();
            if (elem)
            {
                levelsetValue = atof( elem->GetText() );
            }

            // Make sure lists are the same size.
            if (meshFilenames.size() != distanceTransformFilenames.size())
            {
                std::cerr << "mesh file list size does not match distance transform file list size!" << std::endl;
                return EXIT_FAILURE;
            }

            elem = docHandle.FirstChild( "qcFixWinding" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? qcFixWinding = true : qcFixWinding = false;
            }

            elem = docHandle.FirstChild( "qcDoLaplacianSmoothingBeforeDecimation" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? qcDoLaplacianSmoothingBeforeDecimation = true : qcDoLaplacianSmoothingBeforeDecimation = false;
            }

            elem = docHandle.FirstChild( "qcDoLaplacianSmoothingAfterDecimation" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? qcDoLaplacianSmoothingAfterDecimation = true : qcDoLaplacianSmoothingAfterDecimation = false;
            }

            elem = docHandle.FirstChild( "qcSmoothingLambda" ).Element();
            if (elem)
            {
                qcSmoothingLambda = atof( elem->GetText() );
            }

            elem = docHandle.FirstChild( "qcSmoothingIterations" ).Element();
            if (elem)
            {
                qcSmoothingIterations = atoi( elem->GetText() );
            }

            elem = docHandle.FirstChild( "qcDecimationPercentage" ).Element();
            if (elem)
            {
                qcDecimationPercentage = atof( elem->GetText() );
            }
        }

        return EXIT_SUCCESS;
    }
};

#endif // INPUTPARAMS_H
