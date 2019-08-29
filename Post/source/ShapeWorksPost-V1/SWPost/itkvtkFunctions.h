#ifndef _itkvtkFunctions
#define _itkvtkFunctions

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
#include <igl/readOBJ.h>
#include <itkeigen/Eigen/Sparse>

// VTK Dependencies

/* Compilations of all the ITK functions needed for the visualization 
1) ITKPCA 
2) Thresholding
3) Points to Mesh Conversion
4) ...
*/

void meshDecimation(std::string inputFileName, float decimationPercent){
  // the input must be an obj file (convert before calling decimation)
  const unsigned int Dimension = 3;
  typedef float TCoordinate;
  typedef itk::QuadEdgeMesh< TCoordinate, Dimension > TMesh;
  typedef itk::MeshFileReader< TMesh > TReader;
  typedef itk::MeshFileWriter< TMesh > TWriter;
  typedef itk::NumberOfFacesCriterion<TMesh>    CriterionType;
  typedef itk::SquaredEdgeLengthDecimationQuadEdgeMeshFilter<TMesh,
                                                           TMesh,
                                                           CriterionType> DecimationType;
  TReader::Pointer reader = TReader::New();
  reader->SetFileName(inputFileName.c_str());
  Eigen::MatrixXd VtempS;
  Eigen::MatrixXi FtempS;
  igl::readOBJ(inputFileName.c_str(), VtempS, FtempS);
  
  std::cout << "Initial Number of Triangles : "<< FtempS.rows() <<std::endl; 
  std::cout << "Performing Mesh Decimation ... " << std::endl;
  int numTFinal = floor(FtempS.rows() * decimationPercent);
  std::cout << "Final Number of Triangles : " << numTFinal <<std::endl;

  CriterionType::Pointer criterion = CriterionType::New();
  DecimationType::Pointer decimate = DecimationType::New();
  criterion->SetTopologicalChange(false);
  criterion->SetNumberOfElements(numTFinal);
 
  decimate->SetInput(reader->GetOutput());
  decimate->SetCriterion(criterion);
  decimate->Update();
  TWriter::Pointer writer = TWriter::New();
  writer->SetFileName( "TemplateMeshDecimated.obj" );
  writer->SetInput( decimate->GetOutput() );
  writer->Update();
}


void convertVTKtoOBJ(std::string inputFileName){

  // tested for VTK to obj but should work for any input format
  const unsigned int Dimension = 3;
  typedef float TCoordinate;

  typedef itk::Mesh< TCoordinate, Dimension > TMesh;
  typedef itk::MeshFileReader< TMesh > TReader;
  typedef itk::MeshFileWriter< TMesh > TWriter;
  TReader::Pointer reader = TReader::New();
  reader->SetFileName(inputFileName.c_str());
  TWriter::Pointer writer = TWriter::New();
  writer->SetFileName( "TemplateMesh.obj" );
  writer->SetInput( reader->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    }

}

void itkMeshfromDT(std::string inputFileName){

  const unsigned int Dimension = 3;
  typedef double  PixelType;

  typedef itk::Image< PixelType, Dimension >   InputImageType;
  typedef itk::Image< PixelType, Dimension >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFileName.c_str());

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    }

  PixelType lowerThreshold = static_cast< PixelType >( 0 );
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
  writer->SetFileName( "TemplateMesh.obj" );
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
#endif
