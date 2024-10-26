---
title: Studio/Analysis/ShapeScalarPanel.h

---

# Studio/Analysis/ShapeScalarPanel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeScalarPanel](../Classes/classshapeworks_1_1ShapeScalarPanel.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <ParticleShapeStatistics.h>

// Studio
#include <Data/Preferences.h>
#include <Job/ShapeScalarJob.h>
#include <Shape.h>
#include <Visualization/Visualizer.h>

class Ui_ShapeScalarPanel;
class JKQTPlotter;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class ShapeScalarPanel : public QWidget {
  Q_OBJECT;

 public:
  ShapeScalarPanel(QWidget* parent = 0);
  ~ShapeScalarPanel();

  void set_session(QSharedPointer<Session> session);
  void reset();

 public Q_SLOTS:

  void run_clicked();

  void handle_job_progress(int progress);
  void handle_job_complete();

 Q_SIGNALS:

  void update_view();

 private:
  void update_run_button();

  void update_graphs();

  QSharedPointer<Session> session_;
  QSharedPointer<ShapeScalarJob> job_;

  Ui_ShapeScalarPanel* ui_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-10-26 at 19:00:31 +0000
