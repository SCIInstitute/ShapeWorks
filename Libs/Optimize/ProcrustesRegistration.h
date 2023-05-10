#pragma once

#include "ParticleSystem.h"
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
  int m_DomainsPerShape = 1;
  bool m_Scaling = true;
  bool m_RotationTranslation = true;
  ParticleSystemType* m_ParticleSystem = nullptr;
};

}  // namespace shapeworks
