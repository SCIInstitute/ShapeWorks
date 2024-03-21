#pragma once

#include <itkObjectFactory.h>

#include "ImplicitSurfaceDomain.h"
#include "ParticleDomain.h"

namespace shapeworks {

class VolumeDomain : public ParticleRegionDomain {
 public:
  using Pointer = std::shared_ptr<VolumeDomain>;

  DomainType GetDomainType() const override { return DomainType::Volume; }

  bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override;
  VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType &gradE, const PointType &pos, int idx) const override;
  PointType UpdateParticlePosition(const PointType &point, int idx, VectorDoubleType &update) const override;

  double GetCurvature(const PointType &p, int idx) const override {
    // TODO Why not return the actual curvature
    return GetSurfaceMeanCurvature();
  }

  inline double GetSurfaceMeanCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.15;
  }

  inline double GetSurfaceStdDevCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.02;
  }

  const PointType &GetLowerBound() const override { return mesh_wrapper_->GetMeshLowerBound(); }
  const PointType &GetUpperBound() const override { return mesh_wrapper_->GetMeshUpperBound(); }

  PointType GetValidLocationNear(PointType p) const override;

  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }

  double GetMaxDiameter() const override;

  vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const override;
  vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint_ct(const PointType &point, int idx);

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &point, int idx) const override;
  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint_ct(const PointType &point, int idx);

  GradNType SampleGradNAtPoint(const PointType &p, int idx) const override;
  GradNType SampleGradNAtPoint_ct(const PointType &p, int idx);

  void DeleteImages() override {
      image_->DeleteImages();
      ct_image->DeleteImages();
  }

  void DeletePartialDerivativeImages() override {
      image_->DeletePartialDerivativeImages();
      ct_image->DeletePartialDerivativeImages();
  }

  void SetVolume(std::shared_ptr<ImplicitSurfaceDomain> image, std::shared_ptr<ImplicitSurfaceDomain> ct_image);

  std::shared_ptr<Mesh> GetImageCT() const { return ct_image_; }
  // std::shared_ptr<Mesh> GetImage() const { return image_; }

  void UpdateZeroCrossingPoint() override {}

 private:
  std::shared_ptr<ImplicitSurfaceDomain> image_;
  std::shared_ptr<ImplicitSurfaceDomain> ct_image_;
  PointType zero_crossing_point_;
};

}  // namespace shapeworks
