#pragma once

#include <itkObjectFactory.h>

#include "Surface.h"
#include "ParticleDomain.h"

namespace shapeworks {

class MeshDomain : public ParticleDomain {
 public:
  using Pointer = std::shared_ptr<MeshDomain>;

  DomainType GetDomainType() const override { return DomainType::Mesh; }

  bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override;
  bool ApplyVectorConstraints(VectorDoubleType &gradE, const PointType &pos) const;
  VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType &gradE, const PointType &pos, int idx) const override;
  PointType UpdateParticlePosition(const PointType &point, int idx, VectorDoubleType &update) const override;

  virtual void InvalidateParticlePosition(int idx) const override;

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

  const PointType &GetLowerBound() const override { return surface_->get_mesh_lower_bound(); }
  const PointType &GetUpperBound() const override { return surface_->get_mesh_upper_bound(); }

  PointType GetZeroCrossingPoint() const override;

  PointType GetValidLocationNear(PointType p) const override;

  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }

  double GetMaxDiameter() const override;

  vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const override;

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &point, int idx) const override;

  GradNType SampleGradNAtPoint(const PointType &p, int idx) const override;

  double Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                  vnl_vector_fixed<double, DIMENSION> *out_grad = nullptr) const override;

  double SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const override;

  bool IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                        double &dist) const override;

  void DeleteImages() override {
    // TODO Change this to a generic delete function
  }

  void DeletePartialDerivativeImages() override {
    // TODO Change this to a generic delete function
  }

  void SetMesh(std::shared_ptr<Surface> mesh_, double geodesic_remesh_percent);

  std::shared_ptr<Mesh> GetSWMesh() const { return sw_mesh_; }

  void UpdateZeroCrossingPoint() override {}

  std::shared_ptr<Surface> get_surface() const { return surface_; }

 private:
  std::shared_ptr<Surface> surface_;
  std::shared_ptr<Surface> geodesics_mesh_;
  std::shared_ptr<Mesh> sw_mesh_;
  PointType zero_crossing_point_;
};

}  // namespace shapeworks
