#include <vtkMarchingCubes.h>

#include <itkImageToVTKImageFilter.h>
#include <itkImageFileReader.h>

#include <Data/Shape.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

Shape::Shape()
{}

Shape::~Shape()
{}

void Shape::import_initial_file( std::string filename )
{
  this->initial_mesh_.load_file( filename );
}

Mesh Shape::get_initial_mesh()
{
  return this->initial_mesh_;
}
