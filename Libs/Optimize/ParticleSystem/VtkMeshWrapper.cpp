#include "VtkMeshWrapper.h"

#include <vtkCellLocator.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

namespace shapeworks {

namespace {
template<class FROM, class TO>
inline TO convert(FROM& value)
{
  TO converted;
  converted[0] = value[0];
  converted[1] = value[1];
  converted[2] = value[2];
  return converted;
}
}

//---------------------------------------------------------------------------
VtkMeshWrapper::VtkMeshWrapper(vtkSmartPointer<vtkPolyData> poly_data)
{

  vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputData(poly_data);
  normals->ComputeCellNormalsOn();
  normals->ComputePointNormalsOn();
  normals->Update();

  this->poly_data_ = normals->GetOutput();

  this->cell_locator_->SetDataSet(poly_data);
  this->cell_locator_->BuildLocator();

}

//---------------------------------------------------------------------------
double VtkMeshWrapper::ComputeDistance(VtkMeshWrapper::PointType pointa,
                                       VtkMeshWrapper::PointType pointb) const
{
  return pointa.EuclideanDistanceTo(pointb);
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType VtkMeshWrapper::GeodesicWalk(VtkMeshWrapper::PointType pointa, int idx,
                                                       vnl_vector_fixed<double, 3> vector) const
{

  pointa[0] += vector[0];
  pointa[1] += vector[1];
  pointa[2] += vector[2];

  return this->SnapToMesh(pointa, idx);
}

//---------------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION>
VtkMeshWrapper::ProjectVectorToSurfaceTangent(const VtkMeshWrapper::PointType& pointa, int idx,
                                              vnl_vector_fixed<double, 3>& vector)
{

  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];

  int faceIndex = this->GetTriangleForPoint(point, idx);

  double* normal = this->poly_data_->GetCellData()->GetNormals()->GetTuple(faceIndex);

  Eigen::Vector3d vec_normal;
  vec_normal[0] = normal[0];
  vec_normal[1] = normal[1];
  vec_normal[2] = normal[2];

  Eigen::Vector3d vec_vector;
  vec_vector[0] = vector[0];
  vec_vector[1] = vector[1];
  vec_vector[2] = vector[2];

  Eigen::Vector3d result = this->ProjectVectorToFace(vec_normal, vec_vector);
  vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
  return resultvnl;

}

//---------------------------------------------------------------------------
vnl_vector_fixed<float, DIMENSION>
VtkMeshWrapper::SampleNormalAtPoint(VtkMeshWrapper::PointType p, int idx) const
{

  double point[3];
  point[0] = p[0];
  point[1] = p[1];
  point[2] = p[2];

  int face_index = this->GetTriangleForPoint(point, idx);

  auto cell = this->poly_data_->GetCell(face_index);

  vnl_vector_fixed<float, DIMENSION> weightedNormal(0, 0, 0);

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[3];
  cell->EvaluatePosition(point, closest, sub_id, pcoords, dist2, weights);

  for (int i = 0; i < cell->GetNumberOfPoints(); i++) {

    auto id = cell->GetPointId(i);

    double* normal = this->poly_data_->GetPointData()->GetNormals()->GetTuple(id);

    weightedNormal[0] = weightedNormal[0] + normal[0] * weights[i];
    weightedNormal[1] = weightedNormal[1] + normal[1] * weights[i];
    weightedNormal[2] = weightedNormal[2] + normal[2] * weights[i];
  }
  return weightedNormal;
}

//---------------------------------------------------------------------------
VtkMeshWrapper::GradNType
VtkMeshWrapper::SampleGradNAtPoint(VtkMeshWrapper::PointType p, int idx) const
{
  return shapeworks::VtkMeshWrapper::GradNType();
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType
VtkMeshWrapper::SnapToMesh(VtkMeshWrapper::PointType pointa, int idx) const
{
  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];

  double closest_point[3];//the coordinates of the closest point will be returned here
  double closest_point_dist2; //the squared distance to the closest point will be returned here
  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id; //this is rarely used (in triangle strips only, I believe)

  this->cell_locator_->FindClosestPoint(point, closest_point, cell_id, sub_id, closest_point_dist2);

  VtkMeshWrapper::PointType out;
  out[0] = closest_point[0];
  out[1] = closest_point[1];
  out[2] = closest_point[2];
  return out;
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType VtkMeshWrapper::GetPointOnMesh() const
{
  int face_index = 0;

  double* point = this->poly_data_->GetPoint(face_index);

  PointType p;
  p[0] = point[0];
  p[1] = point[1];
  p[2] = point[2];

  return p;
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::GetTriangleForPoint(const double pt[3], int idx) const
{
  double closest_point[3];//the coordinates of the closest point will be returned here
  double closest_point_dist2; //the squared distance to the closest point will be returned here
  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id; //this is rarely used (in triangle strips only, I believe)

  this->cell_locator_->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

  return cell_id;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
VtkMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d& normal, const Eigen::Vector3d& vector)
{
  return vector - normal * normal.dot(vector);
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ComputeMeshBounds()
{
  double bounds[6];
  this->poly_data_->GetBounds(bounds);
  this->mesh_lower_bound_[0] = bounds[0];
  this->mesh_lower_bound_[1] = bounds[2];
  this->mesh_lower_bound_[2] = bounds[4];
  this->mesh_upper_bound_[0] = bounds[1];
  this->mesh_upper_bound_[1] = bounds[3];
  this->mesh_upper_bound_[2] = bounds[5];
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ComputeGradN()
{

}

}