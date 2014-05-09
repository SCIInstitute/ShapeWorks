#include <QFileInfo>

#include <vtkMarchingCubes.h>

#include <itkImageFileReader.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

//---------------------------------------------------------------------------
Mesh::Mesh( QString filename )
{
  try
  {
    this->filename_ = filename;

    // read file using ITK
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( filename.toStdString() );
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();

    // get image dimensions
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();
    this->dimensions_[0] = size[0];
    this->dimensions_[1] = size[1];
    this->dimensions_[2] = size[2];

    // connect to VTK
    vtkSmartPointer<vtkImageImport> vtk_image = vtkSmartPointer<vtkImageImport>::New();
    itk::VTKImageExport<ImageType>::Pointer itk_exporter = itk::VTKImageExport<ImageType>::New();
    itk_exporter->SetInput( image );
    ConnectPipelines( itk_exporter, vtk_image.GetPointer() );
    vtk_image->Update();

    // create isosurface
    vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
    marching->SetInputData( vtk_image->GetOutput() );
    marching->SetNumberOfContours( 1 );
    marching->SetValue( 0, 0.5 );
    marching->Update();

    // store isosurface polydata
    this->poly_data_ = marching->GetOutput();
  }
  catch ( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
}

//---------------------------------------------------------------------------
Mesh::~Mesh()
{}

//---------------------------------------------------------------------------
QString Mesh::get_dimension_string()
{
  QString str = "[" + QString::number( this->dimensions_[0] ) +
                ", " + QString::number( this->dimensions_[1] ) +
                ", " + QString::number( this->dimensions_[2] ) + "]";
  return str;
}

//---------------------------------------------------------------------------
QString Mesh::get_filename()
{
  QFileInfo qfi( this->filename_ );
  return qfi.fileName();
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
}

//---------------------------------------------------------------------------
QString Mesh::get_filename_with_path()
{
  return this->filename_;
}
