#include <vtkMarchingCubes.h>

#include <itkImageToVTKImageFilter.h>
#include <itkImageFileReader.h>

#include <Data/Mesh.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

Mesh::Mesh()
{}

Mesh::~Mesh()
{}

void Mesh::load_file( std::string filename )
{
  this->filename_ = filename;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename );
  reader->Update();
  ImageType::Pointer image = reader->GetOutput();

  ImageType::RegionType region = image->GetLargestPossibleRegion();
  ImageType::SizeType size = region.GetSize();

  std::cerr << size << "\n";

  this->dimensions_[0] = size[0];
  this->dimensions_[1] = size[1];
  this->dimensions_[2] = size[2];

  typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
  ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput( image );
  connector->Update();

  vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
  marching->SetInputData( connector->GetOutput() );
  marching->SetNumberOfContours( 1 );
  marching->SetValue( 0, 0.5 );

  marching->Update();

  this->poly_data_ = marching->GetOutput();
}

std::string Mesh::get_dimension_string()
{
  std::stringstream ss;
  ss << "[" << this->dimensions_[0] << ", " << this->dimensions_[1] << ", " << this->dimensions_[2] << "]";
  return ss.str();
}

std::string Mesh::get_filename()
{
  return this->filename_;
}

vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
}
