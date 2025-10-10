// shapeworks
#include "StudioMesh.h"

#include <StringUtils.h>

#include "Mesh.h"

// itk
#include <itkImageRegionIteratorWithIndex.h>
#include <itkLinearInterpolateImageFunction.h>

// vtk
#include <vtkFloatArray.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkStaticPointLocator.h>

using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

namespace shapeworks {

//---------------------------------------------------------------------------
StudioMesh::StudioMesh() {}

//---------------------------------------------------------------------------
StudioMesh::~StudioMesh() {}

//---------------------------------------------------------------------------
void StudioMesh::set_poly_data(vtkSmartPointer<vtkPolyData> poly_data) { poly_data_ = poly_data; }

//---------------------------------------------------------------------------
void StudioMesh::set_error_message(std::string error_message) { error_message_ = error_message; }

//---------------------------------------------------------------------------
std::string StudioMesh::get_error_message() { return error_message_; }

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> StudioMesh::get_poly_data() { return poly_data_; }

//---------------------------------------------------------------------------
void StudioMesh::apply_feature_map(std::string name, ImageType::Pointer image) {
  if (!poly_data_ || name == "") {
    return;
  }

  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);

  auto region = image->GetLargestPossibleRegion();

  auto points = poly_data_->GetPoints();

  vtkFloatArray* scalars = vtkFloatArray::New();
  scalars->SetNumberOfValues(points->GetNumberOfPoints());
  scalars->SetName(name.c_str());

  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double* pt = points->GetPoint(i);

    ImageType::PointType pitk;
    pitk[0] = pt[0];
    pitk[1] = pt[1];
    pitk[2] = pt[2];

    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    float pixel = 0.0f;
    if (region.IsInside(index)) {
      pixel = interpolator->EvaluateAtContinuousIndex(index);
    }
    scalars->SetValue(i, pixel);
  }

  poly_data_->GetPointData()->AddArray(scalars);
  poly_data_->GetPointData()->SetScalars(scalars);
}

//---------------------------------------------------------------------------
void StudioMesh::interpolate_scalars_to_mesh(std::string name, Eigen::VectorXd positions,
                                             Eigen::VectorXd scalar_values) {
  Mesh mesh(poly_data_);
  mesh.interpolate_scalars_to_mesh(name, positions, scalar_values);
}

//---------------------------------------------------------------------------
double StudioMesh::get_largest_dimension_size() {
  if (!poly_data_) {
    return -1;
  }
  double bounds[6];
  poly_data_->GetBounds(bounds);
  double max = 0;
  max = std::max<double>(max, bounds[1] - bounds[0]);
  max = std::max<double>(max, bounds[3] - bounds[2]);
  max = std::max<double>(max, bounds[5] - bounds[4]);
  return max;
}

//---------------------------------------------------------------------------
vtkFloatArray* StudioMesh::get_or_create_array(std::string name, float default_value) {
  if (!poly_data_ || poly_data_->GetNumberOfPoints() == 0 || !poly_data_->GetPointData()) {
    return nullptr;
  }
  auto result = poly_data_->GetPointData()->GetArray(name.c_str());
  if (!result) {
    vtkFloatArray* array = vtkFloatArray::New();
    array->SetName(name.c_str());
    array->SetNumberOfTuples(poly_data_->GetNumberOfPoints());
    array->FillComponent(0, default_value);
    poly_data_->GetPointData()->AddArray(array);
  }

  vtkFloatArray* array = vtkFloatArray::SafeDownCast(poly_data_->GetPointData()->GetArray(name.c_str()));
  return array;
}

//---------------------------------------------------------------------------
void StudioMesh::paint_ffc(double world_pos[], double radius, bool inclusive) {
  if (!locator_) {
    locator_ = vtkSmartPointer<vtkStaticPointLocator>::New();
    locator_->SetDataSet(poly_data_);
    locator_->BuildLocator();
  }

  auto scalars = get_or_create_array(FFC_PAINT, 1.0);

  vtkNew<vtkIdList> result;
  // find vertices within paint sphere
  locator_->FindPointsWithinRadius(radius, world_pos, result);
  for (vtkIdType i = 0; i < result->GetNumberOfIds(); i++) {
    vtkIdType point_ind = result->GetId(i);
    float value = inclusive ? 1 : 0;
    scalars->SetTuple1(point_ind, value);
  }
  scalars->Modified();
}

//---------------------------------------------------------------------------
void StudioMesh::fill_ffc(bool inclusive) {
  auto scalars = get_or_create_array(FFC_PAINT, 1.0);
  float value = inclusive ? 1.0f : 0.0f;
  scalars->FillComponent(0, value);
  scalars->Modified();
}

//---------------------------------------------------------------------------
bool StudioMesh::has_ffc_paint() {
  auto result = poly_data_->GetPointData()->GetArray(FFC_PAINT);
  return result != nullptr;
}

//---------------------------------------------------------------------------
Mesh StudioMesh::get_sw_mesh() { return Mesh(poly_data_); }

//---------------------------------------------------------------------------
void StudioMesh::apply_scalars(MeshHandle mesh) {
  vtkSmartPointer<vtkPolyData> from_mesh = mesh->get_poly_data();
  vtkSmartPointer<vtkPolyData> to_mesh = get_poly_data();

  // Create the tree
  vtkSmartPointer<vtkKdTreePointLocator> kd_tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kd_tree->SetDataSet(from_mesh);
  kd_tree->BuildLocator();

  int num_arrays = from_mesh->GetPointData()->GetNumberOfArrays();

  // cache pointers to new arrays
  std::vector<vtkSmartPointer<vtkFloatArray>> arrays;

  // set up new arrays
  for (int i = 0; i < num_arrays; i++) {
    std::string name = StringUtils::safeString(from_mesh->GetPointData()->GetArrayName(i));
    vtkSmartPointer<vtkFloatArray> to_array = vtkSmartPointer<vtkFloatArray>::New();
    to_array->SetName(name.c_str());
    to_array->SetNumberOfValues(to_mesh->GetNumberOfPoints());
    to_mesh->GetPointData()->AddArray(to_array);
    arrays.push_back(to_array);
  }

  // apply scalars from one mesh to the other
  for (int j = 0; j < to_mesh->GetNumberOfPoints(); j++) {
    double* p = to_mesh->GetPoint(j);
    vtkIdType id = kd_tree->FindClosestPoint(p);

    for (int i = 0; i < num_arrays; i++) {
      auto from_array = from_mesh->GetPointData()->GetAbstractArray(i);
      vtkVariant var = from_array->GetVariantValue(id);
      arrays[i]->SetVariantValue(j, var);
    }
  }
}
}  // namespace shapeworks
