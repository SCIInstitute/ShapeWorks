#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>
#include "vnl/vnl_vector.h"

#define _USE_MATH_DEFINES

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "ComputeGeodesicDistanceToLandmark " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Compute geodesic distances on given triangular meshes (ply format) to a set of given landmarks " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide" << std::endl;
        std::cout << "\t the spacing, size and origin of their distance transforms that generated those fids files" << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - fids:      a list of corresponding fids files (in case with_fids = 1)" << std::endl;
        std::cerr << "\t - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform" << std::endl;
        std::cerr << "\t - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform" << std::endl;
        std::cerr << "\t - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform" << std::endl;
        std::cout << "\t - landmark_files: a list of text files containing the 3d point locations of the landmarks associated with each mesh (a single file for each mesh with each row a different landmark xyz)" << std::endl;
        std::cout << "\t - landmark_labels: the name of each landmark specified in landmark files, this will be used in the naming of the output feature files (fea)" << std::endl;
        std::cout << "\t - apply_log: 0 (default) or 1 if need log of geodesic distance as feature" << std::endl;
        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef vnl_vector< double > PointVectorType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    if (!loadOkay)
    {
        std::cerr << "Error in loading xml file..." << std::endl;
        return EXIT_FAILURE;
    }
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::vector< std::string > landmarkFilename;
    std::vector< std::string > landmarkLabels;

    bool withFids = false; // if false use kdtree instead, note that filenames with be affected
    std::vector< std::string > fidsFilename; // save name as mesh but with fids extension, ONLY will be used if with_fids is triggered
    
    // in case of fids since image origin, size and spacing is needed that generated the fids files
    int size_x, size_y, size_z;
    float origin_x, origin_y, origin_z;
    float spacing_x, spacing_y,  spacing_z;
    bool apply_log = false;
    std::string tmpString;

    elem = docHandle.FirstChild( "apply_log" ).Element();
    if (elem)
    {
        atoi(elem->GetText()) > 0 ? apply_log = true : apply_log = false;
    }

    elem = docHandle.FirstChild( "with_fids" ).Element();
    if (elem)
    {
        atoi(elem->GetText()) > 0 ? withFids = true : withFids = false;
    }

    if(withFids)
    {
        elem = docHandle.FirstChild( "fids" ).Element();
        if (!elem)
        {
            std::cerr << "No fids files have been specified and with_fids is triggered" << std::endl;
            throw 1;
        }
        else
        {
            fidsFilename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                fidsFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }


        // voxel spacing in the x-direction
        elem = docHandle.FirstChild( "spacing_x" ).Element();
        if (elem)
        {
            spacing_x = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_x specified!" << std::endl;
            return EXIT_FAILURE;
        }


        // voxel spacing in the y-direction
        elem = docHandle.FirstChild( "spacing_y" ).Element();
        if (elem)
        {
            spacing_y = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // voxel spacing in the z-direction
        elem = docHandle.FirstChild( "spacing_z" ).Element();
        if (elem)
        {
            spacing_z = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_z specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the x-direction
        elem = docHandle.FirstChild( "size_x" ).Element();
        if (elem)
        {
            size_x = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_x specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the y-direction
        elem = docHandle.FirstChild( "size_y" ).Element();
        if (elem)
        {
            size_y = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the z-direction
        elem = docHandle.FirstChild( "size_z" ).Element();
        if (elem)
        {
            size_z = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_z specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the x-direction
        elem = docHandle.FirstChild( "origin_x" ).Element();
        if (elem)
        {
            origin_x = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_x specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the y-direction
        elem = docHandle.FirstChild( "origin_y" ).Element();
        if (elem)
        {
            origin_y = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the z-direction
        elem = docHandle.FirstChild( "origin_z" ).Element();
        if (elem)
        {
            origin_z = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_z specified!" << std::endl;
            return EXIT_FAILURE;
        }

    }

    elem = docHandle.FirstChild( "mesh" ).Element();
    if (!elem)
    {
        std::cerr << "No mesh files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        meshFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            meshFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }


    elem = docHandle.FirstChild( "landmark_files" ).Element();
    if (!elem)
    {
        std::cerr << "No landmark files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            landmarkFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }


    elem = docHandle.FirstChild( "landmark_labels" ).Element();
    if (!elem)
    {
        std::cerr << "No landmark labels have been specified" << std::endl;
        throw 1;
    }
    else
    {
        landmarkLabels.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            landmarkLabels.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    for (int s = 0; s < meshFilename.size(); s++)
    {
        // read mesh
        std::cout << "Reading mesh file: " << meshFilename[s] << std::endl;
        TriMesh *mesh = TriMesh::read( meshFilename[s].c_str() );
        orient(mesh);
        if (mesh->neighbors.empty()) mesh->need_neighbors();
        if (mesh->adjacentfaces.empty()) mesh->need_adjacentfaces();
        if (mesh->normals.empty()) mesh->need_normals();

        if(withFids)
        {
            mesh->ReadFaceIndexMap(fidsFilename[s].c_str());
	    
            mesh->imageOrigin[0] = origin_x;
            mesh->imageOrigin[1] = origin_y;
            mesh->imageOrigin[2] = origin_z;

            // Store spacing of image domain
            mesh->imageSpacing[0] = spacing_x;
            mesh->imageSpacing[1] = spacing_y;
            mesh->imageSpacing[2] = spacing_z;

            // Store size of image domain
            mesh->imageSize[0] = size_x;
            mesh->imageSize[1] = size_y;
            mesh->imageSize[2] = size_z;

        }

        // read landmark points
        std::cout << std::endl << "Reading landmarks file: " << landmarkFilename[s] << std::endl;
        std::ifstream landmarkFile(landmarkFilename[s].c_str());
        std::vector< point > landmarkPts;

        if (!landmarkFile)
        {
            std::cout << "landmark file not found: " << landmarkFilename[s] << std::endl;
            return EXIT_FAILURE;
        }

        while (landmarkFile)
        {
            point tmpPt; tmpPt.clear();

            for (int d = 0; d < 3; d++)
            {
                landmarkFile >> tmpPt[d];
            }
            landmarkPts.push_back(tmpPt);
        }
        landmarkPts.pop_back();

        // loop over landmarks to compute distance feature
        for (int lm = 0; lm < landmarkLabels.size(); lm++)
        {
            std::cout << "Computing geodesics to " <<  landmarkLabels[lm] << " landmark ..." << std::endl;

            // get name of output file
            std::string feaFilename;

            // SHIREEN: save fea files where we find the landmarks not the meshes, similar to compute distance to curves tool
            int index = landmarkFilename[s].rfind("/");
            std::string path = landmarkFilename[s].substr(0, index+1);
            //std::string file = landmarkFilename[s].substr(index+1, landmarkFilename[s].length()-1);

            int index_m = meshFilename[s].rfind("/");
            // fea file will take its prefix from mesh file in case we use different meshs with different decimations
            std::string file = meshFilename[s].substr(index_m+1, meshFilename[s].length()-1);

            index = file.rfind(".");
            if (index == std::string::npos)
            {
                if(withFids)
                {
                    if (apply_log)
                        feaFilename = path + file + ".geoTo_" + landmarkLabels[lm] + "_withFids_withLog.fea";
                    else
                        feaFilename = path + file + ".geoTo_" + landmarkLabels[lm] + "_withFids.fea";
                }
                else
                {
                    if (apply_log)
                        feaFilename = path + file + ".geoTo_" + landmarkLabels[lm] + "_withLog.fea";
                    else
                        feaFilename = path + file + ".geoTo_" + landmarkLabels[lm] + ".fea";
                }
            }
            else
            {
                if(withFids)
                {
                    if (apply_log)
                        feaFilename = path + file.substr(0, index) + ".geoTo_" + landmarkLabels[lm] + "_withFids_withLog.fea";
                    else
                        feaFilename = path + file.substr(0, index) + ".geoTo_" + landmarkLabels[lm] + "_withFids.fea";
                }
                else
                {
                    if (apply_log)
                        feaFilename = path + file.substr(0, index) + ".geoTo_" + landmarkLabels[lm] + "_withLog.fea";
                    else
                        feaFilename = path + file.substr(0, index) + ".geoTo_" + landmarkLabels[lm] + ".fea";
                }
            }

            std::cout << "fea filename to be saved : " << feaFilename << std::endl;

            // SHIREEN: computing geodesics to landmarks based on geodesic approximation
            meshFIM *fim = new meshFIM;
            fim->SetMesh(mesh);
            fim->ComputeDistanceToLandmark(mesh, landmarkPts[lm], apply_log, feaFilename.c_str());
        }
    }

    return EXIT_SUCCESS;
}
