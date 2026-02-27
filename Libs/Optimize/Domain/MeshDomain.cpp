#include "MeshDomain.h"

namespace shapeworks {

//-------------------------------------------------------------------
bool MeshDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  if (!surface_) {
    return true;
  }
  p = surface_->snap_to_mesh(p, idx);
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
  return surface_->project_vector_to_surface_tangent(pos, idx, gradE);
}

//-------------------------------------------------------------------
MeshDomain::PointType MeshDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                         vnl_vector_fixed<double, DIMENSION> &update) const {
  vnl_vector_fixed<double, DIMENSION> negativeUpdate;
  for (unsigned int i = 0; i < DIMENSION; i++) {
    negativeUpdate[i] = -update[i];
  }
  PointType newPoint = surface_->geodesic_walk(point, idx, negativeUpdate);
  return newPoint;
}

//-------------------------------------------------------------------
MeshDomain::PointType MeshDomain::GetPositionAfterSplit(const PointType &pt,
                                                         const VectorDoubleType &local_direction,
                                                         const VectorDoubleType &global_direction,
                                                         double epsilon) const {
  VectorDoubleType update;
  for (unsigned int k = 0; k < DIMENSION; k++) {
    // Negate here because UpdateParticlePosition negates before calling geodesic_walk
    update[k] = -epsilon * local_direction[k] / 5.0;
  }
  return UpdateParticlePosition(pt, -1, update);
}

//-------------------------------------------------------------------
double MeshDomain::GetMaxDiameter() const {
  // todo should this not be the length of the bounding box diagonal?
  PointType boundingBoxSize = surface_->get_mesh_upper_bound() - surface_->get_mesh_lower_bound();
  double max = 0;
  for (int d = 0; d < 3; d++) {
    max = max > boundingBoxSize[d] ? max : boundingBoxSize[d];
  }
  return max;
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleGradientAtPoint(const PointType &point, int idx) const {
  return surface_->sample_normal_at_point(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleNormalAtPoint(const PointType &point, int idx) const {
  return surface_->sample_normal_at_point(point, idx);
}

//-------------------------------------------------------------------
ParticleDomain::GradNType MeshDomain::SampleGradNAtPoint(const PointType &p, int idx) const {
  return surface_->sample_gradient_normal_at_point(p, idx);
}

//-------------------------------------------------------------------
double MeshDomain::Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                            vnl_vector_fixed<double, 3> *out_grad) const {
  return geodesics_mesh_->compute_distance(a, idx_a, b, idx_b, out_grad);
}

//-------------------------------------------------------------------
double MeshDomain::SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const {
  double dist = geodesics_mesh_->compute_distance(a, idx_a, b, idx_b);
  return dist * dist;
}

//-------------------------------------------------------------------
bool MeshDomain::IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                                  double &dist) const {
  return geodesics_mesh_->is_within_distance(a, idx_a, b, idx_b, test_dist, dist);
}

//-------------------------------------------------------------------
void MeshDomain::SetMesh(std::shared_ptr<Surface> mesh, double geodesic_remesh_percent) {
  m_FixedDomain = false;
  surface_ = mesh;
  sw_mesh_ = std::make_shared<Mesh>(surface_->get_polydata());

  surface_area_ = sw_mesh_->getSurfaceArea();

  if (geodesic_remesh_percent >= 100.0) {  // no remeshing
    geodesics_mesh_ = surface_;
  } else {
    auto poly_data = surface_->get_polydata();
    Mesh mesh_copy(poly_data);
    mesh_copy.remeshPercent(geodesic_remesh_percent, 1.0);
    geodesics_mesh_ = std::make_shared<Surface>(mesh_copy.getVTKMesh());
  }
}

//-------------------------------------------------------------------
void MeshDomain::SetMesh(std::shared_ptr<Surface> surface, std::shared_ptr<Surface> geodesics_surface,
                          std::shared_ptr<Mesh> sw_mesh, double surface_area) {
  m_FixedDomain = false;
  surface_ = surface;
  geodesics_mesh_ = geodesics_surface;
  sw_mesh_ = sw_mesh;
  surface_area_ = surface_area;
}

//-------------------------------------------------------------------
void MeshDomain::InvalidateParticlePosition(int idx) const { this->surface_->invalidate_particle(idx); }

//-------------------------------------------------------------------
ParticleDomain::PointType MeshDomain::GetZeroCrossingPoint() const {
  // TODO Hong
  // Apply constraints somehow
  if (surface_ == nullptr) {
    // Fixed domain. Unsure if this is the correct thing to do, but it preserves existing behaviour.
    PointType p;
    p[0] = p[1] = p[2] = 0;
    return p;
  }
  return surface_->get_point_on_mesh();
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
