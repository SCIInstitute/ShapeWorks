#include <QFileInfo>

#include <vtkMarchingCubes.h>
#include <vtkImageImport.h>

#include <itkImageFileReader.h>
#include <itkVTKImageExport.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

#include <Visualization/CustomSurfaceReconstructionFilter.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

//---------------------------------------------------------------------------
Mesh::Mesh()
{}

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

//---------------------------------------------------------------------------
void Mesh::create_from_image( QString filename )
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
    marching->SetInputConnection( vtk_image->GetOutputPort() );
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
bool Mesh::create_from_pointset( QString filename )
{
  try
  {
    this->filename_ = filename;

    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
      QMessageBox::warning( 0, "Unable to open file", "Error opening file: " + filename );
      return false;
    }

    QTextStream stream( &file );

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    int num_points = 0;
    //points->SetNumberOfPoints(128);
    while ( !stream.atEnd() )
    {
      QString line = stream.readLine();
      QStringList list = line.split( ' ' );
      if ( list.size() != 4 ) // sanity check
      {
        std::cerr << "Error, line " << line.toStdString() << " does not contain 3 fields\n";
        std::cerr << "list.size = " << list.size() << "\n";
        return false;
      }

      float x = list[0].toFloat();
      float y = list[1].toFloat();
      float z = list[2].toFloat();

      points->InsertNextPoint( x, y, z );
      num_points++;
    }

    std::cerr << "found " << num_points << " points\n";

    points->Modified();

    vtkSmartPointer<vtkPolyData>pointSet = vtkSmartPointer<vtkPolyData>::New();
    pointSet->SetPoints( points );

    vtkSmartPointer<CustomSurfaceReconstructionFilter> surfaceReconstruction = vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
    surfaceReconstruction->SetInputData( pointSet );

    // create isosurface
    vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
    marching->SetInputConnection( surfaceReconstruction->GetOutputPort() );
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
    return false;
  }

  std::cerr << "true!\n";
  return true;
}
