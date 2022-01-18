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

enum LandmarkColumns { VISIBLE_E, COLOR_E, NAME_E, POSITION_E, SET_BUTTON_E, COMMENT_E, END_E };

enum LandmarkVisibility { ALL_VISIBLE_E, NONE_VISIBLE_E, SOME_VISIBLE_E };

class LandmarkTableModel : public QAbstractTableModel {
  Q_OBJECT

  friend class LandmarkTableModelPrivate;

 public:
  LandmarkTableModel(QObject *parent = 0);

  virtual ~LandmarkTableModel();

  void set_project(std::shared_ptr<Project> project);
  void set_session(QSharedPointer<Session> session);
  void store_landmarks();
  void set_active_domain(int domain);

  int rowCount(const QModelIndex &index) const override;
  int columnCount(const QModelIndex &index) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // Update entire table including dimensions.  Scroll to active index.
  // Stops any editing that the user may be doing on the table.  Clears currently selected cells.
  void update_table();

  // Update only table cells, not table dimensions.
  void update_cells();

  // Remove measurements with specified row indices.
  void remove_rows(const std::vector<int> &rows);

  // Handler for when table cells are selected.
  void handle_selected(const QItemSelection &selected);

  // Toggle tri-state button for visibility in horizontal header
  void toggle_visible();

  void set_placing_landmark(int row);
  void set_button_text(std::string text);

  //! delete the selected landmarks
  void delete_landmarks(const QModelIndexList &list);

 public Q_SLOTS:

  //! create a new landmark
  void new_landmark();

  //! Handler for when table cells are clicked.
  void handle_click(const QModelIndex &index);

  //! Handler for when table cells are double clicked.
  void handle_double_click(const QModelIndex &index);

  //! Handle header click
  void handle_header_click(int index);

 private:
  std::string get_active_domain_name();

  void update_visibility();
  void remove_eigen_row(Eigen::MatrixXd &matrix, unsigned int row_to_remove);

  std::shared_ptr<Project> project_;
  QSharedPointer<Session> session_;

  int visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
  std::string button_text_ = "  Place  ";

  int last_landmark_color_ = -1;
  std::vector<LandmarkDefinition> landmarks_;

  QIcon visible_;
  QIcon visible_off_;

  int active_domain_;
};
}  // namespace shapeworks
