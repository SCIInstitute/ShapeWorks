// Qt includes
#include <Data/LandmarkTableModel.h>
#include <Project.h>

#include <Eigen/Core>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QDebug>
#include <QIcon>
#include <iostream>
#include <sstream>

namespace shapeworks {

//---------------------------------------------------------------------------
LandmarkTableModel::LandmarkTableModel(QObject* parent) : QAbstractTableModel(parent) {
  visible_ = QIcon(QString::fromUtf8(":/Studio/Images/Visible.png"));

  visible_off_ = QIcon(QString::fromUtf8(":/Studio/Images/VisibleOff.png"));
}

//---------------------------------------------------------------------------
LandmarkTableModel::~LandmarkTableModel() {
  // Landmarks vector gets destroyed automatically
}

//---------------------------------------------------------------------------
void LandmarkTableModel::set_project(std::shared_ptr<Project> project) {
  project_ = project;
  landmarks_ = project->get_landmarks();
  update_cells();
}

//---------------------------------------------------------------------------
void LandmarkTableModel::set_session(QSharedPointer<Session> session) {
  session_ = session;
  connect(session_.data(), &Session::landmarks_changed, this, &LandmarkTableModel::update_table);
}

//---------------------------------------------------------------------------
void LandmarkTableModel::store_landmarks() { this->project_->set_landmarks(landmarks_); }

//---------------------------------------------------------------------------
void LandmarkTableModel::new_landmark() {
  project_->new_landmark();
  update_table();
}

//---------------------------------------------------------------------------
int LandmarkTableModel::rowCount(const QModelIndex& /*index*/) const { return landmarks_.size(); }

//---------------------------------------------------------------------------
int LandmarkTableModel::columnCount(const QModelIndex& /*index*/) const { return LandmarkColumns::END_E; }

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
bool LandmarkTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  // qDebug() << "setData: " << index << ", " << value;
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
QVariant LandmarkTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
    /// TODO: any tooltips?
  }

  return QVariant();
}

//---------------------------------------------------------------------------
Qt::ItemFlags LandmarkTableModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if (index.column() == LandmarkColumns::NAME_E || index.column() == LandmarkColumns::COMMENT_E ||
      index.column() == LandmarkColumns::SET_BUTTON_E) {
    flags |= Qt::ItemIsEditable;
  }

  if (index.column() == LandmarkColumns::VISIBLE_E) {
    flags &= ~Qt::ItemIsSelectable;
  }

  return flags;
}

//---------------------------------------------------------------------------
void LandmarkTableModel::update_table() {
  this->update_visibility();

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
int LandmarkTableModel::get_active_index() const {}

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
  if (!index.isValid()) {
    return;
  }

  // If color column was double clicked, bring up color chooser
  if (index.column() == LandmarkColumns::COLOR_E) {
    if (index.row() < static_cast<int>(landmarks_.size())) {
      QColor color = index.model()->data(index, Qt::DecorationRole).value<QColor>();
      color = QColorDialog::getColor(color, nullptr);
      if (color.isValid()) {
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
void LandmarkTableModel::set_placing_landmark(int row) {}

//---------------------------------------------------------------------------
void LandmarkTableModel::set_button_text(std::string text) { button_text_ = text; }

//---------------------------------------------------------------------------
void LandmarkTableModel::remove_eigen_row(Eigen::MatrixXd& matrix, unsigned int row_to_remove) {
  if (matrix.rows() == 0) {
    return;
  }
  unsigned int numRows = matrix.rows() - 1;
  unsigned int numCols = matrix.cols();

  if (row_to_remove < numRows) {
    matrix.block(row_to_remove, 0, numRows - row_to_remove, numCols) = matrix.bottomRows(numRows - row_to_remove);
  }
  matrix.conservativeResize(numRows, numCols);
}

//---------------------------------------------------------------------------
void LandmarkTableModel::delete_landmarks(const QModelIndexList& list) {
  std::vector<Landmark> keep;
  std::vector<int> indices_to_keep;

  for (int i = 0; i < landmarks_.size(); i++) {
    int found = false;
    for (int j = 0; j < list.size(); j++) {
      if (list[j].row() == i) {
        found = true;
      }
    }
    if (!found) {
      indices_to_keep.push_back(i);
      keep.push_back(landmarks_[i]);
    }
  }

  auto shapes = session_->get_shapes();

  for (int i = 0; i < list.size(); i++) {
    for (int j = 0; j < shapes.size(); j++) {
      auto& landmarks = shapes[j]->landmarks();
      remove_eigen_row(landmarks, list[i].row());
    }
  }

  landmarks_ = keep;
  store_landmarks();
  session_->trigger_landmarks_changed();
}

}  // namespace shapeworks
