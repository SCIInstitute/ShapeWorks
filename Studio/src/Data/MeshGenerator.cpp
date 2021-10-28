#include <limits>

#include <QFileInfo>

#include <itkPoint.h>

#include <vtkPolyDataNormals.h>
#include <vtkMarchingCubes.h>

#include <Data/MeshGenerator.h>
#include <Data/ItkToVtk.h>
#include <Data/StudioLog.h>

#include <itkVTKImageExport.h>
#include <itkOrientImageFilter.h>

#include <Libs/Mesh/Mesh.h>
#include <Libs/Mesh/MeshUtils.h>

#include <Libs/Utils/StringUtils.h>

#include <Data/LegacyMeshGenerator.h>

namespace shapeworks {

const std::string MeshGenerator::RECONSTRUCTION_LEGACY_C("legacy");
const std::string MeshGenerator::RECONSTRUCTION_DISTANCE_TRANSFORM_C("distance_transform");
const std::string MeshGenerator::RECONSTRUCTION_MESH_WARPER_C("mesh_warper");

//---------------------------------------------------------------------------
MeshGenerator::MeshGenerator()
{}

//---------------------------------------------------------------------------
MeshGenerator::~MeshGenerator()
{}

//---------------------------------------------------------------------------
MeshHandle MeshGenerator::build_mesh(const MeshWorkItem& item)
{
  if (item.filename != "") {
    return this->build_mesh_from_file(item.filename);
  }
  else {
    return this->build_mesh_from_points(item.points, item.domain);
  }
}

//---------------------------------------------------------------------------
MeshHandle MeshGenerator::build_mesh_from_points(const vnl_vector<double>& shape,
                                                 int domain)
{
  MeshHandle mesh(new StudioMesh);

  auto& surface_reconstructors = this->reconstructors_->surface_reconstructors_;
  auto& mesh_warpers = this->reconstructors_->mesh_warpers_;

  // temporary support for contours
  if (mesh_warpers.size() > domain && mesh_warpers[domain]->is_contour()) {
    auto poly_data = vtkSmartPointer<vtkPolyData>::New();
    mesh->set_poly_data(poly_data);
    return mesh;
  }

  bool distance_transform_available =
    surface_reconstructors.size() > domain &&
    surface_reconstructors[domain] &&
    surface_reconstructors[domain]->get_surface_reconstruction_available();

  if (this->reconstruction_method_ == RECONSTRUCTION_DISTANCE_TRANSFORM_C &&
      distance_transform_available) {
    vtkSmartPointer<vtkPolyData> poly_data = surface_reconstructors[domain]->build_mesh(
      shape);

    vtkSmartPointer<vtkPolyDataNormals> polydata_normals =
      vtkSmartPointer<vtkPolyDataNormals>::New();
    polydata_normals->SetInputData(poly_data);
    polydata_normals->Update();
    poly_data = polydata_normals->GetOutput();

    mesh->set_poly_data(poly_data);
  }
  else if (this->reconstruction_method_ == RECONSTRUCTION_MESH_WARPER_C &&
           mesh_warpers.size() > domain && mesh_warpers[domain] &&
           mesh_warpers[domain]->get_warp_available()) {

    Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>((double*) shape.data_block(),
                                                               shape.size());
    points.resize(3, shape.size() / 3);
    points.transposeInPlace();

    vtkSmartPointer<vtkPolyData> poly_data = mesh_warpers[domain]->build_mesh(points);

    if (!poly_data) {
      std::string message = std::string("Unable to warp mesh");
      STUDIO_LOG_ERROR(QString::fromStdString(message));
      mesh->set_error_message(message);
      return mesh;
    }
    vtkSmartPointer<vtkPolyDataNormals> polydata_normals =
      vtkSmartPointer<vtkPolyDataNormals>::New();
    polydata_normals->SetInputData(poly_data);
    polydata_normals->Update();
    poly_data = polydata_normals->GetOutput();

    mesh->set_poly_data(poly_data);
  }
  else {
    LegacyMeshGenerator legacy;
    mesh->set_poly_data(legacy.buildMesh(shape));
  }
  return mesh;
}

//---------------------------------------------------------------------------
MeshHandle MeshGenerator::build_mesh_from_image(ImageType::Pointer image, float iso_value)
{
  MeshHandle mesh(new StudioMesh);

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

    auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputConnection(marching->GetOutputPort());
    normals->Update();

    mesh->set_poly_data(normals->GetOutput());
  } catch (itk::ExceptionObject& excep) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
    mesh->set_error_message(std::string("Exception: ") + excep.what());
  }
  return mesh;
}

//---------------------------------------------------------------------------
MeshHandle MeshGenerator::build_mesh_from_file(std::string filename, float iso_value)
{
  //std::cerr << "build_mesh from " << filename << "\n";
  MeshHandle mesh(new StudioMesh);

  if (!QFileInfo(QString::fromStdString(filename)).exists()) {
    mesh->set_error_message("File does not exist: " + filename);
    return mesh;
  }

  bool is_mesh = false;
  for (auto type : shapeworks::Mesh::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      is_mesh = true;
    }
  }

  if (is_mesh) {
    try {
      mesh->set_poly_data(MeshUtils::threadSafeReadMesh(filename).getVTKMesh());
    } catch (std::exception& e) {
      std::string message = std::string("Error reading: ") + filename;
      STUDIO_LOG_ERROR(QString::fromStdString(message));
      mesh->set_error_message(message);
    }
  }
  else {
    try {
      // read file using ITK
      using ReaderType = itk::ImageFileReader<ImageType>;
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(filename);
      reader->Update();
      ImageType::Pointer image = reader->GetOutput();

      // set orientation to RAI
      using Orienter = itk::OrientImageFilter<ImageType, ImageType>;
      Orienter::Pointer orienter = Orienter::New();
      orienter->UseImageDirectionOn();
      orienter->SetDesiredCoordinateOrientation(
        itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
      orienter->SetInput(image);
      orienter->Update();
      image = orienter->GetOutput();

      mesh = this->build_mesh_from_image(image, iso_value);
    } catch (itk::ExceptionObject& excep) {
      std::cerr << "Exception caught!" << std::endl;
      std::cerr << excep << std::endl;
      mesh->set_error_message(std::string("Exception: ") + excep.what());
    }
  }
  return mesh;

}

//---------------------------------------------------------------------------
void MeshGenerator::set_reconstruction_method(std::string method)
{
  this->reconstruction_method_ = method;
}

//---------------------------------------------------------------------------
std::string MeshGenerator::get_reconstruction_method()
{
  return this->reconstruction_method_;
}

//---------------------------------------------------------------------------
void MeshGenerator::set_mesh_reconstructors(std::shared_ptr<MeshReconstructors> reconstructors)
{
  this->reconstructors_ = reconstructors;
}

}
