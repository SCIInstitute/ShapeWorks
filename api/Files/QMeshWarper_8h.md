---
title: Studio/src/Data/QMeshWarper.h

---

# Studio/src/Data/QMeshWarper.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::QMeshWarper](../Classes/classshapeworks_1_1QMeshWarper.md)** <br>Wraps [MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md) as a QObject.  |




## Source code

```cpp
#pragma once

#include <Libs/Mesh/MeshWarper.h>
#include <QObject>
#include <QMutex>
#include <QElapsedTimer>

namespace shapeworks {

class QMeshWarper : public QObject, public MeshWarper {
Q_OBJECT;

public:
  QMeshWarper(QObject* parent = nullptr);

  float get_progress();

protected:

  void update_progress(float p) override;

Q_SIGNALS:
  void progress();

private:
  float current_progress_ = 0;
};

}
```


-------------------------------

Updated on 2022-02-04 at 06:14:24 +0000
