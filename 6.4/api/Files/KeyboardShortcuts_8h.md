---
title: Studio/Interface/KeyboardShortcuts.h

---

# Studio/Interface/KeyboardShortcuts.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::KeyboardShortcuts](../Classes/classshapeworks_1_1KeyboardShortcuts.md)**  |




## Source code

```cpp
#pragma once

// qt
#include <QDialog>

class Ui_KeyboardShortcuts;

namespace shapeworks {

class KeyboardShortcuts : public QDialog {
Q_OBJECT

public:

  KeyboardShortcuts(QWidget* parent);

  virtual ~KeyboardShortcuts();

private:

  Ui_KeyboardShortcuts* ui_;

};

} // end namespace
```


-------------------------------

Updated on 2023-04-29 at 18:09:09 +0000