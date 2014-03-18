#include <vtkMarchingCubes.h>

#include <itkImageToVTKImageFilter.h>

#include <Data/DataManager.h>


typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;



DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

void DataManager::importFile(std::string file)
{

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( file );
  reader->Update();
  ImageType::Pointer image = reader->GetOutput();

  typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
  ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput( image );
  connector->Update();


  vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
  marching->SetInputData( connector->GetOutput());
  marching->SetNumberOfContours(1);
  marching->SetValue(0,0.5);

  marching->Update();

  vtkSmartPointer<vtkPolyData> polyData = marching->GetOutput();


  std::cerr << "gen: " << polyData->GetNumberOfPoints() << "\n";



  this->meshes_.push_back( polyData );

}

std::vector<vtkSmartPointer<vtkPolyData> > DataManager::getMeshes()
{
  return this->meshes_;
}

