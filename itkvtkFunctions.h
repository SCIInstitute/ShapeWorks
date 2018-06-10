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
#include "itkMeshFileWriter.h"
#include "itkMultiplyImageFilter.h"
#include "itkAddImageFilter.h"

// VTK Dependencies

/* Compilations of all the ITK functions needed for the visualization 
1) ITKPCA 
2) Thresholding
3) Points to Mesh Conversion
4) ...
*/
 

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