#include "VtkMeshWrapper.h"


namespace shapeworks{


//---------------------------------------------------------------------------
VtkMeshWrapper::VtkMeshWrapper(vtkSmartPointer<vtkPolyData> poly_data) {

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
  return shapeworks::VtkMeshWrapper::PointType();
}

//---------------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION>
VtkMeshWrapper::ProjectVectorToSurfaceTangent(const VtkMeshWrapper::PointType& pointa, int idx,
                                              vnl_vector_fixed<double, 3>& vector) const
{
  return vnl_vector_fixed<double, 3>();
}

//---------------------------------------------------------------------------
vnl_vector_fixed<float, DIMENSION>
VtkMeshWrapper::SampleNormalAtPoint(VtkMeshWrapper::PointType p, int idx) const
{
  return vnl_vector_fixed<float, 3>();
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
  return shapeworks::VtkMeshWrapper::PointType();
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType VtkMeshWrapper::GetPointOnMesh() const
{
  return shapeworks::VtkMeshWrapper::PointType();
}

}