// #include "TriMesh.h"
// #include "meshFIM.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Mesh.h"
#include "MeshUtils.h"
#include "ParticleSystem.h"
using namespace shapeworks;

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        std::cout << "Usage: " << argv[0] << " plyMeshFilePath inPointsPath  outMeshFilePath outPointsPath numParticles" << std::endl;
        return EXIT_FAILURE;
    }
    std::string inMesh  = std::string(argv[1]);
    std::string inPoint   = std::string(argv[2]);
	std::string outMesh  = std::string(argv[3]);
    std::string outPoint   = std::string(argv[4]);
	int numP = atoi(argv[5]);

	// read the points
	Eigen::MatrixXd Vcontrol_static;
	Eigen::MatrixXd Vcontrol_moving;

  Mesh ellipsoid( inMesh );
  Eigen::MatrixXd Vref = MeshUtils::distilVertexInfo(ellipsoid);
  Eigen::MatrixXi Fref = MeshUtils::distilFaceInfo(ellipsoid);
  std::string staticPath = inPoint;
  std::string movingPath = outPoint;
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);
  staticPoints.resize(3, numP);
  movingPoints.resize(3, numP);
  std::cout << Fref.rows() << " " << staticPoints.rows() << " " << Vref.size() << std::endl;
  Eigen::MatrixXd W = MeshUtils::generateWarpMatrix(Vref, Fref, staticPoints.transpose());
  Mesh output = MeshUtils::warpMesh(movingPoints.transpose(), W, Fref);
  output.write(outMesh);
  return 0;
}
