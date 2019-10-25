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
        std::cout << "ComputeCurvatureAndCoordFiles " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Compute curvature and x,y,z on vertices on given triangular meshes (ply format) " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - outPath:   path to save feature files " << std::endl;
        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef vnl_vector< double > PointVectorType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::string tmpString;
    std::string outPath;
    std::string feaFilename;

    if(!loadOkay)
    {
        std::cout<<"Failed to load parameter file..."<<std::endl;
        return EXIT_FAILURE;
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

    elem = docHandle.FirstChild( "outPath" ).Element();
    if (!elem)
    {
        std::cerr << "No outPath has been specified" << std::endl;
        throw 1;
    }
    else
    {
        outPath = elem->GetText();
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
        mesh->need_curvatures();
        mesh->need_abs_curvatures();

        int index = meshFilename[s].rfind("/");
        std::string file = meshFilename[s].substr(index+1, meshFilename[s].length()-index-5);

        meshFIM *fim = new meshFIM;
        fim->SetMesh(mesh);


        feaFilename = outPath +"/" + file + ".curvature.fea";
        std::cout<<"-------------------------"<<std::endl;
        std::cout<<feaFilename.c_str()<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
        fim->computeCurvFiles(mesh, feaFilename.c_str());

        feaFilename = outPath +"/" + file + ".Xcoord.fea";
        std::cout<<"-------------------------"<<std::endl;
        std::cout<<feaFilename.c_str()<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
        fim->computeCoordXFiles(mesh, feaFilename.c_str());

        feaFilename = outPath +"/" + file + ".Ycoord.fea";
        std::cout<<"-------------------------"<<std::endl;
        std::cout<<feaFilename.c_str()<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
        fim->computeCoordYFiles(mesh, feaFilename.c_str());

        feaFilename = outPath +"/"+ file + ".Zcoord.fea";
        std::cout<<"-------------------------"<<std::endl;
        std::cout<<feaFilename.c_str()<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
        fim->computeCoordZFiles(mesh, feaFilename.c_str());
    }

    return EXIT_SUCCESS;
}

