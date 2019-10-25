
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

#include <vtkFillHolesFilter.h>

using namespace std;

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "FillMeshHoles " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given a set of meshes (vtk) and corresponding cutting planes, this tool finds holes in a mesh and closes them." << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - inputs: a list of vtk file names for meshes to be processed" << std::endl;
        std::cerr << "\t - outputs: the vtk filenames of the output to be produced " << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string >      inFilenames; inFilenames.clear();
    std::vector< std::string >      outFilenames; outFilenames.clear();

    int numShapes = 0;

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

    }

    for (unsigned int shapeNo = 0; shapeNo < numShapes; shapeNo++)
    {
        std::cout << "Reading mesh for " << inFilenames[shapeNo].c_str()  << std::endl ;

        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(inFilenames[shapeNo].c_str());
        reader->Update();

        vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();

        vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter =
                vtkSmartPointer<vtkFillHolesFilter>::New();
#if VTK_MAJOR_VERSION <= 5
        fillHolesFilter->SetInputConnection(mesh->GetProducerPort());
#else
        fillHolesFilter->SetInputData(mesh);
#endif
        fillHolesFilter->SetHoleSize(1000.0);

        // Make the triangle windong order consistent
        vtkSmartPointer<vtkPolyDataNormals> normals =
                vtkSmartPointer<vtkPolyDataNormals>::New();
        normals->SetInputConnection(fillHolesFilter->GetOutputPort());
        normals->ConsistencyOn();
        normals->SplittingOff();
        normals->Update();

        // Restore the original normals
        normals->GetOutput()->GetPointData()->
                SetNormals(mesh->GetPointData()->GetNormals());

        std::cout << "Wrting output mesh:" << outFilenames[shapeNo].c_str()  << std::endl ;
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInputData(normals->GetOutput());
        writer->SetFileName(outFilenames[shapeNo].c_str());
        writer->Update();
    }


    return EXIT_SUCCESS;
}


