---
title: Studio/Interface/UpdateChecker.h

---

# Studio/Interface/UpdateChecker.h



## Namespaces

| Name           |
| -------------- |
| **[Ui](../Namespaces/namespaceUi.md)**  |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::UpdateChecker](../Classes/classshapeworks_1_1UpdateChecker.md)** <br>Update Checker.  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class UpdateChecker;
}

namespace shapeworks {


class UpdateChecker : public QDialog {
  Q_OBJECT

 public:
  explicit UpdateChecker(Preferences& prefs, QWidget* parent = nullptr);
  ~UpdateChecker();

  void run_auto_update_check();
  void run_manual_update_check();

 public Q_SLOTS:
  void handleNetworkReply(QNetworkReply* reply);

 private:
  void run_update_check();

  bool manual_trigger_{false};
  QNetworkAccessManager network_;

  Ui::UpdateChecker* ui_;

  Preferences& prefs_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-26 at 20:42:16 +0000
