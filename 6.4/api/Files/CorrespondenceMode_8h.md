---
title: Libs/Optimize/CorrespondenceMode.h

---

# Libs/Optimize/CorrespondenceMode.h



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

Updated on 2022-11-20 at 18:23:11 +0000
