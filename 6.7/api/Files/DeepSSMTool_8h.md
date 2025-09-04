---
title: Studio/DeepSSM/DeepSSMTool.h

---

# Studio/DeepSSM/DeepSSMTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DeepSSMTool](../Classes/classshapeworks_1_1DeepSSMTool.md)**  |




## Source code

```cpp
#pragma once

// qt
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QWidget>

#include <DeepSSM/DeepSSMJob.h>

// studio
#include <Data/Preferences.h>
#include <Project.h>
#include <Shape.h>

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

  DeepSSMTool(Preferences& prefs);
  ~DeepSSMTool();

  void set_session(QSharedPointer<Session> session);

  void set_app(ShapeWorksStudioApp* app);

  bool is_active();

  void load_params();
  void store_params();

  void shutdown();

  ShapeList get_shapes();

  void resizeEvent(QResizeEvent* event) override;

  std::string get_display_feature();


 public Q_SLOTS:

  void run_clicked();
  void run_prep_clicked(int step);
  void restore_defaults();

  void handle_thread_complete();

  void handle_progress(int val, QString message);
  void handle_error(QString msg);

  void tab_changed(int tab);

  void update_panels();
  void update_split();

  void handle_new_mesh();

  void training_fine_tuning_changed();

 Q_SIGNALS:

  void update_view();
  void progress(int);

 private:
  void update_meshes();
  void run_tool(DeepSSMJob::JobType type);
  void show_augmentation_meshes();
  void update_tables();
  void show_training_meshes();
  void show_testing_meshes();
  void update_testing_meshes();
  void load_plots();
  void resize_plots();

  std::string get_feature_name();
  void set_subject_image_filename(const std::shared_ptr<Subject>& subject, const std::string& filename);

  QPixmap load_plot(QString filename);
  void set_plot(QLabel* qlabel, QPixmap pixmap);

  void populate_table_from_csv(QTableWidget* table, QString filename, bool header);

  QStringList read_images_from_csv(QString filename);

  Preferences& preferences_;

  Ui_DeepSSMTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;
  DeepSSMJob::PrepStep prep_step_ = DeepSSMJob::PrepStep::NOT_STARTED;

  bool tool_is_running_ = false;
  DeepSSMJob::JobType current_tool_ = DeepSSMJob::JobType::DeepSSM_AugmentationType;
  QSharedPointer<DeepSSMJob> deep_ssm_;
  QElapsedTimer timer_;

  ShapeList shapes_;
  QPixmap violin_plot_;
  QPixmap training_plot_;
  QPixmap training_plot_ft_;
  // training plots for TL mode
  QPixmap training_plot_tl1_;
  QPixmap training_plot_tl2_;
  QPixmap training_plot_tl3_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-04 at 08:01:27 +0000
