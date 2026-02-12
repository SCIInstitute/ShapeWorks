#pragma once

#include "ParticleSystem.h"
#include "Procrustes3D.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
/** \class ParticleProcrustesRegistration
 *  \brief
 *
 *
 *
 */
class ProcrustesRegistration {
 public:
  using Pointer = std::shared_ptr<ProcrustesRegistration>;

  // Particle system typedefs
  using ParticleSystemType = ParticleSystem;
  using PointType = ParticleSystemType::PointType;

  void SetParticleSystem(ParticleSystemType* p) { m_ParticleSystem = p; }
  ParticleSystemType* GetParticleSystem() const { return m_ParticleSystem; }
  ParticleSystemType* GetParticleSystem() { return m_ParticleSystem; }

  //! Performs a procrustes registration and modifies the transforms of the
  //! particle system accordingly.  Assumes m_ParticleSystem has been set to point to a valid object.
  void RunRegistration(int i);
  //! Run registration on all domains
  void RunRegistration();

  //! Set/Get the number of Domains in each shape.  Procrustes will be performed separately for each domain.
  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  bool GetScaling() const { return m_Scaling; }
  void SetScaling(bool scaling) { m_Scaling = scaling; }

  bool GetRotationTranslation() const { return m_RotationTranslation; }
  void SetRotationTranslation(bool rotationTranslation) { m_RotationTranslation = rotationTranslation; }

 private:
  //! Standard GPA registration on all shapes (no fixed domains)
  void RunGPARegistration(int domainStart, int numShapes, int numPoints);

  //! Fixed domain registration: aligns non-fixed shapes to cached fixed mean
  void RunFixedDomainRegistration(int domainStart, int numShapes, int numPoints,
                                  const std::vector<bool>& is_fixed);

  //! Extract particle positions for a single domain.
  //! If fully_transformed is true, applies both prefix and Procrustes transforms (world space).
  //! If false, applies only the prefix transform (for computing new Procrustes transforms).
  Procrustes3D::ShapeType ExtractShape(int domain_index, int num_points, bool fully_transformed = false);

  int m_DomainsPerShape = 1;
  bool m_Scaling = true;
  bool m_RotationTranslation = true;
  ParticleSystemType* m_ParticleSystem = nullptr;

  // Cache for fixed domain alignment optimization.
  // When fixed domains are present, the mean of fixed shapes is computed once
  // (via GPA on fixed shapes only) and cached. Non-fixed shapes are then
  // aligned to this cached mean using OPA, avoiding full GPA on all shapes.
  struct FixedDomainCache {
    Procrustes3D::ShapeType mean;  // Mean of aligned fixed shapes (centered at origin)
    int num_points = 0;            // Particle count when cache was built
    bool valid = false;
  };
  std::vector<FixedDomainCache> m_FixedDomainCaches;
};

}  // namespace shapeworks
