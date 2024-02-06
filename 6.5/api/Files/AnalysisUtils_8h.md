---
title: Studio/Utils/AnalysisUtils.h

---

# Studio/Utils/AnalysisUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::AnalysisUtils](../Classes/classshapeworks_1_1AnalysisUtils.md)**  |




## Source code

```cpp
#pragma once

#include <Particles/ParticleSystemEvaluation.h>

namespace shapeworks {

  class Session;

  class AnalysisUtils {
  public:

    static ParticleSystemEvaluation get_local_particle_system(Session* session, int domain);


  };

}
```


-------------------------------

Updated on 2024-02-06 at 21:07:32 +0000
