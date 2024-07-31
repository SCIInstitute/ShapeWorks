---
title: Libs/Optimize/ProcrustesRegistration.h

---

# Libs/Optimize/ProcrustesRegistration.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ProcrustesRegistration](../Classes/classshapeworks_1_1ProcrustesRegistration.md)**  |




## Source code

```cpp
#pragma once

#include "ParticleSystem.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
class ProcrustesRegistration {
 public:
  using Pointer = std::shared_ptr<ProcrustesRegistration>;

  // Particle system typedefs
  using ParticleSystemType = ParticleSystem;
  using PointType = ParticleSystemType::PointType;

  void SetParticleSystem(ParticleSystemType* p) { m_ParticleSystem = p; }
  ParticleSystemType* GetParticleSystem() const { return m_ParticleSystem; }
  ParticleSystemType* GetParticleSystem() { return m_ParticleSystem; }

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
  ParticleSystemType* m_ParticleSystem = nullptr;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-31 at 08:14:45 +0000
