---
title: Studio/Data/DataTool.h

---

# Studio/Data/DataTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DataTool](../Classes/classshapeworks_1_1DataTool.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>

#include <QElapsedTimer>
#include <QObject>
#include <QProgressDialog>
#include <QSharedPointer>
#include <QWidget>
#include <memory>
#include <string>

class Ui_DataTool;
class QComboBox;

namespace shapeworks {

class SegmentationToolPanel;
class Session;
class LandmarkTableModel;

class DataTool : public QWidget {
  Q_OBJECT;

 public:
  DataTool(Preferences& prefs);
  ~DataTool();

  void set_session(QSharedPointer<Session> session);

  void activate();

  void disable_actions();

  void enable_actions();

  void update_table(bool clean = true);

  void update_landmark_table();

  void landmark_domain_changed();

  void constraints_domain_changed();

  void delete_planes_clicked();

  void delete_ffc_clicked();

  void update_notes();

  std::string get_notes();

  void store_data();

 public Q_SLOTS:

  void delete_button_clicked();

  void delete_landmarks_clicked();

  void set_placing_button_clicked(int id);

  void handle_landmark_mode_changed();
  void handle_constraints_mode_changed();

  void constraints_table_right_click(const QPoint& point);
  void data_table_right_click(const QPoint& point);
  void copy_ffc_clicked();

  void table_selection_changed();
  void subject_notes_changed();
  void table_data_edited();

 Q_SIGNALS:
  void import_button_clicked();

 private:
  void update_plane_table();
  void update_ffc_table();

  Preferences& preferences_;

  Ui_DataTool* ui_;
  SegmentationToolPanel* segmentation_tool_panel_;
  QSharedPointer<Session> session_;

  std::shared_ptr<LandmarkTableModel> landmark_table_model_;

  bool block_table_update_{false};
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-08-14 at 09:07:45 +0000
