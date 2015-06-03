#include <QMessageBox>
#include <QTextStream>

#include <vtkImageImport.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataNormals.h>

#include <itkImageFileReader.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVTKImageExport.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

#include <vtkSurfaceReconstructionFilter.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

//---------------------------------------------------------------------------
Mesh::Mesh()
{}

//---------------------------------------------------------------------------
Mesh::~Mesh()
{}

void Mesh::set_poly_data(vtkSmartPointer<vtkPolyData> poly_data) {
	this->poly_data_ = poly_data;
}

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
void Mesh::create_from_image( QString filename, float iso_value )
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

    // find the center of mass
    itk::Array<double> params( 3 );
    params.Fill( 0.0 );
    double count = 0.0;
    itk::Point<double, 3> point;
    itk::ImageRegionIteratorWithIndex<ImageType> oit( image, image->GetLargestPossibleRegion() );
    for ( oit.GoToBegin(); !oit.IsAtEnd(); ++oit )
    {
      if ( oit.Get() > 0 )
      {

        // Get the physical index from the image index.
        image->TransformIndexToPhysicalPoint( oit.GetIndex(), point );
        for ( unsigned int i = 0; i < 3; i++ )
        {
          params[i] += point[i];
        }
        count += 1.0;
      }
    }

    // compute center of mass
    this->center_transform_.set_size( 3 );
    for ( unsigned int i = 0; i < 3; i++ )
    {
      this->center_transform_[i] = params[i] / count;
    }

    // connect to VTK
    vtkSmartPointer<vtkImageImport> vtk_image = vtkSmartPointer<vtkImageImport>::New();
    itk::VTKImageExport<ImageType>::Pointer itk_exporter = itk::VTKImageExport<ImageType>::New();
    itk_exporter->SetInput(image);
    ConnectPipelines( itk_exporter, vtk_image.GetPointer() );
    vtk_image->Update();
	
    // create isosurface
    if (!this->marching_)
  	  this->marching_ = vtkSmartPointer<vtkMarchingCubes>::New();
    this->marching_->SetInputConnection( vtk_image->GetOutputPort() );
    this->marching_->SetNumberOfContours( 1 );
    this->marching_->SetValue( 0, iso_value );
    this->marching_->Update();

    // store isosurface polydata
    this->poly_data_ = this->marching_->GetOutput();
  }
  catch ( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
}

//---------------------------------------------------------------------------
bool Mesh::create_from_pointset( const vnl_vector<double>& vnl_points )
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
  vtkSmartPointer<vtkPolyData>pointSet = vtkSmartPointer<vtkPolyData>::New();
  pointSet->SetPoints( points );

  // create isosurface
  if (!this->marching_)
	this->marching_ = vtkSmartPointer<vtkMarchingCubes>::New();
  this->marching_->SetNumberOfContours( 1 );
  this->marching_->SetValue( 0, 0.0 );

  vtkSmartPointer<vtkSurfaceReconstructionFilter> surface_reconstruction =
    vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
  surface_reconstruction->SetInputData( pointSet );
  this->marching_->SetInputConnection( surface_reconstruction->GetOutputPort() );
  
  return true;
}

//---------------------------------------------------------------------------
vnl_vector<double> Mesh::get_center_transform()
{
  return this->center_transform_;
}

//---------------------------------------------------------------------------
void Mesh::Update() {
	this->marching_->Update();
	this->poly_data_ = marching_->GetOutput();
}