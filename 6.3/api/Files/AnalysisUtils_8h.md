---
title: Studio/src/Utils/AnalysisUtils.h

---

# Studio/src/Utils/AnalysisUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::AnalysisUtils](../Classes/classshapeworks_1_1AnalysisUtils.md)**  |




## Source code

```cpp
#pragma once

#include <Libs/Particles/ParticleSystem.h>

namespace shapeworks {

  class Session;

  class AnalysisUtils {
  public:

    static ParticleSystem get_local_particle_system(Session* session, int domain);


  };

}
```


-------------------------------

Updated on 2022-05-04 at 15:21:59 +0000
