#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

#include "tinyxml.h"
#include "Utils.h"
#include <sys/stat.h> // for mkdir
#ifdef WIN32
#include <direct.h>
#endif

class InputParams
{
public:

    // correspondence model parameters
    int number_of_particles ;
    bool do_procrustes;
    bool do_procrustes_scaling;
    const float pi = std::acos(-1.0);
    // where to save output files:
    //   WarpToMeanSpace: the template (DT, dense and sparse shapes) and warped DT
    //   WarpToSubjectSpace: the warped dense mean to each individual subject
    //   MoveAlongPCAModes:  the warped dense mean to samples from the shape space sampled along individual pca modes
    //   WarpToMeanSpaceWithPreviewMeshQC: same as WarpToMeanSpace but the resulting mesh is QCed via preview mesh processing tools
    std::string out_prefix;
    std::string out_path;

    // used in case of WarpToSubjectSpace or MoveAlongPCAModes
    std::string mean_prefix;
    std::string mean_path;

    // parameters for mesh QC via preview
    bool qcFixWinding;
    bool qcDoLaplacianSmoothingBeforeDecimation;
    bool qcDoLaplacianSmoothingAfterDecimation;
    float qcSmoothingLambda;
    int qcSmoothingIterations;
    float qcDecimationPercentage;

    float K;
    // this is number of particles to be used to build the warp, their choice will be based on the ranking if point files are provided -->
    // otherwise, the choice will be random
    int number_of_correspondences_in_warp;

    // attributes that can be used to help quantifying the quality of each correspondence, e.g. sulcal depth for the brains
    int number_of_attributes;
    std::string attributes_prefix;
    std::vector< std::string> attribute_labels;

    // isosurface extraction parameters
    float levelsetValue ;
    float targetReduction;
    float featureAngle   ;
    int   lsSmootherIterations;
    int   meshSmootherIterations;
    bool  preserveTopology    ;
    bool  display             ;
    float glyph_radius        ;
    float maximum_variance_captured;
    float maximum_std_dev;
    int number_of_samples_per_mode;
    float normalAngle;

    bool use_tps_transform;
    bool use_bspline_interpolation;

    // input files
    std::vector< std::string > localPointsFilenames;
    std::vector< std::string > worldPointsFilenames; // optional, wpts from shapeworks (procrustes in this tool need more debuging)
    std::vector< std::string > distanceTransformFilenames;
    std::vector< std::string > attributeFilenames;
    //std::vector< std::string > landmarksFilenames;

    InputParams(){

        // correspondence model parameters
        number_of_particles               = -1;
        do_procrustes                     =  true;
        do_procrustes_scaling             =  true;
        number_of_correspondences_in_warp = -1;
        number_of_attributes              =  0;
        K = -1;
        // isosurface parameters
        levelsetValue          = 0.5; // to make sure no holes (don't use strict 0.0 since this will isosurface a mean distance transform - i.e. having non-zero isosurface)
        targetReduction        = 0.01; // percentage to decimate
        featureAngle           = 30; // in degrees
        lsSmootherIterations   = 1; // number of iterations to smooth the levelset
        meshSmootherIterations = 1; // number of iterations to smooth the initial mesh
        preserveTopology       = true;

        qcFixWinding = true;
        qcDoLaplacianSmoothingBeforeDecimation = true;
        qcDoLaplacianSmoothingAfterDecimation = true;
        qcSmoothingLambda = 0.5;
        qcSmoothingIterations = 1;
        qcDecimationPercentage = 0.5;

        display                = true;
        glyph_radius           = 1;

        maximum_variance_captured  = 0.95;
        maximum_std_dev            = 2.0;
        number_of_samples_per_mode = 10;
        normalAngle = pi/2.0;

        use_tps_transform         = false;
        use_bspline_interpolation = false;

        localPointsFilenames.clear();
        worldPointsFilenames.clear();
        distanceTransformFilenames.clear();
        attribute_labels.clear();
    }

    int readParams(char* infilename, int mode ) // 0 - WarpToMeanSpace, 1 - WarpToSubjectSpace, 2 - MoveAlongPCAModes, 3 - WarpToMeanSpaceWithPreviewMeshQC
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

            // reconstruction template parameters
            elem = docHandle.FirstChild( "use_tps_transform" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? use_tps_transform = true : use_tps_transform = false;
            }

            elem = docHandle.FirstChild( "use_bspline_interpolation" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? use_bspline_interpolation = true : use_bspline_interpolation = false;
            }

            if (mode == 0 || mode == 3 || mode == 1) // WarpToMeanSpace or WarpToMeanSpaceWithPreviewMeshQC  or WarpToSubject
            {
                // Compile the list of input files.
                elem = docHandle.FirstChild( "local_point_files" ).Element();
                if (elem)
                {
                    inputsBuffer.str(elem->GetText());
                    while (inputsBuffer >> filename)
                    {
                        localPointsFilenames.push_back(filename);
                    }
                    inputsBuffer.clear();
                    inputsBuffer.str("");
                }
                else
                {
                    std::cerr << "No local points to process!" << std::endl;
                    return EXIT_FAILURE;
                }
            }

            if (mode == 0 || mode == 3 || mode == 2) // WarpToMeanSpace or WarpToMeanSpaceWithPreviewMeshQC  or MoveAlongPCAModes
            {
                elem = docHandle.FirstChild( "world_point_files" ).Element();
                if (elem)
                {
                    inputsBuffer.str(elem->GetText());
                    while (inputsBuffer >> filename)
                    {
                        worldPointsFilenames.push_back(filename);
                    }
                    inputsBuffer.clear();
                    inputsBuffer.str("");
                }
                else
                {
                    std::cerr << "No world points (wpts) to process .. procrustes will be used for the given lpts" << std::endl;
                }
            }
            if (mode == 0 || mode == 3) // WarpToMeanSpace or WarpToMeanSpaceWithPreviewMeshQC
            {
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

                //Praful
                elem = docHandle.FirstChild("number_of_clusters").Element();
                if (elem)
                {
                    K = atof(elem->GetText());
                }
                else
                {
                    std::cout<<"Warning: All shapes will be used to generate mean distance transform (could be slower)!"<<std::endl;
                }
            }

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


            if (mode == 1 || mode == 2) // WarpToSubjectSpace or MoveAlongPCAModes
            {
                elem = docHandle.FirstChild( "mean_prefix" ).Element();
                if (elem)
                {
                    inputsBuffer.str(elem->GetText());
                    inputsBuffer >> mean_prefix;
                    inputsBuffer.clear();
                    inputsBuffer.str("");
                }
                else
                {
                    std::cerr << "No mean_prefix provided ...!" << std::endl;
                    return EXIT_FAILURE;
                }
            }

            elem = docHandle.FirstChild( "number_of_particles" ).Element();
            if (elem)
            {
                number_of_particles = atoi( elem->GetText() );
            }

            elem = docHandle.FirstChild( "number_of_correspondences_in_warp" ).Element();
            if (elem)
            {
                number_of_correspondences_in_warp = atoi( elem->GetText() );
            }
            else
            {
                std::cerr << "No number_of_correspondences_in_warp provided, all good correspondences will be used to build the warp ...!" << std::endl;
            }

            if (mode == 0 || mode == 3) // WarpToMeanSpace or WarpToMeanSpaceWithPreviewMeshQC
            {
                elem = docHandle.FirstChild( "number_of_attributes" ).Element();
                if (elem)
                {
                    number_of_attributes = atoi( elem->GetText() );
                }

                elem = docHandle.FirstChild( "attributes_prefix" ).Element();
                if (elem)
                {
                    inputsBuffer.str(elem->GetText());
                    inputsBuffer >> attributes_prefix;
                    inputsBuffer.clear();
                    inputsBuffer.str("");
                }
                else
                {
                    if(number_of_attributes > 0)
                    {
                        std::cerr << "No attributes_prefix provided ...!" << std::endl;
                        return EXIT_FAILURE;
                    }
                }


                elem = docHandle.FirstChild( "attribute_labels" ).Element();
                if (!elem)
                {
                    if(number_of_attributes > 0)
                    {
                        std::cerr << "No attribute files have been specified" << std::endl;
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    inputsBuffer.str(elem->GetText());
                    while (inputsBuffer >> filename)
                    {
                        attribute_labels.push_back(filename);
                    }
                    inputsBuffer.clear();
                    inputsBuffer.str("");
                }

                elem = docHandle.FirstChild( "do_procrustes" ).Element();
                if (elem)
                {
                    atoi(elem->GetText()) > 0 ? do_procrustes = true : do_procrustes = false;
                }


                elem = docHandle.FirstChild( "do_procrustes_scaling" ).Element();
                if (elem)
                {
                    atoi(elem->GetText()) > 0 ? do_procrustes_scaling = true : do_procrustes_scaling = false;
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

                // read normal angle value if given
                elem = docHandle.FirstChild( "normalAngle" ).Element();
                if (elem)
                {
                    normalAngle = atof( elem->GetText() )*pi/180.0;
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
            }

            // check if we want to display the computed template
            elem = docHandle.FirstChild( "display" ).Element();
            if (elem)
            {
                atoi(elem->GetText()) > 0 ? display = true : display = false;
            }

            elem = docHandle.FirstChild( "glyph_radius" ).Element();
            if (elem)
            {
                glyph_radius = atof( elem->GetText() );
            }

            if (mode == 0 || mode == 3) // WarpToMeanSpace or WarpToMeanSpaceWithPreviewMeshQC
            {
                // Make sure lists are the same size.
                if (localPointsFilenames.size() != distanceTransformFilenames.size())
                {
                    std::cerr << "local points file list size does not match distance transform file list size!" << std::endl;
                    return EXIT_FAILURE;
                }
            }

            if (mode == 2) // MoveAlongPCAModes
            {
                elem = docHandle.FirstChild( "maximum_variance_captured" ).Element();
                if (elem)
                {
                    maximum_variance_captured = atof( elem->GetText() );
                }

                elem = docHandle.FirstChild( "maximum_std_dev" ).Element();
                if (elem)
                {
                    maximum_std_dev = atof( elem->GetText() );
                }

                elem = docHandle.FirstChild( "number_of_samples_per_mode" ).Element();
                if (elem)
                {
                    number_of_samples_per_mode = atoi( elem->GetText() );
                }
            }

            if (mode == 3) // WarpToMeanSpaceWithPreviewMeshQC
            {

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

            if (number_of_particles <= 0)
            {
                std::cerr << "Invalide number of particles, it should be > 0 ...!" << std::endl;
                return EXIT_FAILURE;
            }

            if(number_of_correspondences_in_warp <= 0)
                number_of_correspondences_in_warp = number_of_particles;

            if(number_of_correspondences_in_warp > number_of_particles)
            {
                number_of_correspondences_in_warp = number_of_particles;
                std::cerr << "number_of_correspondences_in_warp exceeds the total number of particles, all correspondences will be used to build the warp ...!" << std::endl;
            }

            // create the directory where the results should be, in case it is not there
            out_path = Utils::getPath(out_prefix);

            //            if(use_tps_transform)
            //                out_path = out_path + "TPS";
            //            else
            //                out_path = out_path + "RBF";

            out_prefix = out_path + "/" + Utils::getFilename(out_prefix);

            // remove the output if it already exists to remove any historical results, just in case
            // std::string cmdStr = "rm -rf " + out_path;
            // std::system(cmdStr.c_str());
#ifdef WIN32
            int mkdirStatus = _mkdir(out_path.c_str());
#else
            int mkdirStatus = mkdir(out_path.c_str(), S_IRWXU);
#endif

            // copy the current param file to the output to keep track of the parameters that generated this output
            std::string cmdStr = "cp " + std::string(infilename) + " " + out_path;
            std::system(cmdStr.c_str());

            if (mode == 1 || mode == 2) // WarpToSubjectSpace or MoveAlongPCAModes
            {
                // create the directory where the results should be, in case it is not there
                mean_path = Utils::getPath(mean_prefix) + "/" ;

                //                if(use_tps_transform)
                //                    mean_path = mean_path + "TPS";
                //                else
                //                    mean_path = mean_path + "RBF";

                mean_prefix = mean_path + Utils::getFilename(mean_prefix);
            }

        }
        return EXIT_SUCCESS;
    }
};

#endif // INPUTPARAMS_H
