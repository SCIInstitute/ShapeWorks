
#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <climits>

// C++/STL
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <set>
#include <iterator>
#include "math.h"

// Etc
#include "tinyxml.h"
#include "vnl/vnl_vector.h"


int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ComputeMeshGeodesics " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Compute pairwise vertices geodesic distances on given triangular meshes (ply format)" << std::endl;

        std::cerr << "It uses a parameter file with the following tags" << std::endl;
        std::cerr << "\t - mesh: a list of ply file names for meshes to be processed" << std::endl;
	std::cerr << "\t - geo: a list of geo file names for meshes to be saved" << std::endl;
	std::cerr << "\t - stopping_distance_factor: percentage of the great circle circumference where vertices will be assigned an very large geodesic distance" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }
    
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::vector< std::string > geoFilename;
    double stopping_distance_factor = 0.1f; // percentage of the great circle circumference
  
    std::string tmpString;
    if(loadOkay)
    {
        elem = docHandle.FirstChild( "mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                meshFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        
        elem = docHandle.FirstChild( "geo" ).Element();
        if (!elem)
        {
            std::cerr << "No geo files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                geoFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        
        elem = docHandle.FirstChild( "stopping_distance_factor" ).Element();
        if (elem)
	{
            stopping_distance_factor = atof( elem->GetText() );
        }
    }
    else
    {
        std::cerr << "param file can not be opened ..." << std::endl;
        throw 1;
    }

    int numShapes = meshFilename.size();
    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {
        //size_t ind = meshFilename[shCount].find_last_of('.');
        //std::string prefix = meshFilename[shCount].substr(0,ind);
        //std::cout << "prefix: " << prefix << std::endl;

        //std::string geoFileName = prefix +  ".geo";

        // read triangle mesh and set feature values at vertices
        TriMesh *mesh = TriMesh::read( meshFilename[shCount].c_str() );
        if (mesh == NULL)
        {
            std::cout << "Can't compute geo files (unreadable mesh) for " << meshFilename[shCount] << std::endl;
            continue;
        }
        mesh->need_faces();
        mesh->need_neighbors();
        orient(mesh);

        // bounding sphere
        mesh->need_bsphere();

        // Test for Colors
        if (mesh->colors.empty()){
            for(int i=0; i < mesh->vertices.size(); i++){
                mesh->colors.push_back(Color(1.0f, 1.0f, 1.0f));
            }
        }

        // Recompute values that are no longer correct
        if (!mesh->normals.empty()) mesh->normals.clear();
        mesh->need_normals();

        if (!mesh->adjacentfaces.empty()) mesh->adjacentfaces.clear();
        mesh->need_adjacentfaces();

        if (!mesh->across_edge.empty()) mesh->across_edge.clear();
        mesh->need_across_edge();

        if (!mesh->tstrips.empty()) mesh->tstrips.clear();
        mesh->need_tstrips();

        mesh->need_faceedges();
        mesh->need_oneringfaces();
        mesh->need_abs_curvatures();
        mesh->need_speed();
        mesh->setSpeedType(1);
        mesh->need_kdtree();

        double stopDist = mesh->bsphere.r * PI * 2.0f * stopping_distance_factor;

        meshFIM *fim = new meshFIM;
        fim->SetMesh(mesh);
        fim->SetStopDistance( stopDist );

        int numVert = mesh->vertices.size();
        mesh->geodesicMap.resize(numVert);
        fim->computeFIM(mesh,geoFilename[shCount].c_str());

        std::cout << "Done\n";

    }

    return EXIT_SUCCESS;
}
