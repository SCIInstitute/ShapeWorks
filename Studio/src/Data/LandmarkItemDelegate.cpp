// Qt includes
#include <Data/LandmarkItemDelegate.h>
#include <Data/LandmarkTableModel.h>

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableView>
#include <iostream>

namespace shapeworks {

class LandmarkItemDelegatePrivate {
 public:
  QColor new_color_;
  QSignalMapper mapper_;
  std::string button_text_;
  QPushButton* button_;
  QTableView* table_view_;
  bool is_one_cell_in_edit_mode_ = false;
  QPersistentModelIndex current_edited_cell_index;
  QIcon visible_;
  QIcon visible_off_;
};

//---------------------------------------------------------------------------
LandmarkItemDelegate::LandmarkItemDelegate(QObject* parent /*= 0 */)
    : QStyledItemDelegate(parent), private_(new LandmarkItemDelegatePrivate) {
  private_->button_ = new QPushButton("  set  ");
  private_->button_->hide();
  private_->button_->setCheckable(true);
  private_->button_->setChecked(true);

  connect(&private_->mapper_, SIGNAL(mapped(int)), this, SLOT(set_button_pressed(int)));

  if (QTableView* table_view = qobject_cast<QTableView*>(parent)) {
    private_->table_view_ = table_view;
    private_->table_view_->setMouseTracking(true);
    connect(private_->table_view_, &QTableView::entered, this, &LandmarkItemDelegate::cell_entered);
    private_->is_one_cell_in_edit_mode_ = false;
  }

  private_->visible_ = QIcon(QString::fromUtf8(":/Studio/Images/Visible.png"));
  private_->visible_off_ = QIcon(QString::fromUtf8(":/Studio/Images/VisibleOff.png"));
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const {
  Q_ASSERT(index.isValid());

  if (index.column() == LandmarkColumns::COLOR_E) {
    QColor color = index.model()->data(index, Qt::DecorationRole).value<QColor>();
    painter->fillRect(option.rect, color);
    return;

  } else if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    private_->button_->setGeometry(option.rect);
    if (option.state & QStyle::State_Selected) {
      // painter->fillRect(option.rect, option.palette.highlight());
      // private_->button_->setChecked(true);
    } else {
      // private_->button_->setChecked(false);
    }

    bool checked = index.model()->data(index, Qt::DisplayRole).value<bool>();

    private_->button_->setText(checked ? "placing" : "  set  ");

    // private_->button_->setText(QString::number(index.row()));

    // private_->button_->setChecked(checked);
    // private_->button_->setChecked(true);
    //std::cerr << "paint row " << index.row() << ", checked = " << checked << "\n";
    QPixmap map = private_->button_->grab();
    painter->drawPixmap(option.rect.x(), option.rect.y(), map);
    return;
  }

  QStyledItemDelegate::paint(painter, option, index);
}

//---------------------------------------------------------------------------
QWidget* LandmarkItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const {
  if (index.column() == LandmarkColumns::NAME_E) {
    QLineEdit* line_edit = new QLineEdit(parent);
    return line_edit;
  } else if (index.column() == LandmarkColumns::COMMENT_E) {
    QLineEdit* line_edit = new QLineEdit(parent);
    return line_edit;
  } else if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    bool checked = index.model()->data(index, Qt::DisplayRole).value<bool>();
    QPushButton* button = new QPushButton(checked ? "placing" : "  set  ", parent);
    button->setCheckable(true);
    button->setChecked(checked);
    private_->mapper_.setMapping(button, index.row());
    connect(button, SIGNAL(clicked()), &private_->mapper_, SLOT(map()));
    connect(button, &QPushButton::toggled, [=](bool checked) {
      button->setText(index.model()->data(index, Qt::DisplayRole).value<bool>() ? "placing" : "  set  ");
    });

    return button;
  } else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  if (index.column() == LandmarkColumns::NAME_E) {
    QString model_text = index.model()->data(index, Qt::DisplayRole).toString();
    QLineEdit* line_edit = qobject_cast<QLineEdit*>(editor);
    line_edit->setText(model_text);
  } else if (index.column() == LandmarkColumns::COMMENT_E) {
    QString model_text = index.model()->data(index, Qt::DisplayRole).toString();
    QLineEdit* line_edit = qobject_cast<QLineEdit*>(editor);
    line_edit->setText(model_text);
  } else if (index.column() == LandmarkColumns::COLOR_E) {
    // Here we do nothing since our editor has already finished.
  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  if (index.column() == LandmarkColumns::NAME_E) {
    QLineEdit* line_edit = qobject_cast<QLineEdit*>(editor);
    QString editor_text = line_edit->text();
    model->setData(index, editor_text);
  } else if (index.column() == LandmarkColumns::COLOR_E) {
    // finally, if the user has selected a valid color we set it in the model.
    if (this->private_->new_color_.isValid()) {
      model->setData(index, this->private_->new_color_);
    }
  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}

//---------------------------------------------------------------------------
QSize LandmarkItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QSize size_hint = QStyledItemDelegate::sizeHint(option, index);
  // We don't want a tall cell when the text has line breaks, so set height to 0 and rely
  // on other columns to set reasonable size.
  // size_hint.setHeight(0);
  return size_hint;
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                                const QModelIndex& index) const {
  editor->setGeometry(option.rect);
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::set_button_pressed(int row) {
  Q_EMIT button_clicked(row);
  model_->update_cells();
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::cell_entered(QModelIndex index) {
  if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    if (private_->is_one_cell_in_edit_mode_) {
      private_->table_view_->closePersistentEditor(private_->current_edited_cell_index);
    }
    private_->table_view_->openPersistentEditor(index);
    private_->is_one_cell_in_edit_mode_ = true;
    private_->current_edited_cell_index = index;
  } else {
    if (private_->is_one_cell_in_edit_mode_) {
      private_->is_one_cell_in_edit_mode_ = false;
      private_->table_view_->closePersistentEditor(private_->current_edited_cell_index);
    }
  }
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::set_button_text(std::string button_text) { private_->button_text_ = button_text; }

//---------------------------------------------------------------------------
void LandmarkItemDelegate::set_model(std::shared_ptr<LandmarkTableModel> model) { this->model_ = model; }
}  // namespace shapeworks
