#include "MeshDomain.h"

namespace shapeworks {

//-------------------------------------------------------------------
bool MeshDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  return image_->ApplyConstraints(p, idx, dbg);
}


//-------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION> MeshDomain::ProjectVectorToSurfaceTangent(
    vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const {
  return image_->ProjectVectorToSurfaceTangent(gradE, pos, idx);
}

//-------------------------------------------------------------------
MeshDomain::PointType MeshDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                         vnl_vector_fixed<double, DIMENSION> &update) const {

  return image_->UpdateParticlePosition(point, idx, update);                                                                    
}

//-------------------------------------------------------------------
double MeshDomain::GetMaxDiameter() const {
  return image_->GetMaxDiameter();
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleGradientAtPoint(const PointType &point, int idx) const {
  return image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleNormalAtPoint(const PointType &point, int idx) const {
  return image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
ParticleDomain::GradNType MeshDomain::SampleGradNAtPoint(const PointType &p, int idx) const {
  return image_->SampleGradNAtPoint(p, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleGradientAtPoint_ct(const PointType &point, int idx) const {
  return ct_image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> MeshDomain::SampleNormalAtPoint_ct(const PointType &point, int idx) const {
  return ct_image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
ParticleDomain::GradNType MeshDomain::SampleGradNAtPoint_ct(const PointType &p, int idx) const {
  return ct_image_->SampleGradNAtPoint(p, idx);
}

//-------------------------------------------------------------------
void SetVolume(std::shared_ptr<ImplicitSurfaceDomain> image, std::shared_ptr<ImplicitSurfaceDomain> ct_image) {
  m_FixedDomain = false;
  ct_image_ = ct_image;
  image_ = image;
}

//-------------------------------------------------------------------
ParticleDomain::PointType MeshDomain::GetValidLocationNear(PointType p) const {
  return image_->GetValidLocationNear(p);
}

}  // namespace shapeworks
