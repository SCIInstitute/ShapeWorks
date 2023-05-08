---
title: Studio/Interface/WheelEventForwarder.h

---

# Studio/Interface/WheelEventForwarder.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::WheelEventForwarder](../Classes/classshapeworks_1_1WheelEventForwarder.md)**  |




## Source code

```cpp
#pragma once

#include <QtCore/QObject>

class QScrollBar;

namespace shapeworks {

class WheelEventForwarder : public QObject {
Q_OBJECT
public:
  explicit WheelEventForwarder(QScrollBar* target);
  ~WheelEventForwarder();

  bool eventFilter(QObject* obj, QEvent* event);

private:
  QScrollBar* target_;
};

}
```


-------------------------------

Updated on 2023-05-08 at 20:57:59 +0000
