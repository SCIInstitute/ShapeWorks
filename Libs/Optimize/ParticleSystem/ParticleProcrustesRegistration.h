#pragma once

#include "itkParticleSystem.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
/** \class ParticleProcrustesRegistration
 *  \brief
 *
 *
 *
 */
class ParticleProcrustesRegistration {
 public:
  using Pointer = std::shared_ptr<ParticleProcrustesRegistration>;

  /** Particle system typedefs. */
  using ParticleSystemType = itk::ParticleSystem;
  using PointType = ParticleSystemType::PointType;

  /** Set/Get the target particle system. */
  void SetParticleSystem(ParticleSystemType *p) { m_ParticleSystem = p; }
  ParticleSystemType *GetParticleSystem() const { return m_ParticleSystem; }
  ParticleSystemType *GetParticleSystem() { return m_ParticleSystem; }

  /** Performs a procrustes registration and modifies the transforms of the
      particle system accordingly.  Assumes m_ParticleSystem has been set to
      point to a valid object.*/
  void RunRegistration(int i);
  void RunRegistration();

  /** Set/Get the number of Domains in each shape.  Procrustes will be
      performed separately for each domain. */
  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  bool GetScaling() const { return m_Scaling; }
  void ScalingOn() { m_Scaling = true; }
  void ScalingOff() { m_Scaling = false; }

  bool GetRotationTranslation() const { return m_RotationTranslation; }
  void RotationTranslationOn() { m_RotationTranslation = true; }
  void RotationTranslationOff() { m_RotationTranslation = false; }

 private:
  int m_DomainsPerShape = 1;
  bool m_Scaling = true;
  bool m_RotationTranslation = true;
  ParticleSystemType *m_ParticleSystem = nullptr;
};

}  // namespace shapeworks
