#pragma once


// QT Includes
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QItemSelection>


namespace shapeworks
{

class Project;

enum LandmarkColumns {
  VISIBLE_E,
  COLOR_E,
  NAME_E,
  POSITION_E,
  SET_BUTTON_E,
  COMMENT_E,
  END_E
};

enum LandmarkVisibility {
  ALL_VISIBLE_E,
  NONE_VISIBLE_E,
  SOME_VISIBLE_E
};


class LandmarkTableModel : public QAbstractTableModel
{
  Q_OBJECT

  friend class LandmarkTableModelPrivate;

public:
  LandmarkTableModel( QObject* parent = 0 );

  virtual ~LandmarkTableModel();

  void set_project(std::shared_ptr<Project> project);

  //
  // Inherited functions
  //

  int rowCount( const QModelIndex &index ) const;
  int columnCount( const QModelIndex &index ) const;

  QVariant data( const QModelIndex& index, int role ) const;
  bool setData( const QModelIndex &index, const QVariant &value, int role );
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
  Qt::ItemFlags flags( const QModelIndex &index ) const;
  bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );


  //
  // Extended functions
  //

  // UPDATE_TABLE:
  // Update entire table including dimensions.  Scroll to active index.
  // Stops any editing that the user may be doing on the table.  Clears currently selected cells.
  void update_table();

  // UPDATE_CELLS:
  // Update only table cells, not table dimensions.
  void update_cells();

  // REMOVE_ROWS:
  // Remove measurements with specified row indices.
  void remove_rows( const std::vector< int >& rows );

  // GET_ACTIVE_INDEX:
  // Return row index of active measurement. Returns -1 if there are no measurements.
  // Needed by LandmarkTableView, which doesn't have direct access to LandmarkTool.
  // Locks: StateEngine
  int get_active_index() const;

  // HANDLE_SELECTED:
  // Handler for when table cells are selected.
  void handle_selected( const QItemSelection & selected );

  // TOGGLE_VISIBLE:
  // Toggle tri-state button for visibility in horizontal header
  void toggle_visible();

  void set_placing_landmark( int row );
  void set_button_text( std::string text );

private Q_SLOTS:

  // HANDLE_CLICK:
  // Handler for when table cells are clicked.
  void handle_click( const QModelIndex & index );

  // SAVE_ACTIVE_NOTE:
  // Save cached active note to state vector.  Avoids saving to state vector and triggering
  // updates on every keystroke.
  //void save_cached_active_name();

private:

  void update_visibility();

  std::shared_ptr<Project> project_;

  int visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
  std::string button_text_ = "set";

};
}
