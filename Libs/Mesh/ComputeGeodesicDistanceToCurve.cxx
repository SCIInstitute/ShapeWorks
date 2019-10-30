#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>

#define _USE_MATH_DEFINES

#include "itkImage.h"
#include "itkImageFileReader.h"

typedef  float  PixelType;
typedef itk::Image< PixelType,  3 >   ImageType;
typedef itk::ImageFileReader< ImageType >  ImageReaderType;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "ComputeGeodesicDistanceToCurve " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Compute geodesic distances on given triangular meshes (ply format) to a set of given curves " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide" << std::endl;
        std::cout << "\t the spacing, size and origin of their distance transforms that generated those fids files" << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - fids:      a list of corresponding fids files (in case with_fids = 1)" << std::endl;
        std::cerr << "\t - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform" << std::endl;
        std::cerr << "\t - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform" << std::endl;
        std::cerr << "\t - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform" << std::endl;
        
        std::cout << "\t - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files" << std::endl;
        std::cout << "\t - curves_suffix: string (optional) that will suffix all curve files" << std::endl;
        std::cout << "\t - curves_ext: the extension of the curve files" << std::endl;
        std::cout << "\t - curve_labels: the name of each curve according to the suffix given for each curve pts file, this will also be used in the naming of the output feature files (fea)" << std::endl;

        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }


    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::vector< std::string > curveLabels;
    std::string curves_prefix;
    std::string curves_suffix = "";
    std::string curves_ext = "lpts";

    bool withFids = false; // if false use kdtree instead, note out filenames with be affected
    std::vector< std::string > fidsFilename; // save name as mesh but with fids extension, ONLY will be used if with_fids is triggered
    
    // in case of fids since image origin, size and spacing is needed that generated the fids files
    int size_x, size_y, size_z;
    float origin_x, origin_y, origin_z;
    float spacing_x, spacing_y,  spacing_z;

    std::string tmpString;

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

    elem = docHandle.FirstChild( "curve_labels" ).Element();
    if (!elem)
    {
        std::cerr << "No curve labels have been specified" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            curveLabels.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "curves_prefix" ).Element();
    if (!elem)
    {
        std::cerr << "No curves_prefix have been specified" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        inputsBuffer.str(elem->GetText());
        inputsBuffer >> curves_prefix;
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "curves_suffix" ).Element();
    if (elem)
    {
        inputsBuffer.str(elem->GetText());
        inputsBuffer >> curves_suffix;
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "curves_ext" ).Element();
    if (elem)
    {
        inputsBuffer.str(elem->GetText());
        inputsBuffer >> curves_ext;
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
        
        for (int c = 0; c < curveLabels.size(); c++)
        {
            // read curve points
            int index = meshFilename[s].rfind("/");
            std::string path = meshFilename[s].substr(0, index+1);
            std::string file = meshFilename[s].substr(index+1, meshFilename[s].length()-1);
            std::string curveFileName;
            std::string feaFilename;
            
            index = file.rfind(".");
            if (index == std::string::npos)
            {
                curveFileName = curves_prefix + file + "_" + curveLabels[c] + "." + curves_ext;
                
                if(withFids)
                    feaFilename = curves_prefix + file + ".geoTo_" + curveLabels[c] + "_withFids.fea";
                else
                    feaFilename = curves_prefix + file + ".geoTo_" + curveLabels[c] + ".fea";
            }
            else
            {
                curveFileName = curves_prefix + file.substr(0, index) + "_" + curveLabels[c] + "." + curves_ext;
                
                if(withFids)
                    feaFilename = curves_prefix + file.substr(0, index) + ".geoTo_" + curveLabels[c] + "_withFids.fea";
                else
                    feaFilename = curves_prefix + file.substr(0, index) + ".geoTo_" + curveLabels[c] + ".fea";
            }

            std::cout << "curve filename: " << curveFileName << std::endl;
            std::cout << "fea filename: " << feaFilename << std::endl;

            std::ifstream curveFile(curveFileName.c_str());
            std::vector< point > curvePts;

            if (!curveFile)
            {
                std::cout << "Curve file not found" << std::endl;
                return EXIT_FAILURE;
            }

            while (curveFile)
            {
                point tmpPt; tmpPt.clear();

                for (int d = 0; d < 3; d++)
                {
                    curveFile >> tmpPt[d];
                }
                curvePts.push_back(tmpPt);
            }
            curvePts.pop_back();

            // set up FIM object and compute distance to curve
            meshFIM *fim = new meshFIM;
            fim->SetMesh(mesh);
            fim->ComputeDistanceToCurve(mesh,curvePts,feaFilename.c_str());
        }
    }

    return EXIT_SUCCESS;
}

