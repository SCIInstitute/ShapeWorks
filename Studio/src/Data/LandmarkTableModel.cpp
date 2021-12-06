// Qt includes
#include <QBrush>
#include <QColor>
#include <QIcon>

#include <sstream>

#include <Data/LandmarkTableModel.h>
#include <Project.h>

namespace shapeworks
{

LandmarkTableModel::LandmarkTableModel(QObject* parent) :
  QAbstractTableModel(parent)
{}

LandmarkTableModel::~LandmarkTableModel()
{
  // Landmarks vector gets destroyed automatically
}

void LandmarkTableModel::set_project(std::shared_ptr<Project> project)
{
  this->project_ = project;
}

int LandmarkTableModel::rowCount(const QModelIndex& /*index*/) const
{
  return 10;
}

int LandmarkTableModel::columnCount(const QModelIndex& /*index*/) const
{
  //return LandMarkColumns + LandmarkColumns::END_E;
}

QVariant LandmarkTableModel::data(const QModelIndex& index, int role) const
{

  if (!index.isValid()) { return QVariant(); }

  /*
     Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());
     const std::vector< Core::Annotation >& measurements =
     this->private_->landmark_manager_->annotations_state_->get();

     if (role == Qt::TextAlignmentRole) {
     return int( Qt::AlignLeft | Qt::AlignVCenter );
     }
     else if (role == Qt::DecorationRole) {
     if (index.column() == LandmarkColumns::VISIBLE_E) {
      if (index.row() < static_cast< int >(measurements.size())) {
        if (measurements[ index.row() ].get_visible()) {
          return QIcon(QString::fromUtf8(":/Images/Visible.png"));
        }
        else {
          return QIcon(QString::fromUtf8(":/Images/VisibleOff.png"));
        }
      }
     }
     else if (index.column() == LandmarkColumns::COLOR_E) {
      Core::Color color;
      measurements[ index.row() ].get_color(color);
      return QColor(color.r() * 255, color.g() * 255, color.b() * 255);
     }
     }
     else if (role == Qt::DisplayRole) {

     int sz = this->rowCount(index);
     if (index.row() < sz) {
      if (index.row() < static_cast< int >(measurements.size())) {
        if (index.column() == LandmarkColumns::NAME_E) {
          if (index.row() == this->private_->landmark_manager_->active_landmark_state_->get()
              && this->private_->use_cached_active_name_) {
            return QString::fromStdString(this->private_->cached_active_name_);
          }
          else {
            return QString::fromStdString(measurements[ index.row() ].get_name());
          }
        }
        else if (index.column() == LandmarkColumns::POSITION_E) {

          std::vector<float> values = measurements[index.row()].get_values();

          if (index.row() == private_->landmark_manager_->landmark_placing_index_state_->get()) {
            return "placing...";
          }

          if (values.empty()) {
            return "not placed";
          }

          std::ostringstream m;
          m << values[0] << " " << values[1] << " " << values[2];

          return QString::fromStdString(m.str());
        }
        else if (index.column() == LandmarkColumns::SET_BUTTON_E) {   // Used only for export
          return QString("");
        }
        else if (index.column() == LandmarkColumns::COMMENT_E) {                         // Used only for export
          return QString::fromStdString(measurements[ index.row() ].get_comment());
        }
        else if (index.column() >= LandmarkColumns::END_E) {
          if (index.row() == private_->landmark_manager_->landmark_placing_index_state_->get()) {
            return "painting...";
          }
          else {

            std::vector<float> values = measurements[index.row()].get_values();

            if (values.empty()) {
              return "-";
            }
            else {

              int column_index = index.column() - LandmarkColumns::END_E;

              if (column_index >= values.size()) {
                return "NA";
              }

              double value = values[column_index];
              RegionTableColumn region_column = RegionTableColumns::Instance()->get_columns()[column_index];

              return QString::fromStdString(region_column.convert_value_to_string(value));
            }
          }
        }
      }
     }
     }
     else if (role == Qt::BackgroundRole) {
     if (index.row() == this->private_->landmark_manager_->active_landmark_state_->get()) {
      // The active index is always selected, but if one column is highlighted it may not be
      // obvious which is the active index, so color it.
      return QBrush(QColor(225, 243, 252));                           // Light blue
     }
     }
   */
  return QVariant();
}

bool LandmarkTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Called only after enter is pressed or focus changed
  if (index.isValid() && role == Qt::EditRole) {
    /*
       const std::vector< Core::Annotation >& measurements =
       this->private_->landmark_manager_->annotations_state_->get();

       if (index.row() < static_cast< int >(measurements.size())) {
       if (index.column() == LandmarkColumns::VISIBLE_E) {
        Core::Annotation m = measurements[ index.row() ];
        m.set_visible(value.toBool());
        Core::ActionSetAt::Dispatch(Core::Interface::GetWidgetActionContext(),
                                    this->private_->landmark_manager_->annotations_state_, index.row(), m);
       }
       if (index.column() == LandmarkColumns::COLOR_E) {
        Core::Annotation m = measurements[ index.row() ];
        QColor color = value.value< QColor >();
        Core::Color m_color(color.red() / 255.0f, color.green() / 255.0f,
                            color.blue() / 255.0f);
        m.set_color(m_color);
        Core::ActionSetAt::Dispatch(Core::Interface::GetWidgetActionContext(),
                                    this->private_->landmark_manager_->annotations_state_, index.row(), m);
       }
       else if (index.column() == LandmarkColumns::NAME_E) {
        // Don't save name to state variable yet because we don't want to trigger a full update
        // for every character typed.  A full update interrupts editing so we're never
        // able to type more than one character before losing edit focus.  Instead, wait until
        // the name editing is finished to save the state.  In the meantime save and use a
        // cached copy of the name.
        this->private_->cached_active_name_ = value.toString().toStdString();
        this->private_->use_cached_active_name_ = true;
       }
       else if (index.column() == LandmarkColumns::POSITION_E) {
        return false;
       }
       else {
        return false;
       }
     */
    return true;
  }
  return false;
}

QVariant LandmarkTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DecorationRole) {
    if (orientation == Qt::Horizontal) {
      if (section == LandmarkColumns::VISIBLE_E) {
        if (this->visibility_ == LandmarkVisibility::ALL_VISIBLE_E) {
          return QIcon(QString::fromUtf8(":/Images/Visible.png"));
        }
        else if (this->visibility_ == LandmarkVisibility::NONE_VISIBLE_E) {
          return QIcon(QString::fromUtf8(":/Images/VisibleOff.png"));
        }
        else if (this->visibility_ == LandmarkVisibility::SOME_VISIBLE_E) {
          return QIcon(QString::fromUtf8(":/Images/VisibleGray.png"));
        }
      }
      else if (section == LandmarkColumns::COLOR_E) {
        return QIcon(QString::fromUtf8(":/Images/AppearanceOff.png"));
      }
    }
  }
  else if (role == Qt::DisplayRole) {
    if (orientation == Qt::Vertical) {
      if (section < this->rowCount(QModelIndex())) {
        return section;
      }
    }
    else if (orientation == Qt::Horizontal) {
      if (section == LandmarkColumns::NAME_E) {
        return QString("Name");
      }
      else if (section == LandmarkColumns::POSITION_E) {
        return QString("Position");
      }
      else if (section == LandmarkColumns::COMMENT_E) {
        return QString("Comment");
      }
      else if (section == LandmarkColumns::SET_BUTTON_E) {
        return QString::fromStdString(button_text_);
      }
      else if (section >= LandmarkColumns::END_E) {
        int region_column = section - 6;
        return "uh oh?";
      }
    }
  }
  else if (role == Qt::ToolTipRole) {
    if (orientation == Qt::Horizontal) {

      if (section >= LandmarkColumns::END_E) {
        int region_column = section - LandmarkColumns::END_E;
        return "uh oh2?";
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags LandmarkTableModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  //if ( index.column() == LandmarkColumns::COLOR_E ||  // TODO: Fix color dialog position on Windows
  //	index.column() == LandmarkColumns::NAME_E ||
  //	index.column() == LandmarkColumns::LENGTH_E )
  //{
  //   flags |= Qt::ItemIsEditable;
  //}

  if (index.column() == LandmarkColumns::NAME_E) {
    flags |= Qt::ItemIsEditable;
  }

  return flags;
}

bool LandmarkTableModel::removeRows(int row, int count, const QModelIndex & /* parent */)
{
  // Lock StateEngine here so that measurements vector doesn't change after rowCount check
  if (row < 0 || row + count > this->rowCount(QModelIndex())) { return false; }

  // Get all the measurements to be removed first, before indices change
//  const std::vector< Core::Annotation >& annotations =
//  this->private_->landmark_manager_->annotations_state_->get();
//std::vector< Core::Annotation > remove_annotations;
//  for (int row_index = row; row_index < row + count; row_index++) {
//    if (row_index < static_cast< int >(annotations.size())) {
//      remove_annotations.push_back(annotations[ row_index ]);
//    }
//  }

  // Remove rows from measurement list
//  for (size_t i = 0; i < remove_annotations.size(); i++) {
  // Remove is done on application thread -- no way to know if it succeeds
//    Core::ActionRemove::Dispatch(Core::Interface::GetWidgetActionContext(),
//                                 this->private_->landmark_manager_->annotations_state_, remove_annotations[ i ]);
// }

  return true;
}

void LandmarkTableModel::update_table()
{
  this->update_visibility();

  //QAbstractTableModel::modelReset();
  //QAbstractTableModel::reset();

  // Column resize doesn't work properly without this call
  this->update_cells();
}

void LandmarkTableModel::update_cells()
{
  this->update_visibility();
  //this->private_->use_cached_active_name_ = false;

  int rows = this->rowCount(QModelIndex());
  int columns = this->columnCount(QModelIndex());
  QModelIndex top_left = this->index(0, 0);
  QModelIndex bottom_right = this->index(rows - 1, columns - 1);
  Q_EMIT dataChanged(top_left, bottom_right);
  Q_EMIT headerDataChanged(Qt::Horizontal, 0, 0);
}

void LandmarkTableModel::remove_rows(const std::vector< int >& rows)
{
  // Copy the vector (it should be small)
  std::vector< int > sorted_rows = rows;

  // Sort rows in ascending order
  std::sort(sorted_rows.begin(), sorted_rows.end());

  // Must delete in reverse since row numbers are adjusted during removal
  std::vector< int >::reverse_iterator reverse_iter;
  for (reverse_iter = sorted_rows.rbegin(); reverse_iter != sorted_rows.rend(); ++reverse_iter) {
    //this->private_->landmark_manager_->handle_delete(*reverse_iter);
    this->removeRow(*reverse_iter);
  }
}

int LandmarkTableModel::get_active_index() const
{
  //Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());
  //return this->private_->landmark_manager_->active_landmark_state_->get();
}

void LandmarkTableModel::handle_selected(const QItemSelection & selected)
{
  if (selected.isEmpty()) {return; }
  //this->private_->set_active_index(selected.indexes().back().row());
}

void LandmarkTableModel::toggle_visible()
{
  // Run from interface thread
  bool visible = true;
  if (this->visibility_ == LandmarkVisibility::ALL_VISIBLE_E) {
    visible = false;
  }
  else if (this->visibility_ == LandmarkVisibility::NONE_VISIBLE_E ||
           this->visibility_ == LandmarkVisibility::SOME_VISIBLE_E) {
    visible = true;
  }

/*
   const std::vector< Core::Annotation >& measurements =
    this->private_->landmark_manager_->annotations_state_->get();
   for (size_t i = 0; i < measurements.size(); i++) {
    Core::Annotation m = measurements[ i ];
    m.set_visible(visible);
    Core::ActionSetAt::Dispatch(Core::Interface::GetWidgetActionContext(),
                                this->private_->landmark_manager_->annotations_state_, i, m);
   }
 */
}

void LandmarkTableModel::handle_click(const QModelIndex & index)
{
  if (!index.isValid()) { return; }

  // If visible column was clicked, toggle visible
  if (index.column() == LandmarkColumns::VISIBLE_E) {
    //Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());
    //const std::vector< Core::Annotation >& measurements =
//      this->private_->landmark_manager_->annotations_state_->get();
//    if (index.row() < static_cast< int >(measurements.size())) {
    // Toggle visible
//      bool visible = !(measurements[ index.row() ].get_visible());
//      this->setData(index, visible, Qt::EditRole);
  }
}

void LandmarkTableModel::update_visibility()
{
  /*
     ASSERT_IS_INTERFACE_THREAD();

     Core::StateEngine::lock_type lock( Core::StateEngine::GetMutex() );
     const std::vector< Core::Annotation >& measurements =
      this->landmark_manager_->annotations_state_->get();
     size_t num_visible_measurements = 0;
     for ( size_t i = 0; i < measurements.size(); i++ )
     {
      if ( measurements[ i ].get_visible() )
      {
        num_visible_measurements++;
      }
     }
     if ( num_visible_measurements == 0 )
     {
      this->visibility_ = LandmarkVisibility::NONE_VISIBLE_E;
     }
     else if ( num_visible_measurements == measurements.size() )
     {
      this->visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
     }
     else
     {
      this->visibility_ = LandmarkVisibility::SOME_VISIBLE_E;
     }
   */
}

void LandmarkTableModel::set_placing_landmark(int row)
{
  /*
     Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());

     int current_placing_landmark = this->private_->landmark_manager_->landmark_placing_index_state_->get();

     if (current_placing_landmark == row) {
     Core::ActionSet::Dispatch(Core::Interface::GetWidgetActionContext(),
                              private_->landmark_manager_->landmark_placing_index_state_, -1);
     }
     else {
     const std::vector< Core::Annotation >& landmarks =
      this->private_->landmark_manager_->annotations_state_->get();

     Core::Annotation landmark = landmarks[row];
     landmark.set_values(std::vector<float>());

     Core::ActionSetAt::Dispatch(Core::Interface::GetWidgetActionContext(),
                                this->private_->landmark_manager_->annotations_state_, row, landmark);

     Core::ActionSet::Dispatch(Core::Interface::GetWidgetActionContext(),
                              private_->landmark_manager_->landmark_placing_index_state_, row);
     }
   */
}

void LandmarkTableModel::set_button_text(std::string text)
{
  button_text_ = text;
}
}
