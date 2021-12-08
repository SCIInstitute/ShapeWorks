// Qt includes
#include <Data/LandmarkTableModel.h>
#include <Project.h>

#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QDebug>
#include <QIcon>
#include <iostream>
#include <sstream>

namespace shapeworks {

//---------------------------------------------------------------------------
LandmarkTableModel::LandmarkTableModel(QObject* parent)
    : QAbstractTableModel(parent) {
  this->default_colors_.push_back(QColor(255, 175, 78).name().toStdString());   // orange
  this->default_colors_.push_back(QColor(116, 255, 122).name().toStdString());  // green
  this->default_colors_.push_back(QColor(143, 214, 255).name().toStdString());  // light blue
  this->default_colors_.push_back(QColor(255, 0, 0).name().toStdString());      // red
  this->default_colors_.push_back(QColor(255, 233, 0).name().toStdString());    // yellow
  this->default_colors_.push_back(QColor(108, 0, 212).name().toStdString());    // grape
  this->default_colors_.push_back(QColor(0, 0, 255).name().toStdString());      // blue
  this->default_colors_.push_back(QColor(255, 94, 122).name().toStdString());   // mauve
  this->default_colors_.push_back(QColor(255, 255, 165).name().toStdString());  // light yellow
  this->default_colors_.push_back(QColor(255, 0, 255).name().toStdString());    // magenta
  this->default_colors_.push_back(QColor(194, 118, 0).name().toStdString());    // brown
  this->default_colors_.push_back(QColor(159, 143, 255).name().toStdString());  // light purple

  visible_ = QIcon(QString::fromUtf8(":/Studio/Images/Visible.png"));

  visible_off_ = QIcon(QString::fromUtf8(":/Studio/Images/VisibleOff.png"));
}

//---------------------------------------------------------------------------
LandmarkTableModel::~LandmarkTableModel() {
  // Landmarks vector gets destroyed automatically
}

//---------------------------------------------------------------------------
void LandmarkTableModel::set_project(std::shared_ptr<Project> project) {
  // beginResetModel();
  this->project_ = project;
  this->landmarks_ = project->get_landmarks();
  // endResetModel();
  this->update_cells();
}

//---------------------------------------------------------------------------
int LandmarkTableModel::rowCount(const QModelIndex& /*index*/) const {
  // std::cerr << "returning row count: " << landmarks_.size() << "\n";
  return landmarks_.size();
}

//---------------------------------------------------------------------------
int LandmarkTableModel::columnCount(const QModelIndex& /*index*/) const {
  return LandmarkColumns::END_E;
}

//---------------------------------------------------------------------------
QVariant LandmarkTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft | Qt::AlignVCenter);
  } else if (role == Qt::DecorationRole) {
    if (index.column() == LandmarkColumns::VISIBLE_E) {
      if (index.row() < static_cast<int>(landmarks_.size())) {
        if (landmarks_[index.row()].visible_) {
          return this->visible_;
        } else {
          return this->visible_off_;
        }
      }
    } else if (index.column() == LandmarkColumns::COLOR_E) {
      return QString::fromStdString(landmarks_[index.row()].color_);
    }
  } else if (role == Qt::DisplayRole) {
    int sz = this->rowCount(index);
    if (index.row() < sz) {
      if (index.row() < static_cast<int>(landmarks_.size())) {
        if (index.column() == LandmarkColumns::NAME_E) {
          return QString::fromStdString(landmarks_[index.row()].name_);
        } else if (index.column() == LandmarkColumns::POSITION_E) {
          /// TODO : placeholder
          return "1/100";
        } else if (index.column() == LandmarkColumns::VISIBLE_E) {
          return QVariant();
        } else if (index.column() == LandmarkColumns::SET_BUTTON_E) {
          return QString("");
        } else if (index.column() == LandmarkColumns::COMMENT_E) {
          return QString::fromStdString(landmarks_[index.row()].comment_);
        } else if (index.column() >= LandmarkColumns::END_E) {
          return "";
        }
      }
    }
  } else if (role == Qt::BackgroundRole) {
    bool setting = false;
    if (setting) {
      // The active index is always selected, but if one column is highlighted
      // it may not be obvious which is the active index, so color it.
      return QBrush(QColor(225, 243, 252));
    }
  }

  return QVariant();
}

//---------------------------------------------------------------------------
bool LandmarkTableModel::setData(const QModelIndex& index,
                                 const QVariant& value, int role) {
  qDebug() << "setData: " << index << ", " << value;
  // Called only after enter is pressed or focus changed
  if (index.isValid() && role == Qt::EditRole) {
    if (index.row() >= static_cast<int>(landmarks_.size())) {
      return false;
    }
    if (index.column() == LandmarkColumns::VISIBLE_E) {
      landmarks_[index.row()].visible_ = value.toBool();
      update_visibility();
      return false;
    }
    if (index.column() == LandmarkColumns::COLOR_E) {
      QColor color = value.value<QColor>();
      landmarks_[index.row()].color_ = color.name().toStdString();
    } else if (index.column() == LandmarkColumns::NAME_E) {
      landmarks_[index.row()].name_ = value.toString().toStdString();
    } else if (index.column() == LandmarkColumns::COMMENT_E) {
      landmarks_[index.row()].comment_ = value.toString().toStdString();
    } else if (index.column() == LandmarkColumns::POSITION_E) {
      return false;
    }
    Q_EMIT dataChanged(index, index);
  }
  return false;
}

//---------------------------------------------------------------------------
QVariant LandmarkTableModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const {
  if (role == Qt::DecorationRole) {
    if (orientation == Qt::Horizontal) {
      if (section == LandmarkColumns::VISIBLE_E) {
        if (this->visibility_ == LandmarkVisibility::ALL_VISIBLE_E) {
          return QIcon(":/Studio/Images/Visible.png");
        } else if (this->visibility_ == LandmarkVisibility::NONE_VISIBLE_E) {
          return QIcon(QString::fromUtf8(":/Studio/Images/VisibleOff.png"));
        } else if (this->visibility_ == LandmarkVisibility::SOME_VISIBLE_E) {
          return QIcon(QString::fromUtf8(":/Studio/Images/VisibleGray.png"));
        }
      } else if (section == LandmarkColumns::COLOR_E) {
        return "...";
        return QIcon(QString::fromUtf8(":/Studio/Images/Brush.png"));
      }
    }
  } else if (role == Qt::DisplayRole) {
    if (orientation == Qt::Vertical) {
      if (section < this->rowCount(QModelIndex())) {
        return section;
      }
    } else if (orientation == Qt::Horizontal) {
      if (section == LandmarkColumns::NAME_E) {
        return QString("Name");
      } else if (section == LandmarkColumns::POSITION_E) {
        return QString("# Set");
      } else if (section == LandmarkColumns::COMMENT_E) {
        return QString("Comment");
      } else if (section == LandmarkColumns::SET_BUTTON_E) {
        return QString::fromStdString(button_text_);
      }
    }
  } else if (role == Qt::ToolTipRole) {
    // ... any tooltips?
  }

  return QVariant();
}

//---------------------------------------------------------------------------
Qt::ItemFlags LandmarkTableModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if (index.column() == LandmarkColumns::NAME_E ||
      index.column() == LandmarkColumns::COMMENT_E ||
      index.column() == LandmarkColumns::SET_BUTTON_E) {
    flags |= Qt::ItemIsEditable;
  }

  if (index.column() == LandmarkColumns::VISIBLE_E) {
    flags &= ~Qt::ItemIsSelectable;
  }

  return flags;
}

//---------------------------------------------------------------------------
bool LandmarkTableModel::removeRows(int row, int count,
                                    const QModelIndex& /* parent */) {

  if (row < 0 || row + count > this->rowCount(QModelIndex())) {
    return false;
  }

  // Get all the measurements to be removed first, before indices change
  //  const std::vector< Core::Annotation >& annotations =
  //  this->private_->landmark_manager_->annotations_state_->get();
  // std::vector< Core::Annotation > remove_annotations;
  //  for (int row_index = row; row_index < row + count; row_index++) {
  //    if (row_index < static_cast< int >(annotations.size())) {
  //      remove_annotations.push_back(annotations[ row_index ]);
  //    }
  //  }

  // Remove rows from measurement list
  //  for (size_t i = 0; i < remove_annotations.size(); i++) {
  // Remove is done on application thread -- no way to know if it succeeds
  //    Core::ActionRemove::Dispatch(Core::Interface::GetWidgetActionContext(),
  //                                 this->private_->landmark_manager_->annotations_state_,
  //                                 remove_annotations[ i ]);
  // }

  return true;
}

//---------------------------------------------------------------------------
void LandmarkTableModel::update_table() {
  this->update_visibility();

  // QAbstractTableModel::modelReset();
  // QAbstractTableModel::reset();

  // Column resize doesn't work properly without this call
  if (this->project_) {
    this->landmarks_ = project_->get_landmarks();
  }
  this->update_cells();
}

//---------------------------------------------------------------------------
void LandmarkTableModel::update_cells() {
  this->update_visibility();

  int rows = this->rowCount(QModelIndex());
  int columns = this->columnCount(QModelIndex());
  QModelIndex top_left = this->index(0, 0);
  QModelIndex bottom_right = this->index(rows - 1, columns - 1);
  Q_EMIT dataChanged(top_left, bottom_right);
  Q_EMIT headerDataChanged(Qt::Horizontal, 0, 0);
}

//---------------------------------------------------------------------------
void LandmarkTableModel::remove_rows(const std::vector<int>& rows) {
  // Copy the vector (it should be small)
  std::vector<int> sorted_rows = rows;

  // Sort rows in ascending order
  std::sort(sorted_rows.begin(), sorted_rows.end());

  // Must delete in reverse since row numbers are adjusted during removal
  std::vector<int>::reverse_iterator reverse_iter;
  for (reverse_iter = sorted_rows.rbegin(); reverse_iter != sorted_rows.rend();
       ++reverse_iter) {
    // this->private_->landmark_manager_->handle_delete(*reverse_iter);
    this->removeRow(*reverse_iter);
  }
}

//---------------------------------------------------------------------------
int LandmarkTableModel::get_active_index() const {
  // Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());
  // return this->private_->landmark_manager_->active_landmark_state_->get();
}

//---------------------------------------------------------------------------
void LandmarkTableModel::handle_selected(const QItemSelection& selected) {
  if (selected.isEmpty()) {
    return;
  }
  // this->private_->set_active_index(selected.indexes().back().row());
}

//---------------------------------------------------------------------------
void LandmarkTableModel::toggle_visible() {
  // Run from interface thread
  bool visible = true;
  if (this->visibility_ == LandmarkVisibility::ALL_VISIBLE_E) {
    visible = false;
  } else if (this->visibility_ == LandmarkVisibility::NONE_VISIBLE_E ||
             this->visibility_ == LandmarkVisibility::SOME_VISIBLE_E) {
    visible = true;
  }

  for (auto& landmark : landmarks_) {
    landmark.visible_ = visible;
  }

  update_visibility();
  Q_EMIT dataChanged(index(0, 0), index(0, landmarks_.size()));
}

//---------------------------------------------------------------------------
void LandmarkTableModel::handle_click(const QModelIndex& index) {
  if (!index.isValid()) {
    return;
  }

  // If visible column was clicked, toggle visible
  if (index.column() == LandmarkColumns::VISIBLE_E) {
    if (index.row() < static_cast<int>(landmarks_.size())) {
      // Toggle visible
      bool visible = !landmarks_[index.row()].visible_;
      this->setData(index, visible, Qt::EditRole);
    }
  }
}

//---------------------------------------------------------------------------
void LandmarkTableModel::handle_double_click(const QModelIndex& index) {
  qDebug() << "handle double click: " << index;
  if (!index.isValid()) {
    return;
  }

  // If color column was double clicked, bring up color chooser
  if (index.column() == LandmarkColumns::COLOR_E) {
    if (index.row() < static_cast<int>(landmarks_.size())) {
      QColor color = index.model()->data(index, Qt::DecorationRole).value<QColor>();
      color = QColorDialog::getColor(color, nullptr);
      if (color.isValid()) {
        qDebug() << "set data" << index << " " << color;
        this->setData(index, color, Qt::EditRole);
      }
    }
  }
}

//---------------------------------------------------------------------------
void LandmarkTableModel::handle_header_click(int index) {
  if (index == LandmarkColumns::VISIBLE_E) {
    this->toggle_visible();
  }
}

//---------------------------------------------------------------------------
void LandmarkTableModel::update_visibility() {
  int num_visible = 0;
  for (size_t i = 0; i < landmarks_.size(); i++) {
    if (landmarks_[i].visible_) {
      num_visible++;
    }
  }
  if (num_visible == 0) {
    this->visibility_ = LandmarkVisibility::NONE_VISIBLE_E;
  } else if (num_visible == landmarks_.size()) {
    this->visibility_ = LandmarkVisibility::ALL_VISIBLE_E;
  } else {
    this->visibility_ = LandmarkVisibility::SOME_VISIBLE_E;
  }
  Q_EMIT headerDataChanged(Qt::Horizontal, LandmarkColumns::VISIBLE_E, LandmarkColumns::VISIBLE_E);
}

//---------------------------------------------------------------------------
void LandmarkTableModel::set_placing_landmark(int row) {
  /*
     Core::StateEngine::lock_type lock(Core::StateEngine::GetMutex());

     int current_placing_landmark =
     this->private_->landmark_manager_->landmark_placing_index_state_->get();

     if (current_placing_landmark == row) {
     Core::ActionSet::Dispatch(Core::Interface::GetWidgetActionContext(),
                              private_->landmark_manager_->landmark_placing_index_state_,
     -1);
     }
     else {
     const std::vector< Core::Annotation >& landmarks =
      this->private_->landmark_manager_->annotations_state_->get();

     Core::Annotation landmark = landmarks[row];
     landmark.set_values(std::vector<float>());

     Core::ActionSetAt::Dispatch(Core::Interface::GetWidgetActionContext(),
                                this->private_->landmark_manager_->annotations_state_,
     row, landmark);

     Core::ActionSet::Dispatch(Core::Interface::GetWidgetActionContext(),
                              private_->landmark_manager_->landmark_placing_index_state_,
     row);
     }
   */
}
//---------------------------------------------------------------------------

void LandmarkTableModel::set_button_text(std::string text) {
  button_text_ = text;
}

//---------------------------------------------------------------------------
std::string LandmarkTableModel::get_next_landmark_name() {
  return std::to_string(this->landmarks_.size() + 1);
}

//---------------------------------------------------------------------------
std::string LandmarkTableModel::get_next_landmark_color() {
  int index = this->landmarks_.size() % this->default_colors_.size();

  std::string c = this->default_colors_[index];

  return c;
}
}  // namespace shapeworks
