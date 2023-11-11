#include "MeshDomain.h"

namespace shapeworks {

//-------------------------------------------------------------------
bool MeshDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  if (!mesh_wrapper_) {
    return true;
  }
  p = mesh_wrapper_->SnapToMesh(p, idx);
  return true;
}

//-------------------------------------------------------------------
bool MeshDomain::ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const {
  // TODO need to refactor into update particle method
  // TODO reduce magnitude of vector so it doesn't violate constraints
  return true;
}

//-------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION> MeshDomain::ProjectVectorToSurfaceTangent(
    vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const {
  return mesh_wrapper_->ProjectVectorToSurfaceTangent(pos, idx, gradE);
}

//-------------------------------------------------------------------
MeshDomain::PointType MeshDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                         vnl_vector_fixed<double, DIMENSION> &update) const {
  vnl_vector_fixed<double, DIMENSION> negativeUpdate;
  for (unsigned int i = 0; i < DIMENSION; i++) {
    negativeUpdate[i] = -update[i];
  }
  PointType newPoint = mesh_wrapper_->GeodesicWalk(point, idx, negativeUpdate);
  return newPoint;
}

//-------------------------------------------------------------------
double MeshDomain::GetMaxDiameter() const {
  // todo should this not be the length of the bounding box diagonal?
  PointType boundingBoxSize = mesh_wrapper_->GetMeshUpperBound() - mesh_wrapper_->GetMeshLowerBound();
  double max = 0;
  for (int d = 0; d < 3; d++) {
    max = max > boundingBoxSize[d] ? max : boundingBoxSize[d];
  }
  return max;
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleGradientAtPoint(const PointType &point, int idx) const {
  return mesh_wrapper_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleNormalAtPoint(const PointType &point, int idx) const {
  return mesh_wrapper_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
ParticleDomain::GradNType MeshDomain::SampleGradNAtPoint(const PointType &p, int idx) const {
  return mesh_wrapper_->SampleGradNAtPoint(p, idx);
}

//-------------------------------------------------------------------
double MeshDomain::Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                            vnl_vector_fixed<double, 3> *out_grad) const {
  return geodesics_mesh_->ComputeDistance(a, idx_a, b, idx_b, out_grad);
}

//-------------------------------------------------------------------
double MeshDomain::SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const {
  double dist = geodesics_mesh_->ComputeDistance(a, idx_a, b, idx_b);
  return dist * dist;
}

//-------------------------------------------------------------------
bool MeshDomain::IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                                  double &dist) const {
  return geodesics_mesh_->IsWithinDistance(a, idx_a, b, idx_b, test_dist, dist);
}

//-------------------------------------------------------------------
void MeshDomain::SetMesh(std::shared_ptr<MeshWrapper> mesh_) {
  m_FixedDomain = false;
  mesh_wrapper_ = mesh_;
  sw_mesh_ = std::make_shared<Mesh>(mesh_wrapper_->GetPolydata());

  auto poly_data = mesh_wrapper_->GetPolydata();
  Mesh mesh_copy(poly_data);
  mesh_copy.remeshPercent(50, 1.0);

  geodesics_mesh_ = std::make_shared<MeshWrapper>(mesh_copy.getVTKMesh());
}

//-------------------------------------------------------------------
void MeshDomain::InvalidateParticlePosition(int idx) const { this->mesh_wrapper_->InvalidateParticle(idx); }

//-------------------------------------------------------------------
ParticleDomain::PointType MeshDomain::GetZeroCrossingPoint() const {
  // TODO Hong
  // Apply constraints somehow
  if (mesh_wrapper_ == nullptr) {
    // Fixed domain. Unsure if this is the correct thing to do, but it preserves existing behaviour.
    PointType p;
    p[0] = p[1] = p[2] = 0;
    return p;
  }
  return mesh_wrapper_->GetPointOnMesh();
}

//-------------------------------------------------------------------
ParticleDomain::PointType MeshDomain::GetValidLocationNear(PointType p) const {
  PointType valid;
  valid[0] = p[0];
  valid[1] = p[1];
  valid[2] = p[2];
  ApplyConstraints(valid, -1);
  return valid;
}

}  // namespace shapeworks
