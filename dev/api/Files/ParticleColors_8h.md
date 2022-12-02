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
| class | **[shapeworks::ParticleColors](../Classes/classshapeworks_1_1ParticleColors.md)**  |




## Source code

```cpp
#pragma once

#include <QObject>
#include <vtkLookupTable.h>

namespace shapeworks {

class ParticleColors : public QObject {
  Q_OBJECT;

 public:
  enum ParticleColorsType { Distinct = 0, Original = 1 };
  Q_ENUM(ParticleColorsType);

  static vtkSmartPointer<vtkLookupTable> construct(ParticleColorsType type, int num);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-12-02 at 23:43:49 +0000
