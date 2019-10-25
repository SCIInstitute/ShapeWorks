#include <iostream>
#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"
#include "tinyxml.h"
#include <sstream>
#include <string>
#include "vnl/vnl_vector.h"

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        std::cout << "Usage: " << argv[0] << "meshFile(.ply) pointsFile outFileName" << std::endl;
        return EXIT_FAILURE;
    }

    TriMesh *mesh = TriMesh::read(argv[1]);
    orient(mesh);
    if (mesh->neighbors.empty()) mesh->need_neighbors();
    if (mesh->adjacentfaces.empty()) mesh->need_adjacentfaces();
    if (mesh->normals.empty()) mesh->need_normals();
    meshFIM *fim = new meshFIM;
    fim->SetMesh(mesh);
    fim->ComputeDistanceToLandmarksGivenTriangleInfo(mesh, argv[2], argv[3]);
    return 0;
}
