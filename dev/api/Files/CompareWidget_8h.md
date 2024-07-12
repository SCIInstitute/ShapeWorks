---
title: Studio/Interface/CompareWidget.h

---

# Studio/Interface/CompareWidget.h



## Namespaces

| Name           |
| -------------- |
| **[Ui](../Namespaces/namespaceUi.md)**  |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

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
  void set_mean_shape_available(bool available);

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

Updated on 2024-07-12 at 20:10:36 +0000
