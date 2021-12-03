---
title: Libs/Optimize/ParticleSystem/CorrespondenceMode.h

---

# Libs/Optimize/ParticleSystem/CorrespondenceMode.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |




## Source code

```cpp
#pragma once

namespace shapeworks {

  enum class CorrespondenceMode : int {
    MeanEnergy = 0,
    EnsembleEntropy = 1,
    EnsembleRegressionEntropy = 3,
    EnsembleMixedEffectsEntropy = 4,
    MeshBasedGeneralEntropy = 5,
    MeshBasedGeneralMeanEnergy = 6
  };
}
```


-------------------------------

Updated on 2021-12-03 at 20:11:58 +0000
