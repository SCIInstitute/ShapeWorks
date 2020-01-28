#include <iostream>
#include <math.h>

// ITK Dependencies
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImage.h"
#include "itkQuadEdgeMesh.h"
#include "itkMeshFileWriter.h"
#include "itkMultiplyImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkMeshFileReader.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter.h"
#include "itkMesh.h"
// #include <igl/readOBJ.h>
#include <itkeigen/Eigen/Sparse>

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
    const unsigned int Dimension = 3;
	  typedef double  PixelType;
	  typedef itk::Image< PixelType, Dimension >   InputImageType;
	  typedef itk::Image< PixelType, Dimension >   OutputImageType;
	  typedef itk::ImageFileReader< InputImageType >    ReaderType;
	  ReaderType::Pointer reader = ReaderType::New();
	  reader->SetFileName(inFilenames[shapeNo].c_str());
	  try
	    {
	    reader->Update();
	    }
	  catch( itk::ExceptionObject & exp )
	    {
	    std::cerr << "Exception thrown while reading the input file " << std::endl;
	    std::cerr << exp << std::endl;
	    }
	  PixelType lowerThreshold = static_cast< PixelType >( levelsetValue );
	  PixelType upperThreshold = static_cast< PixelType >( 1000 );
	  typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdFilterType;
	    BinaryThresholdFilterType::Pointer threshold = BinaryThresholdFilterType::New();
	    threshold->SetInput( reader->GetOutput() );
	    threshold->SetLowerThreshold( lowerThreshold );
	    threshold->SetUpperThreshold( upperThreshold );
	    threshold->SetOutsideValue( 0 );
	    threshold->SetInsideValue( 1 );
	    
	  typedef itk::Mesh<double> MeshType;
	  typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType >   MeshSourceType;
	  MeshSourceType::Pointer meshSource = MeshSourceType::New();
	  const PixelType objectValue = static_cast<PixelType>( 1 );
	  meshSource->SetObjectValue( objectValue );
	  meshSource->SetInput( threshold->GetOutput() );
  
	  typedef itk::MeshFileWriter< MeshType > WriterType;
	  WriterType::Pointer writer = WriterType::New();
	  writer->SetFileName( outFilenames[shapeNo].c_str() );
	  writer->SetInput( meshSource->GetOutput() );
	  try
	    {
	    writer->Update();
	    }
	  catch( itk::ExceptionObject & error )
	    {
	    std::cerr << "Error: " << error << std::endl;
	    }
	}
  return EXIT_SUCCESS;
}
