#include "vtkPolyDataToImageData.h"

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPolyDataPointSampler.h"

// ITK includes
#include "itkImage.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"

// either from ItkVtkGlue or just copied in
#include "itkImageToVTKImageFilter.h"

typedef itk::Image< unsigned char, 3 > ImageType;

ImageType::Pointer BinaryErodeFilter3D( ImageType::Pointer & img, unsigned int ballsize )
{
  typedef itk::BinaryBallStructuringElement<unsigned char, 3>                     KernalType;
  typedef itk::BinaryErodeImageFilter<ImageType, ImageType, KernalType> ErodeFilterType;
  ErodeFilterType::Pointer erodeFilter = ErodeFilterType::New();
  erodeFilter->SetInput( img );

  KernalType ball;
  KernalType::SizeType ballSize;
  for ( int k = 0; k < 3; k++ )
  {
    ballSize[k] = ballsize;
  }
  ball.SetRadius( ballSize );
  ball.CreateStructuringElement();
  erodeFilter->SetKernel( ball );
  erodeFilter->Update();
  return erodeFilter->GetOutput();
}

ImageType::Pointer BinaryDilateFilter3D( ImageType::Pointer & img, unsigned int ballsize )
{
  typedef itk::BinaryBallStructuringElement<unsigned char, 3>                      KernalType;
  typedef itk::BinaryDilateImageFilter<ImageType, ImageType, KernalType> DilateFilterType;
  DilateFilterType::Pointer dilateFilter = DilateFilterType::New();
  dilateFilter->SetInput( img );
  KernalType ball;
  KernalType::SizeType ballSize;
  for ( int k = 0; k < 3; k++ )
  {
    ballSize[k] = ballsize;
  }
  ball.SetRadius( ballSize );
  ball.CreateStructuringElement();
  dilateFilter->SetKernel( ball );
  dilateFilter->Update();
  return dilateFilter->GetOutput();
}

ImageType::Pointer BinaryOpeningFilter3D( ImageType::Pointer & img, unsigned int ballsize )
{
  ImageType::Pointer imgErode = BinaryErodeFilter3D( img, ballsize );

  return BinaryDilateFilter3D( imgErode, ballsize );
}

ImageType::Pointer BinaryClosingFilter3D( ImageType::Pointer & img, unsigned int ballsize )
{
  if ( ballsize > 0 )
  {
    ImageType::Pointer imgDilate = BinaryDilateFilter3D( img, ballsize );
    return BinaryErodeFilter3D( imgDilate, ballsize );
  }
  else
  {
    return img;
  }
}

vtkStandardNewMacro( vtkPolyDataToImageData );

vtkPolyDataToImageData::vtkPolyDataToImageData()
{
  this->Spacing = 0.5;
  this->Border = 10;
  this->SampleDistance = 0.2;
  this->InitialBallSize = 4;
  this->FinalBallSize = 2;
}

//----------------------------------------------------------------------------
int vtkPolyDataToImageData::FillInputPortInformation(
  int vtkNotUsed( port ), vtkInformation* info )
{
  info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
  return 1;
}

int vtkPolyDataToImageData::RequestData(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector )
{

  // Get the input and output
  vtkInformation* inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkPolyData* input = vtkPolyData::SafeDownCast(
    inInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  vtkInformation* outInfo = outputVector->GetInformationObject( 0 );
  vtkImageData* output = vtkImageData::SafeDownCast(
    outInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  // create a point sampler to generate points along the surface
  vtkSmartPointer<vtkPolyDataPointSampler> sampler = vtkSmartPointer<vtkPolyDataPointSampler>::New();
  sampler->SetInput( input );
  sampler->SetDistance( this->SampleDistance );
  sampler->GenerateEdgePointsOn();
  sampler->GenerateInteriorPointsOn();
  sampler->GenerateVertexPointsOn();
  sampler->Update();
  std::cout << input->GetNumberOfPoints() << std::endl;
  std::cout << sampler->GetOutput()->GetNumberOfPoints() << std::endl;

  int dim[3];
  double spacing[3], origin[3];
  this->GetOutputInfo( inputVector, dim, origin, spacing );

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  ImageType::SizeType size;
  size[0] = dim[0];
  size[1] = dim[1];
  size[2] = dim[2];

  // create a new itk image
  ImageType::Pointer image = ImageType::New();
  image->SetSpacing( spacing );
  image->SetOrigin( origin );

  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  image->SetRegions( region );

  image->Allocate();
  image->FillBuffer( 0 );

  for ( int k = 0; k < sampler->GetOutput()->GetNumberOfPoints(); k++ )
  {
    double* pt = sampler->GetOutput()->GetPoint( k );
    ImageType::PointType pitk;
    pitk[0] = pt[0];
    pitk[1] = pt[1];
    pitk[2] = pt[2];
    ImageType::IndexType idx;
    image->TransformPhysicalPointToIndex( pitk, idx );

    if ( image->GetLargestPossibleRegion().IsInside( idx ) )
    {
      image->SetPixel( idx, 255 );
    }
  }

  // do morphological closing
  ImageType::Pointer closedLabel = BinaryClosingFilter3D( image, this->InitialBallSize );
  itk::ImageRegionIteratorWithIndex<ImageType> itLabel( closedLabel, closedLabel->GetLargestPossibleRegion() );

  // do flood fill using binary threshold image function
  typedef itk::BinaryThresholdImageFunction<ImageType> ImageFunctionType;
  ImageFunctionType::Pointer func = ImageFunctionType::New();
  func->SetInputImage( closedLabel );
  func->ThresholdBelow( 1 );

  ImageType::IndexType idx;
  ImageType::PointType COG;

  // set the center of gravity
  COG.Fill( 0.0 );
  for ( vtkIdType k = 0; k < input->GetNumberOfPoints(); k++ )
  {
    double* pt = input->GetPoint( k );
    for ( int m = 0; m < 3; m++ )
    {
      COG[m] += pt[m];
    }
  }
  for ( int m = 0; m < 3; m++ )
  {
    COG[m] /= static_cast<float>( input->GetNumberOfPoints() );
  }

  image->TransformPhysicalPointToIndex( COG, idx );

  itk::FloodFilledImageFunctionConditionalIterator<ImageType, ImageFunctionType> floodFill( closedLabel, func, idx );
  for ( floodFill.GoToBegin(); !floodFill.IsAtEnd(); ++floodFill )
  {
    ImageType::IndexType i = floodFill.GetIndex();
    closedLabel->SetPixel( i, 255 );
  }

  ImageType::Pointer finalLabel = BinaryClosingFilter3D( closedLabel, this->FinalBallSize );
  for ( itLabel.GoToBegin(); !itLabel.IsAtEnd(); ++itLabel )
  {
    ImageType::IndexType i = itLabel.GetIndex();
    image->SetPixel( i, finalLabel->GetPixel( i ) );
  }

  typedef itk::ImageToVTKImageFilter< ImageType > ConnectorType;
  ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput( image );
  connector->Update();

  output->DeepCopy( connector->GetOutput() );

  return 1;
}

//----------------------------------------------------------------------------
void vtkPolyDataToImageData::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

int vtkPolyDataToImageData::RequestInformation( vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector )
{
  // Get the input and output
  vtkInformation* inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkPolyData* input = vtkPolyData::SafeDownCast(
    inInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  int dim[3];
  double spacing[3], origin[3];
  this->GetOutputInfo( inputVector, dim, origin, spacing );

  // set the output info
  vtkInformation* outInfo = outputVector->GetInformationObject( 0 );

  vtkDataObject::SetPointDataActiveScalarInfo( outInfo, VTK_UNSIGNED_CHAR, 1 );

  outInfo->Set( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                0, dim[0] - 1,
                0, dim[1] - 1,
                0, dim[2] - 1 );

  outInfo->Set( vtkDataObject::ORIGIN(), origin, 3 );
  outInfo->Set( vtkDataObject::SPACING(), spacing, 3 );

  return 1;
}

void vtkPolyDataToImageData::GetOutputInfo( vtkInformationVector** inputVector, int dim[3], double origin[3], double spacing[3] )
{
  // Get the input and output
  vtkInformation* inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkPolyData* input = vtkPolyData::SafeDownCast(
    inInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  input->Update();

  // Get the bounds of the input input
  double bounds[6];
  input->GetBounds( bounds );

  //xmin, xmax, ymin, ymax, zmin, zmax
  bounds[0] -= this->Border;
  bounds[1] += this->Border;
  bounds[2] -= this->Border;
  bounds[3] += this->Border;
  bounds[4] -= this->Border;
  bounds[5] += this->Border;
  double xmin = bounds[0];
  double xmax = bounds[1];
  double ymin = bounds[2];
  double ymax = bounds[3];
  double zmin = bounds[4];
  double zmax = bounds[5];

  // compute dimensions
  for ( int i = 0; i < 3; i++ )
  {
    dim[i] = static_cast<int>( ceil( ( bounds[i * 2 + 1] - bounds[i * 2] ) / this->Spacing ) );
  }

  origin[0] = xmin;
  origin[1] = ymin;
  origin[2] = zmin;
  spacing[0] = this->Spacing;
  spacing[1] = this->Spacing;
  spacing[2] = this->Spacing;
}
