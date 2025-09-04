---
title: Studio/Analysis/ParticleAreaPanel.h

---

# Studio/Analysis/ParticleAreaPanel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleAreaPanel](../Classes/classshapeworks_1_1ParticleAreaPanel.md)**  |




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
#include <Job/ParticleAreaJob.h>
#include <Shape.h>
#include <Visualization/Visualizer.h>

class Ui_ParticleAreaPanel;
class JKQTPlotter;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class ParticleAreaPanel : public QWidget {
  Q_OBJECT;

 public:
  ParticleAreaPanel(QWidget* parent = 0);
  ~ParticleAreaPanel();

  void set_session(QSharedPointer<Session> session);
  void reset();

  bool get_display_particle_area() const;

  Eigen::VectorXd get_computed_values() const;
  std::string get_computed_value_name() const;

 public Q_SLOTS:

  void run_clicked();
  void show_particle_area_clicked();

  void display_option_changed();

  void handle_job_progress(int progress);
  void handle_job_complete();

 Q_SIGNALS:

  void update_view();

 private:

  void update_run_button();

  void update_graphs();

  QSharedPointer<Session> session_;
  QSharedPointer<ParticleAreaJob> job_;

  Ui_ParticleAreaPanel* ui_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-04 at 08:01:27 +0000
