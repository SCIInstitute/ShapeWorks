---
title: Libs/Optimize/ParticleSystem/ParticleProcrustesRegistration.h

---

# Libs/Optimize/ParticleSystem/ParticleProcrustesRegistration.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleProcrustesRegistration](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md)**  |




## Source code

```cpp
#pragma once

#include "itkParticleSystem.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
class ParticleProcrustesRegistration {
 public:
  using Pointer = std::shared_ptr<ParticleProcrustesRegistration>;

  // Particle system typedefs
  using ParticleSystemType = itk::ParticleSystem;
  using PointType = ParticleSystemType::PointType;

  void SetParticleSystem(ParticleSystemType *p) { m_ParticleSystem = p; }
  ParticleSystemType *GetParticleSystem() const { return m_ParticleSystem; }
  ParticleSystemType *GetParticleSystem() { return m_ParticleSystem; }

  void RunRegistration(int i);
  void RunRegistration();

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
  ParticleSystemType *m_ParticleSystem = nullptr;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-22 at 21:16:04 +0000
