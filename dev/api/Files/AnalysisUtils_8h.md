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

#include <Particles/ParticleSystem.h>

namespace shapeworks {

  class Session;

  class AnalysisUtils {
  public:

    static ParticleSystem get_local_particle_system(Session* session, int domain);


  };

}
```


-------------------------------

Updated on 2023-01-16 at 21:02:46 +0000
