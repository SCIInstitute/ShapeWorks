#include <QMessageBox>
#include <QTextStream>

#include <itkImageRegionIteratorWithIndex.h>
#include <itkVTKImageExport.h>
#include <itkOrientImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLinearInterpolateImageFunction.h>

#include <vtkSurfaceReconstructionFilter.h>
#include <vtkMarchingCubes.h>
#include <vtkTriangleFilter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointLocator.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

using NearestNeighborInterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

//---------------------------------------------------------------------------
Mesh::Mesh()
{}

//---------------------------------------------------------------------------
Mesh::~Mesh()
{}

//---------------------------------------------------------------------------
void Mesh::set_poly_data(vtkSmartPointer<vtkPolyData> poly_data)
{
  this->poly_data_ = poly_data;
}

//---------------------------------------------------------------------------
void Mesh::set_error_message(std::string error_message)
{
  this->error_message_ = error_message;
}

//---------------------------------------------------------------------------
std::string Mesh::get_error_message()
{
  return this->error_message_;
}

//---------------------------------------------------------------------------
QString Mesh::get_dimension_string()
{
  QString str = "[" + QString::number(this->dimensions_[0]) +
                ", " + QString::number(this->dimensions_[1]) +
                ", " + QString::number(this->dimensions_[2]) + "]";
  return str;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
}

//---------------------------------------------------------------------------
void Mesh::create_from_image(ImageType::Pointer image, double iso_value)
{
  try {
    // get image dimensions
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();
    this->dimensions_[0] = size[0];
    this->dimensions_[1] = size[1];
    this->dimensions_[2] = size[2];

    // find the center of mass
    itk::Array<double> params(3);
    params.Fill(0.0);
    double count = 0.0;
    itk::Point<double, 3> point;
    itk::ImageRegionIteratorWithIndex<ImageType> oit(image, image->GetLargestPossibleRegion());
    for (oit.GoToBegin(); !oit.IsAtEnd(); ++oit) {
      if (oit.Get() > 0) {
        // Get the physical index from the image index.
        image->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
        for (unsigned int i = 0; i < 3; i++) {
          params[i] += point[i];
        }
        count += 1.0;
      }
    }

    // compute center of mass
    this->center_transform_.set_size(3);
    for (unsigned int i = 0; i < 3; i++) {
      this->center_transform_[i] = params[i] / count;
    }

    this->center_transform_[0] = 0;
    this->center_transform_[1] = 0;
    this->center_transform_[2] = 0;

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

    // store isosurface polydata
    this->poly_data_ = marching->GetOutput();
  } catch (itk::ExceptionObject& excep) {
    std::cerr << "3Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
}

//---------------------------------------------------------------------------
vnl_vector<double> Mesh::get_center_transform()
{
  return this->center_transform_;
}

//---------------------------------------------------------------------------
void Mesh::apply_feature_map(std::string name, ImageType::Pointer image,
                             vnl_vector<double> transform)
{
  if (!this->poly_data_ || name == "") {
    return;
  }

  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);

  auto region = image->GetLargestPossibleRegion();

  float radius = 5.0; // mm - need to expose as parameter

  auto points = this->poly_data_->GetPoints();

  vtkFloatArray* scalars = vtkFloatArray::New();
  scalars->SetNumberOfValues(points->GetNumberOfPoints());
  scalars->SetName(name.c_str());

  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double pt[3];
    points->GetPoint(i, pt);

    ImageType::PointType pitk;
    pitk[0] = pt[0];
    pitk[1] = pt[1];
    pitk[2] = pt[2];

    if (transform.size() == 3) {
      pitk[0] = pitk[0] + transform[0];
      pitk[1] = pitk[1] + transform[1];
      pitk[2] = pitk[2] + transform[2];
    }

    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    auto pixel = 0;
    if (region.IsInside(index)) {
      pixel = interpolator->EvaluateAtContinuousIndex(index);
      //  std::cerr << pixel << " ";
    }
    scalars->SetValue(i, pixel);
  }
  //std::cerr << "\n";

  this->poly_data_->GetPointData()->AddArray(scalars);

  this->poly_data_->GetPointData()->SetScalars(scalars);

  /*
  auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetInputData(this->poly_data_);
  writer->SetFileName("/tmp/foo.vtk");
  writer->Update();
*/
}

//---------------------------------------------------------------------------
void Mesh::interpolate_scalars_to_mesh(std::string name, vnl_vector<double> positions,
                                       itkeigen::VectorXf scalar_values)
{

  int num_points = positions.size() / 3;
  if (num_points == 0) {
    return;
  }

  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();

  vtk_points->SetNumberOfPoints(num_points);

  unsigned int idx = 0;
  for (int i = 0; i < num_points; i++) {

    double x = positions[idx++];
    double y = positions[idx++];
    double z = positions[idx++];

    vtk_points->InsertPoint(i, x, y, z);
  }

  vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
  point_data->SetPoints(vtk_points);

  vtkSmartPointer<vtkPointLocator> point_locator = vtkPointLocator::New();
  point_locator->SetDataSet(point_data);
  point_locator->SetDivisions(100, 100, 100);
  point_locator->BuildLocator();

  if (!this->poly_data_)
  {
    return;
  }
  auto points = this->poly_data_->GetPoints();

  vtkFloatArray* scalars = vtkFloatArray::New();
  scalars->SetNumberOfValues(points->GetNumberOfPoints());
  scalars->SetName(name.c_str());

  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double pt[3];
    points->GetPoint(i, pt);

    // find the 8 closest correspondence points the to current point
    vtkSmartPointer<vtkIdList> closest_points = vtkSmartPointer<vtkIdList>::New();
    point_locator->FindClosestNPoints(8, pt, closest_points);

    // assign scalar value based on a weighted scheme
    float weighted_scalar = 0.0f;
    float distanceSum = 0.0f;
    float distance[8];
    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      // get a particle position
      vtkIdType id = closest_points->GetId(p);

      // compute distance to current particle
      double x = pt[0] - point_data->GetPoint(id)[0];
      double y = pt[1] - point_data->GetPoint(id)[1];
      double z = pt[2] - point_data->GetPoint(id)[2];
      distance[p] = 1.0f / (x * x + y * y + z * z);

      // multiply scalar value by weight and add to running sum
      distanceSum += distance[p];
    }

    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      vtkIdType current_id = closest_points->GetId(p);
      weighted_scalar += distance[p] / distanceSum * scalar_values[current_id];
    }

    scalars->SetValue(i, weighted_scalar);
  }

  this->poly_data_->GetPointData()->AddArray(scalars);

}
