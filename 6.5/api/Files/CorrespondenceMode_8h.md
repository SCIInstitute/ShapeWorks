---
title: Libs/Optimize/CorrespondenceMode.h

---

# Libs/Optimize/CorrespondenceMode.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |




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
    MeshBasedGeneralMeanEnergy = 6,
    DisentagledEnsembleEntropy = 7,
    DisentangledEnsembleMeanEnergy = 8
  };
}
```


-------------------------------

Updated on 2023-08-02 at 17:10:01 +0000