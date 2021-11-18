---
title: Studio/src/DeepSSM/DeepSSMTool.h

---

# Studio/src/DeepSSM/DeepSSMTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DeepSSMTool](../Classes/classshapeworks_1_1DeepSSMTool.md)**  |




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
#include <Data/ShapeWorksWorker.h>

// Studio
#include <Data/Shape.h>
#include <Data/Preferences.h>
#include <Visualization/Visualizer.h>
#include <Visualization/BarGraph.h>
#include <Python/PythonWorker.h>

class Ui_DeepSSMTool;
class QLabel;
class QTableWidget;
class QLineEdit;

namespace shapeworks {

class Session;
class Lightbox;
class DeepSSMJob;
class ShapeWorksStudioApp;

class DeepSSMTool : public QWidget {
Q_OBJECT;
public:


  enum class ToolMode {
    DeepSSM_SplitType, DeepSSM_AugmentationType, DeepSSM_TrainingType, DeepSSM_TestingType
  };

  DeepSSMTool(Preferences& prefs);
  ~DeepSSMTool();

  void set_session(QSharedPointer<Session> session);

  void set_app(ShapeWorksStudioApp* app);

  void set_active(bool active);

  bool get_active();

  void load_params();
  void store_params();

  void shutdown();

  QVector<QSharedPointer<Shape>> get_shapes();

  void resizeEvent(QResizeEvent* event) override;

  std::string get_display_feature();

public Q_SLOTS:

  void run_clicked();
  void restore_defaults();

  void handle_thread_complete();

  void handle_progress(int val);
  void handle_error(QString msg);

  void tab_changed(int tab);

  void update_panels();
  void update_split(QLineEdit *source);

  void handle_new_mesh();

  void training_fine_tuning_changed();

signals:

  void update_view();
  void progress(int);
  void message(QString);
  void error(QString);
  void warning(QString);

private:

  void update_meshes();
  void run_tool(DeepSSMTool::ToolMode type);
  void show_augmentation_meshes();
  void update_tables();
  void show_training_meshes();
  void show_testing_meshes();
  void update_testing_meshes();
  void load_plots();
  void resize_plots();
  QPixmap load_plot(QString filename);
  void set_plot(QLabel *qlabel, QPixmap pixmap);

  void populate_table_from_csv(QTableWidget *table, QString filename, bool header);

  Preferences& preferences_;

  Ui_DeepSSMTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  bool tool_is_running_ = false;
  DeepSSMTool::ToolMode current_tool_ = DeepSSMTool::ToolMode::DeepSSM_AugmentationType;
  QSharedPointer<DeepSSMJob> deep_ssm_;
  QElapsedTimer timer_;

  QVector<QSharedPointer<Shape>> shapes_;
  QPixmap violin_plot_;
  QPixmap training_plot_;

};

}
```


-------------------------------

Updated on 2021-11-18 at 01:30:46 +0000
