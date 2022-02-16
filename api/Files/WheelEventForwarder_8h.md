---
title: Studio/src/Interface/WheelEventForwarder.h

---

# Studio/src/Interface/WheelEventForwarder.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-02-16 at 06:47:28 +0000
