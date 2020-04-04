#include <limits>

#include <itkPoint.h>

#include <vtkPolyDataNormals.h>
#include <vtkMarchingCubes.h>

#include <Data/MeshGenerator.h>
#include <Data/ItkToVtk.h>
#include <itkVTKImageExport.h>
#include <itkOrientImageFilter.h>

//---------------------------------------------------------------------------
MeshGenerator::MeshGenerator(Preferences& prefs)
  : prefs_(prefs),
  legacy_reconstructor_(new LegacyMeshGenerator())
{}

//---------------------------------------------------------------------------
MeshGenerator::~MeshGenerator() {}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::build_mesh(const MeshWorkItem &item)
{
  if (item.filename != "") {
    return this->build_mesh(item.filename);
  }
  else {
    return this->build_mesh(item.shape, item.domain);
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::build_mesh(const vnl_vector<double>& shape, int domain)
{
  if (this->surface_reconstructor_ &&
      this->surface_reconstructor_->get_surface_reconstruction_available()) {
    vtkSmartPointer<vtkPolyData> poly_data = this->surface_reconstructor_->build_mesh(shape);

    vtkSmartPointer<vtkPolyDataNormals> polydata_normals =
      vtkSmartPointer<vtkPolyDataNormals>::New();
    polydata_normals->SetInputData(poly_data);
    polydata_normals->Update();
    poly_data = polydata_normals->GetOutput();

    return poly_data;
  }
  else {

    return this->legacy_reconstructor_->buildMesh(shape);
  }
  return vtkPolyData::New();
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::build_mesh(ImageType::Pointer image, float iso_value)
{
  vtkSmartPointer<vtkPolyData> poly_data;
  try {

    // connect to VTK
    vtkSmartPointer<vtkImageImport> vtk_image = vtkSmartPointer<vtkImageImport>::New();
    itk::VTKImageExport<ImageType>::Pointer itk_exporter = itk::VTKImageExport<ImageType>::New();
    itk_exporter->SetInput(image);
    ConnectPipelines(itk_exporter, vtk_image.GetPointer());
    vtk_image->Update();

    // create isosurface
    auto marching = vtkSmartPointer<vtkMarchingCubes>::New();
    marching->SetInputConnection(vtk_image->GetOutputPort());
    marching->SetNumberOfContours(1);
    marching->SetValue(0, iso_value);
    marching->Update();

    poly_data = marching->GetOutput();
  } catch (itk::ExceptionObject & excep) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
  return poly_data;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::build_mesh(std::string filename, float iso_value)
{
  std::cerr << "build_mesh from " << filename << "\n";

  vtkSmartPointer<vtkPolyData> poly_data;
  try {

    // read file using ITK
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();

    // set orientation to RAI
    itk::OrientImageFilter<ImageType, ImageType>::Pointer orienter =
      itk::OrientImageFilter<ImageType, ImageType>::New();
    orienter->UseImageDirectionOn();
    orienter->SetDesiredCoordinateOrientation(
      itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
    orienter->SetInput(image);
    orienter->Update();
    image = orienter->GetOutput();

    poly_data = this->build_mesh(image, iso_value);
  } catch (itk::ExceptionObject & excep) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
  return poly_data;
}

//---------------------------------------------------------------------------
void MeshGenerator::set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor)
{
  this->surface_reconstructor_ = reconstructor;
}
