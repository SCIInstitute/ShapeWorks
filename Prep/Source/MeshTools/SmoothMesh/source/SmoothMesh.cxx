
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

//------------- VTK ---------------------
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

#include <vtkSmoothPolyDataFilter.h>

using namespace std;

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "SmoothMesh " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given a set of meshes (vtk), this tool apply laplacian smoothing." << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - inputs: a list of vtk file names for meshes to be processed" << std::endl;
        std::cerr << "\t - outputs: the vtk filenames of the output to be produced " << std::endl;
        std::cerr << "\t - iterations: number of iterations " << std::endl;
        std::cerr << "\t - relaxation_factor: amount of displacement for a vertex to move in each iteration" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string >      inFilenames; inFilenames.clear();
    std::vector< std::string >      outFilenames; outFilenames.clear();

    int numShapes = 0;
    int iterations = 10;
    float relaxation_factor = 0.01;

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
        elem = docHandle.FirstChild( "inputs" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
            numShapes = inFilenames.size();
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "outputs" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                outFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No outputs specified!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "iterations" ).Element();
        if (elem)
        {
            iterations = atoi( elem->GetText() );
        }

        elem = docHandle.FirstChild( "relaxation_factor" ).Element();
        if (elem)
        {
            relaxation_factor = atof( elem->GetText() );
        }

    }

    for (unsigned int shapeNo = 0; shapeNo < numShapes; shapeNo++)
    {
        std::cout << "Reading mesh for " << inFilenames[shapeNo].c_str()  << std::endl ;

        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(inFilenames[shapeNo].c_str());
        reader->Update();

        vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();

        vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
                vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
        smoothFilter->SetInputData(mesh);
        smoothFilter->SetNumberOfIterations(iterations);
        smoothFilter->SetRelaxationFactor(relaxation_factor);
        smoothFilter->FeatureEdgeSmoothingOff();
        smoothFilter->BoundarySmoothingOn();
        smoothFilter->Update();

        // Update normals on newly smoothed polydata
        //vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
        //normalGenerator->SetInputConnection(smoothFilter->GetOutputPort());
        //normalGenerator->ComputePointNormalsOn();
        //normalGenerator->ComputeCellNormalsOn();
        //normalGenerator->Update();

        std::cout << "Wrting output mesh:" << outFilenames[shapeNo].c_str()  << std::endl ;
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInputData(smoothFilter->GetOutput());
        writer->SetFileName(outFilenames[shapeNo].c_str());
        writer->Update();
    }


    return EXIT_SUCCESS;
}


