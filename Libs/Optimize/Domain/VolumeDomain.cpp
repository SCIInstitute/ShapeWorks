#include "VolumeDomain.h"

namespace shapeworks {

//-------------------------------------------------------------------
bool VolumeDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  return image_->ApplyConstraints(p, idx, dbg);
}


//-------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION> VolumeDomain::ProjectVectorToSurfaceTangent(
    vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const {
  return image_->ProjectVectorToSurfaceTangent(gradE, pos, idx);
}

//-------------------------------------------------------------------
VolumeDomain::PointType VolumeDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                         vnl_vector_fixed<double, DIMENSION> &update) const {

  return image_->UpdateParticlePosition(point, idx, update);                                                                    
}

//-------------------------------------------------------------------
double VolumeDomain::GetMaxDiameter() const {
  return image_->GetMaxDiameter();
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> VolumeDomain::SampleGradientAtPoint(const PointType &point, int idx) const {
  return image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> VolumeDomain::SampleNormalAtPoint(const PointType &point, int idx) const {
  return image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
ParticleRegionDomain::GradNType VolumeDomain::SampleGradNAtPoint(const PointType &p, int idx) const {
  return image_->SampleGradNAtPoint(p, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> VolumeDomain::SampleGradientAtPoint_ct(const PointType &point, int idx) const {
  return ct_image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
vnl_vector_fixed<float, 3> VolumeDomain::SampleNormalAtPoint_ct(const PointType &point, int idx) const {
  return ct_image_->SampleNormalAtPoint(point, idx);
}

//-------------------------------------------------------------------
ParticleRegionDomain::GradNType VolumeDomain::SampleGradNAtPoint_ct(const PointType &p, int idx) const {
  return ct_image_->SampleGradNAtPoint(p, idx);
}

//-------------------------------------------------------------------
void VolumeDomain::SetVolume(ImageType::Pointer image, ImageType::Pointer ct_image, double narrow_band) {
  m_FixedDomain = false;
  ct_image_ = std::make_shared<ImplicitSurfaceDomain<ImageType::PixelType>>();
  image_ = std::make_shared<ImplicitSurfaceDomain<ImageType::PixelType>>();
  double narrow_band_world_ct = ct_image->GetSpacing().GetVnlVector().max_value() * narrow_band;
  ct_image_->SetImage(image, narrow_band_world_ct);
  double narrow_band_world = image->GetSpacing().GetVnlVector().max_value() * narrow_band;
  image_->SetImage(image, narrow_band_world);
}

}  // namespace shapeworks
