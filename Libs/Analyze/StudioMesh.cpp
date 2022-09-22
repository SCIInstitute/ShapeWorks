// shapeworks
#include "StudioMesh.h"
#include <StringUtils.h>

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
                                             Eigen::VectorXf scalar_values) {
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

  if (num_points != scalar_values.size()) {
    std::cerr << "Warning, mismatch of points and scalar values\n";
    return;
  }

  vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
  point_data->SetPoints(vtk_points);

  vtkSmartPointer<vtkPointLocator> point_locator = vtkPointLocator::New();
  point_locator->SetDataSet(point_data);
  point_locator->SetDivisions(100, 100, 100);
  point_locator->BuildLocator();

  if (!poly_data_) {
    return;
  }

  auto points = poly_data_->GetPoints();

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
    bool exactly_on_point = false;
    float exact_scalar = 0.0f;
    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      // get a particle position
      vtkIdType id = closest_points->GetId(p);

      // compute distance to current particle
      double x = pt[0] - point_data->GetPoint(id)[0];
      double y = pt[1] - point_data->GetPoint(id)[1];
      double z = pt[2] - point_data->GetPoint(id)[2];

      if (x == 0 && y == 0 && z == 0) {
        distance[p] = 0;
        exactly_on_point = true;
        exact_scalar = scalar_values[id];
      } else {
        distance[p] = 1.0f / (x * x + y * y + z * z);
      }

      // multiply scalar value by weight and add to running sum
      distanceSum += distance[p];
    }

    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      vtkIdType current_id = closest_points->GetId(p);
      weighted_scalar += distance[p] / distanceSum * scalar_values[current_id];
    }

    if (exactly_on_point) {
      weighted_scalar = exact_scalar;
    }

    scalars->SetValue(i, weighted_scalar);
  }

  poly_data_->GetPointData()->AddArray(scalars);
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
bool StudioMesh::has_ffc_paint() {
  auto result = poly_data_->GetPointData()->GetArray(FFC_PAINT);
  return result != nullptr;
}

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
