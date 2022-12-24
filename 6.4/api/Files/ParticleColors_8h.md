---
title: Studio/src/Visualization/ParticleColors.h

---

# Studio/src/Visualization/ParticleColors.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-12-24 at 10:20:12 +0000
