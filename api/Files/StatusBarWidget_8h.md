---
title: Studio/src/Interface/StatusBarWidget.h

---

# Studio/src/Interface/StatusBarWidget.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StatusBarWidget](../Classes/classshapeworks_1_1StatusBarWidget.md)** <br>[StatusBarWidget]().  |




## Source code

```cpp
#pragma once

#include <QMainWindow>

// Forward Qt class declarations
class Ui_StatusBarWidget;

namespace shapeworks {

enum class MessageType {
  normal,
  warning,
  error,
  debug
};


class StatusBarWidget : public QWidget {
Q_OBJECT

public:
  StatusBarWidget(QMainWindow* parent = 0);

  void set_message(MessageType message_type, QString message);
  void set_progress(int value);

  void resizeEvent(QResizeEvent* event) override;

Q_SIGNALS:
  void toggle_log_window();

private:

  void update_layout();

  Ui_StatusBarWidget* ui_;

  QStatusBar* q_status_bar_;

  QIcon normal_message_icon_;
  QIcon warning_message_icon_;
  QIcon error_message_icon_;

};

} // shapeworks
```


-------------------------------

Updated on 2022-01-27 at 02:24:33 +0000
