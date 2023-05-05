---
title: Studio/src/Data/PlaneTableModel.h

---

# Studio/src/Data/PlaneTableModel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PlaneInfo](../Classes/classshapeworks_1_1PlaneInfo.md)**  |
| class | **[shapeworks::PlaneTableModel](../Classes/classshapeworks_1_1PlaneTableModel.md)**  |




## Source code

```cpp
#pragma once

// QT Includes
#include <Data/Session.h>
#include <Libs/Project/Project.h>

#include <QAbstractTableModel>
#include <QIcon>
#include <QItemSelection>
#include <QModelIndex>

namespace shapeworks {

class Project;
class PlaneConstraint;

enum LandmarkColumns { VISIBLE_E, COLOR_E, NAME_E, POSITION_E, SET_BUTTON_E, COMMENT_E, END_E };

enum LandmarkVisibility { ALL_VISIBLE_E, NONE_VISIBLE_E, SOME_VISIBLE_E };


class PlaneInfo {
public:
  QString shape_;
  QString domain_;
  PlaneConstraint plane_;
};

class PlaneTableModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  PlaneTableModel(QObject *parent = 0);

  virtual ~PlaneTableModel();

  void set_session(QSharedPointer<Session> session);

  void store_landmarks();

  void set_active_domain(int domain);

  void update_table();

  void update_cells();

  void remove_rows(const std::vector<int> &rows);

  void toggle_visible();

  void set_placing_landmark(int row);

  void delete_landmarks(const QModelIndexList &list);

  //------------------  QAbstractTableModel implementation ------------------

  int rowCount(const QModelIndex &index) const override;

  int columnCount(const QModelIndex &index) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

 public Q_SLOTS:

  void new_landmark();

  void handle_click(const QModelIndex &index);

  void handle_double_click(const QModelIndex &index);

  void handle_header_click(int index);

 private:
  void update_visibility();

  static void remove_eigen_row(Eigen::MatrixXd &matrix, unsigned int row_to_remove);


  void update_planes();

  std::shared_ptr<Project> project_;
  QSharedPointer<Session> session_;

  int visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
  std::string button_text_ = "  Place  ";

  std::vector<LandmarkDefinition> landmarks_;

  std::vector<PlaneInfo> planes_;

  QIcon visible_;
  QIcon visible_off_;

  int active_domain_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-23 at 16:40:07 -0600
