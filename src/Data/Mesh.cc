#include <QMessageBox>
#include <QTextStream>

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
vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
}

//---------------------------------------------------------------------------
void Mesh::create_from_image( QString filename )
{
  try
  {

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
bool Mesh::create_from_pointset( QString filename, const vnl_vector<double>& vnl_points )
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

  int num_points = vnl_points.size() / 3;
  points->SetNumberOfPoints( num_points );

  unsigned int k = 0;
  for ( unsigned int i = 0; i < num_points; i++ )
  {
    double x = vnl_points[k++];
    double y = vnl_points[k++];
    double z = vnl_points[k++];
    points->SetPoint( i, x, y, z );
  }
  points->Modified();

  std::cerr << "found " << num_points << " points\n";

  points->Modified();

  vtkSmartPointer<vtkPolyData>pointSet = vtkSmartPointer<vtkPolyData>::New();
  pointSet->SetPoints( points );

  vtkSmartPointer<CustomSurfaceReconstructionFilter> surfaceReconstruction =
    vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
  surfaceReconstruction->SetInputData( pointSet );

  // create isosurface
  vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
  marching->SetInputConnection( surfaceReconstruction->GetOutputPort() );
  marching->SetNumberOfContours( 1 );
  marching->SetValue( 0, 0.5 );
  marching->Update();

  // store isosurface polydata
  this->poly_data_ = marching->GetOutput();

  return true;
}
