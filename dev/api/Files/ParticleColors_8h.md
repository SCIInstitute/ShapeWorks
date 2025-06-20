---
title: Studio/Visualization/ParticleColors.h

---

# Studio/Visualization/ParticleColors.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleColors](../Classes/classshapeworks_1_1ParticleColors.md)** <br>Supplies particle colors.  |




## Source code

```cpp
#pragma once

#include <vtkLookupTable.h>

#include <QObject>

namespace shapeworks {


class ParticleColors : public QObject {
  Q_OBJECT;

 public:
  enum ParticleColorsType { Distinct = 0, Original = 1 };
  Q_ENUM(ParticleColorsType);

  static vtkSmartPointer<vtkLookupTable> construct_distinct();
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-06-20 at 19:54:39 +0000
