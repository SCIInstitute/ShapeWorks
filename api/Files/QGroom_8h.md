---
title: Studio/src/Groom/QGroom.h

---

# Studio/src/Groom/QGroom.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::QGroom](../Classes/classshapeworks_1_1QGroom.md)** <br>Qt Wrapper for [Groom](../Classes/classshapeworks_1_1Groom.md).  |




## Source code

```cpp
#pragma once

#include <QObject>

#ifndef Q_MOC_RUN
#include <Libs/Groom/Groom.h>
#endif

namespace shapeworks {


class QGroom : public QObject, public Groom {

Q_OBJECT;

public:

  QGroom(ProjectHandle project);

protected:
  // override update_progress to emit q_signal
  void update_progress();

Q_SIGNALS:
  void progress(int);

};
}
```


-------------------------------

Updated on 2022-01-12 at 19:02:31 +0000
