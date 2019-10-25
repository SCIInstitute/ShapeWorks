#include "vtkMetaImageReader.h"
#include "vtkSmartPointer.h"
#include "vtkContourFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkPLYWriter.h"
#include "vtkPolyDataWriter.h"
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "vtkMarchingCubes.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

int main(int argc, char *argv[])
{
  // preliminaries
  if( argc < 2 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " paramfile " << std::endl; 
    //std::cerr << argv[0] << " inDT outMesh lsval" << std::endl; 
    return EXIT_FAILURE;
  }

  // variables
  std::vector< std::string > inFilenames; inFilenames.clear();
  std::vector< std::string > outFilenames; outFilenames.clear();
  float levelsetValue = 0.0;
  float targetReduction = 0.01; // percentage to decimate
  float featureAngle = 30; // in degrees
  int lsSmootherIterations = 1; // number of iterations to smooth the levelset
  int meshSmootherIterations = 1; // number of iterations to smooth the initial mesh
  bool preserveTopology = true;

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

    // read levelset value if given
    elem = docHandle.FirstChild( "levelsetValue" ).Element();
    if (elem)
    {
      levelsetValue = atof( elem->GetText() );
    }

    // read target reduction value if given
    elem = docHandle.FirstChild( "targetReduction" ).Element();
    if (elem)
    {
      targetReduction = atof( elem->GetText() );
    }

    // read feature angle value if given
    elem = docHandle.FirstChild( "featureAngle" ).Element();
    if (elem)
    {
      featureAngle = atof( elem->GetText() );
    }

    // read number of iterations for levelset smoother if given
    elem = docHandle.FirstChild( "lsSmootherIterations" ).Element();
    if (elem)
    {
      lsSmootherIterations = atoi( elem->GetText() );
    }

    // read number of iterations for mesh smoother if given
    elem = docHandle.FirstChild( "meshSmootherIterations" ).Element();
    if (elem)
    {
      meshSmootherIterations = atoi( elem->GetText() );
    }


    // check if topology changes are allowed
    elem = docHandle.FirstChild( "preserveTopology" ).Element();
    if (elem)
    {
      atoi(elem->GetText()) > 0 ? preserveTopology = true : preserveTopology = false;
    }

    // Make sure lists are the same size.
    if (inFilenames.size() > outFilenames.size())
    {
      std::cerr << "Input list size does not match output list size!" << std::endl;
      return EXIT_FAILURE;
    }
  }

  // create meshes from all input distance transforms at particular levelsetValue
  for (unsigned int shapeNo = 0; shapeNo < inFilenames.size(); shapeNo++)
  {
    
    typedef itk::Image< double, 3 >         ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;
 
    ReaderType::Pointer reader = ReaderType::New();
    ConnectorType::Pointer connector = ConnectorType::New();
    
    reader->SetFileName(inFilenames[shapeNo].c_str());
    connector->SetInput(reader->GetOutput());
      
      //vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
      //reader->SetFileName( inFilenames[shapeNo].c_str() );
      //reader->Update();
      std::cout << "processing DT: " << inFilenames[shapeNo].c_str() << "..";
      
      vtkSmartPointer<vtkContourFilter> ls = vtkSmartPointer<vtkContourFilter>::New();
      //vtkSmartPointer<vtkMarchingCubes> ls = vtkSmartPointer<vtkMarchingCubes>::New();
      //ls->SetInputConnection( reader->GetOutputPort() );
      ls->SetInputData( connector->GetOutput() );
      ls->SetValue(0, levelsetValue);
      ls->Update();
      std::cout << "..";
      
      vtkSmartPointer<vtkSmoothPolyDataFilter> lsSmoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
      lsSmoother->SetInputConnection( ls->GetOutputPort() );
      lsSmoother->SetNumberOfIterations(lsSmootherIterations);
      lsSmoother->Update();
      std::cout << "..";

      vtkSmartPointer<vtkDecimatePro> decimator = vtkSmartPointer<vtkDecimatePro>::New();
      decimator->SetInputConnection( lsSmoother->GetOutputPort() );
      decimator->SetTargetReduction(targetReduction);
      decimator->SetFeatureAngle(featureAngle);
      preserveTopology == true ? decimator->PreserveTopologyOn() : decimator->PreserveTopologyOff();
      decimator->BoundaryVertexDeletionOn();
      decimator->Update();
      std::cout << "..";

      vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
      smoother->SetInputConnection(decimator->GetOutputPort());
      smoother->SetNumberOfIterations(meshSmootherIterations);
      smoother->Update();
      std::cout << "..";

      vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetInputConnection(smoother->GetOutputPort());
      writer->SetFileTypeToASCII();
      writer->SetFileName( outFilenames[shapeNo].c_str() );
      writer->Update();
      std::cout << " Done !" << std::endl;
    
        
    //vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
    //writer->SetInputConnection(smoother->GetOutputPort());
    //writer->SetFileName( outFilenames[shapeNo].c_str() );
    //writer->Update();
    //std::cout << " Done !" << std::endl;
  }

  return EXIT_SUCCESS;
}
 
