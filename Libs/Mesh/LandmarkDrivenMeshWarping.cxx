
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "TriMesh.h"
#include "meshFIM.h"
#include <Eigen/Dense>

//------------- VTK ---------------------
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#define MAXBUFSIZE  ((int) 1e6)

template<typename M>
M load_mat (const std::string & path) {
    std::ifstream indata;
    indata.open(path);
    std::string line;
    std::vector<double> values;
    uint rows = 0;
    while (std::getline(indata, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ' ')) {
            values.push_back(std::stod(cell));
        }
        ++rows;
    }
    return Map<const Matrix<typename M::Scalar, M::RowsAtCompileTime, M::ColsAtCompileTime, RowMajor>>(values.data(), rows, values.size()/rows);
}



int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "LandmarkDrivenMeshWarping " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given a template mesh and template landmarks, we define a warping matrix." << std::endl;
		std::cerr << "Using the set of target landmarks and the warping matrix we warp the template mesh to generate a set of output target meshes." << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - input_landmarks: a list of .particles file names for the target landmarks" << std::endl;
        std::cerr << "\t - output_meshes: the vtk filenames of the warped output meshes to be produced " << std::endl;
		std::cerr << "\t - template_landmarks: .particles file defining the template landmark positions" << std::endl;
        std::cerr << "\t - template_mesh: the vtk template mesh file " << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string >      inLandmarkFilenames; inLandmarkFilenames.clear();
    std::vector< std::string >      outMeshFilenames; outMeshFilenames.clear();
	std::string templateLandmarkName;
	std::string templateMeshName;
	
    int numShapes = 0;
	int numPoints = 0
    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        // Compile the list of input files.
        elem = docHandle.FirstChild( "input_landmarks" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inLandmarkFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
            numShapes = inLandmarkFilenames.size();
        }
        else
        {
            std::cerr << "No input landmarks to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "output_meshes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                outMeshFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No output meshs specified!" << std::endl;
            return EXIT_FAILURE;
        }

		elem = docHandle.FirstChild( "template_mesh" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> templateMeshName;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No template mesh specified!" << std::endl;
            return EXIT_FAILURE;
        }

		elem = docHandle.FirstChild( "template_landmark" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> templateLandmarkName;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No template landmarks specified!" << std::endl;
            return EXIT_FAILURE;
        }

    }

	// define the mapping using the landmark and the mesh
	std::cout << "Creating the mapping matrix using the template landmarks and mesh ... " << std::endl;
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(templateMeshName.c_str());
    reader->Update();

	
    vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();
	vtkSmartPointer<vtkPoints> points = mesh->GetPoints();
	vtkSmartPointer<vtkDataArray> dataArray = points->GetData();
	// read the points and save it in an Eigen matrix
	Eigen::MatrixXd templateLandmarks = load_mat<Eigen::MatrixXd>(templateLandmarkName);
	

    return EXIT_SUCCESS;
}