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
 
/*////////////////////////////////////////////////////////////////////////////////////////
ITK IMAGE GENERATION BY PCA
*/////////////////////////////////////////////////////////////////////////////////////////
void ITKPCAProcessing(const char * PCAModeName, const char * MeanName, 
  double sigma, double eigval){
  // add and scalar multiply the scalar
  double factor = sigma * sqrt(eigval);
  const unsigned int Dimension = 3;
  typedef double PixelType;

  typedef itk::Image< PixelType, Dimension > InputImageType;

  // read image one and two
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader_mean = ReaderType::New();
  reader_mean->SetFileName( MeanName );
  ReaderType::Pointer reader_pca = ReaderType::New();
  reader_pca->SetFileName( PCAModeName );

  // set up the writer
  typedef itk::Image< PixelType, Dimension >  OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "compute.nrrd" );

  // multiply the image by factor 
  typedef itk::MultiplyImageFilter< InputImageType, InputImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader_pca->GetOutput() );
  filter->SetConstant( factor );

  // add two image filter
  typedef itk::AddImageFilter <InputImageType, InputImageType >
    AddImageFilterType;
 
  AddImageFilterType::Pointer addFilter
    = AddImageFilterType::New ();
  addFilter->SetInput1( filter->GetOutput() );
  addFilter->SetInput2( reader_mean->GetOutput() );
  addFilter->Update();

  writer->SetInput( addFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    }

}

/*////////////////////////////////////////////////////////////////////////////////////////
ITK THRESHOLD AND MARCHING CUBES
*/////////////////////////////////////////////////////////////////////////////////////////

void ITKThreshold(){
  const char * inputFileName = "compute.nrrd";
  const unsigned int Dimension = 3;
  typedef double  PixelType;

  typedef itk::Image< PixelType, Dimension >   InputImageType;
  typedef itk::Image< PixelType, Dimension >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFileName);

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

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
    writer->SetInput( threshold->GetOutput() );
    writer->SetFileName("thresh.nrrd");
    writer->Update();
  // return 0;
}

void ITKMarchingCubes(){
  const char * inputFileName = "thresh.nrrd";
  const unsigned int Dimension = 3;
  typedef double  PixelType;

  typedef itk::Image< PixelType, Dimension >   InputImageType;
  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFileName);

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    }

  typedef itk::Mesh<double> MeshType;

  typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType >   MeshSourceType;

  MeshSourceType::Pointer meshSource = MeshSourceType::New();

  const PixelType objectValue = static_cast<PixelType>( 1 );

  meshSource->SetObjectValue( objectValue );
  meshSource->SetInput( reader->GetOutput() );

  typedef itk::MeshFileWriter< MeshType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "mesh.obj" );
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