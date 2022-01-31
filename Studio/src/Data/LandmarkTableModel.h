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

 public:
  //! Constructor
  LandmarkTableModel(QObject *parent = 0);

  //! Destructor
  virtual ~LandmarkTableModel();

  //! Attach to a session
  void set_session(QSharedPointer<Session> session);

  //! Store landmarks back to project
  void store_landmarks();

  //! Set the currently active domain
  void set_active_domain(int domain);

  //! Update the table of landmarks from the project
  void update_table();

  //! Update table cells
  void update_cells();

  //! Remove landmarks with specified row indices.
  void remove_rows(const std::vector<int> &rows);

  //! Toggle tri-state button for visibility in horizontal header
  void toggle_visible();

  //! Set the currently placing landmark
  void set_placing_landmark(int row);

  //! delete the selected landmarks
  void delete_landmarks(const QModelIndexList &list);

  //------------------  QAbstractTableModel implementation ------------------

  //! QAbstractTableModel::rowCount implementation
  int rowCount(const QModelIndex &index) const override;

  //! QAbstractTableModel::columnCount implementation
  int columnCount(const QModelIndex &index) const override;

  //! QAbstractTableModel::data implementation
  QVariant data(const QModelIndex &index, int role) const override;

  //! QAbstractTableModel::setData implementation
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  //! QAbstractTableModel::headerData implementation
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  //! QAbstractTableModel::flags implementation
  Qt::ItemFlags flags(const QModelIndex &index) const override;

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
  //! Update visibility of status based on each landmark
  void update_visibility();

  //! Utility to remove a row from an Eigen::MatrixXd
  static void remove_eigen_row(Eigen::MatrixXd &matrix, unsigned int row_to_remove);

  std::shared_ptr<Project> project_;
  QSharedPointer<Session> session_;

  int visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
  std::string button_text_ = "  Place  ";

  std::vector<LandmarkDefinition> landmarks_;

  QIcon visible_;
  QIcon visible_off_;

  int active_domain_;
};
}  // namespace shapeworks
