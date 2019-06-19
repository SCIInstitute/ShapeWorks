
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

//------------- VTK ---------------------
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkProbeFilter.h>

//--------------- ITK --------------------------
#include <itkImage.h>
#include <itkImageFileReader.h>

//--------- ITK <-> VTK ---------------
#include "itkImageToVTKImageFilter.h"


using namespace std;

//------------- typedefs ---------------
const unsigned int Dimension = 3;
typedef float      InputPixelType;
typedef float      OutputPixelType;

// basic types
typedef itk::Image< InputPixelType, Dimension >  InputImageType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

// IO
typedef itk::ImageFileReader< InputImageType >   ReaderType;

// connectors
typedef itk::ImageToVTKImageFilter<InputImageType>  ITK2VTKConnectorType;

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ProbeFeatureVolumesAtMeshVertices " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given a set of meshes (vtk) and corresponding feature volumes where these meshes live, this tool probe the feature volumes at each mesh vertex and " << std::endl;
        std::cerr << "output vtk meshes with scalar field defined based on such probing process (report the feature values from the feature volumes at the mesh vertices)" << std::endl;

        std::cerr << "It uses a parameter file with the following tags" << std::endl;
        std::cerr << "\t - input_meshes: a list of vtk file names for meshes to be processed" << std::endl;
        std::cerr << "\t - feature_volumes: a list of image files (3D) to be probed corresponding to the given set of meshes" << std::endl;
        std::cerr << "\t - output_meshes: the vtk filenames of the output to be produced " << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::vector< std::string > outFilenames; outFilenames.clear();
    std::vector< std::string > volFilenames; volFilenames.clear();

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
        elem = docHandle.FirstChild( "input_meshes" ).Element();
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

        elem = docHandle.FirstChild( "output_meshes" ).Element();
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

        elem = docHandle.FirstChild( "feature_volumes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                volFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No feature volumes to probe!" << std::endl;
            return EXIT_FAILURE;
        }

    }

    for (unsigned int shapeNo = 0; shapeNo < inFilenames.size(); shapeNo++)
    {
        std::cout << "Reading mesh for " << inFilenames[shapeNo].c_str()  << std::endl ;

        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(inFilenames[shapeNo].c_str());
        reader->Update();

        vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();

        std::cout << "Reading feature volume for " << volFilenames[shapeNo].c_str()  << std::endl ;

        ReaderType::Pointer volReader = ReaderType::New();
        volReader->SetFileName( volFilenames[shapeNo].c_str() );
        volReader->Update();

        // going to vtk to extract the mesh to the current shape to define geodesics to landmarks
        ITK2VTKConnectorType::Pointer itk2vtkConnector2 = ITK2VTKConnectorType::New();
        itk2vtkConnector2->SetInput(volReader->GetOutput());

        std::cout << "Probing feature volume ... " << std::endl ;

        // now probe the current attribute by the isosurfaces
        vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
        probeFilter->SetInput(mesh);
        probeFilter->SetSource(itk2vtkConnector2->GetOutput());
        probeFilter->Update();

        std::cout << "Wrting output mesh with features for " << outFilenames[shapeNo].c_str()  << std::endl ;

        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInput(probeFilter->GetPolyDataOutput());
        writer->SetFileName(outFilenames[shapeNo].c_str());
        writer->Update();
    }


    return EXIT_SUCCESS;
}


