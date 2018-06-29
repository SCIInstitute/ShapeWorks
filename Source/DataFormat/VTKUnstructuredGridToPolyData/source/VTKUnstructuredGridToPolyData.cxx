 
#include "vtkGeometryFilter.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "vtkMarchingCubes.h"
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataReader.h"
#include "vtkXMLUnstructuredDataReader.h"
#include "vtkShrinkFilter.h"

void convertUnstructuredGridToPolyData( vtkSmartPointer<vtkUnstructuredGrid> data, vtkSmartPointer<vtkPolyData> &output)
{
    vtkSmartPointer<vtkPoints> verts = vtkSmartPointer<vtkPoints>::New();
    for(vtkIdType n = 0; n < data->GetNumberOfPoints(); n++) {
        double pt[3];
        data->GetPoint(n, pt);

        verts->InsertNextPoint(pt[0], pt[1], pt[2]);
    }

    output = vtkSmartPointer<vtkPolyData>::New();
    output->SetPoints( verts );

    vtkCellArray* cells = data->GetCells();
    output->SetPolys( cells );
}

int main(int argc, char *argv[])
{
    // check input
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "VTKUnstructuredGridToPolyData " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Convert vtk unstructered grid data to vtk polydata. It uses a paramfile of the following tags:" << std::endl;
        std::cerr << "\t - inputs:  list of input vtk files containing unstructured grid data" << std::endl;
        std::cerr << "\t - outputs: list of output vtk files which will be saved containing the vtk polydata version of the corresponding input vtkfiles" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::vector< std::string > outFilenames; outFilenames.clear();

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
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        // Compile the list of output files.
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

        // Make sure lists are the same size.
        if (inFilenames.size() > outFilenames.size())
        {
            std::cerr << "Input list size does not match output list size!" << std::endl;
            return EXIT_FAILURE;
        }
    }


    for (unsigned int shapeNo = 0; shapeNo < inFilenames.size(); shapeNo++)
    {
        vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
        reader->SetFileName( inFilenames[shapeNo].c_str() );
        reader->Update();
        std::cout << "processing unstructured mesh: " << inFilenames[shapeNo].c_str() << "..";

        vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
        convertUnstructuredGridToPolyData( reader->GetOutput(), mesh );

        //    vtkSmartPointer<vtkGeometryFilter> geomfilter = vtkSmartPointer<vtkGeometryFilter>::New();
        //    geomfilter->SetInputConnection( reader->GetOutputPort() );
        //    geomfilter->Update();

        vtkSmartPointer<vtkPolyDataWriter> wr = vtkSmartPointer<vtkPolyDataWriter>::New();
        wr->SetInput(mesh);
        wr->SetFileName( outFilenames[shapeNo].c_str() );
        wr->Update();

        std::cout << " Done !" << std::endl;
    }

    return EXIT_SUCCESS;
}
