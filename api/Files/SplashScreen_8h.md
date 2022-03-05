---
title: Studio/src/Interface/SplashScreen.h

---

# Studio/src/Interface/SplashScreen.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::SplashScreen](../Classes/classshapeworks_1_1SplashScreen.md)**  |




## Source code

```cpp
#pragma once

// qt
#include <QDialog>

// studio
#include <Data/Preferences.h>

class Ui_SplashScreen;
class QListWidgetItem;

namespace shapeworks {

class SplashScreen : public QDialog {
Q_OBJECT

public:

  SplashScreen(QWidget* parent, Preferences& preferences);

  virtual ~SplashScreen();

private Q_SLOTS:

  void new_project();

  void open_existing();

  void open_recent();

  void quit();

  void enable_load_recent_button(QListWidgetItem* item);

Q_SIGNALS:

  void open_project(QString file_path);

private:

  void resizeEvent(QResizeEvent* event) override;

  void populate_recent_projects();

  Ui_SplashScreen* ui_;

  Preferences& preferences_;
};

} // end namespace
```


-------------------------------

Updated on 2022-03-05 at 23:20:35 +0000
