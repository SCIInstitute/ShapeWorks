---
title: Studio/src/Interface/KeyboardShortcuts.h

---

# Studio/src/Interface/KeyboardShortcuts.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-03-05 at 23:20:35 +0000
