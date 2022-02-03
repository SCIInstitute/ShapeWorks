---
title: Studio/src/Interface/LogWindow.h

---

# Studio/src/Interface/LogWindow.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LogWindow](../Classes/classshapeworks_1_1LogWindow.md)** <br>Log Window.  |




## Source code

```cpp
#pragma once

#include <QDialog>

#include <Interface/StatusBarWidget.h>

// Forward Qt class declarations
class Ui_LogWindow;

namespace shapeworks {


class LogWindow : public QDialog {
Q_OBJECT

public:

  LogWindow(QWidget* parent = 0);

  void add_message(MessageType message_type, QString message);

private:

  Ui_LogWindow* ui_;

};

}
```


-------------------------------

Updated on 2022-02-03 at 23:25:53 +0000
