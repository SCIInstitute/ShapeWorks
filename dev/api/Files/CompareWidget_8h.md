---
title: Studio/src/Interface/CompareWidget.h

---

# Studio/src/Interface/CompareWidget.h



## Namespaces

| Name           |
| -------------- |
| **[Ui](../Namespaces/namespaceUi.md)**  |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::CompareWidget](../Classes/classshapeworks_1_1CompareWidget.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Session.h>

#include <QWidget>

namespace Ui {
class CompareWidget;
}

namespace shapeworks {

class CompareWidget : public QWidget {
  Q_OBJECT

 public:
  explicit CompareWidget(QWidget *parent = nullptr);
  ~CompareWidget();

  CompareSettings get_settings();
  void set_available(DisplayMode mode, bool available);

  bool check_any_available();

 Q_SIGNALS:
  void settings_changed();

 private:
  Ui::CompareWidget *ui_;
  CompareSettings settings_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-08-13 at 05:00:37 +0000
