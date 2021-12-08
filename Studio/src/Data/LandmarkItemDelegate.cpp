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
  bool isOneCellInEditMode = false;
  QPersistentModelIndex currentEditedCellIndex;
  QIcon visible_;
  QIcon visible_off_;
};

//---------------------------------------------------------------------------
LandmarkItemDelegate::LandmarkItemDelegate(QObject* parent /*= 0 */) : QStyledItemDelegate(parent),
                                                                       private_(new LandmarkItemDelegatePrivate) {
  private_->button_ = new QPushButton("set");
  private_->button_->hide();
  private_->button_->setCheckable(true);

  connect(&private_->mapper_, SIGNAL(mapped(int)), this, SLOT(set_button_pressed(int)));

  if (QTableView* table_view = qobject_cast<QTableView*>(parent)) {
    private_->table_view_ = table_view;
    //btn = new QPushButton("...", myWidget);
    //btn->hide();
    private_->table_view_->setMouseTracking(true);
    connect(private_->table_view_, &QTableView::entered, this, &LandmarkItemDelegate::cell_entered);
    private_->isOneCellInEditMode = false;
  }

  private_->visible_ = QIcon(QString::fromUtf8(":/Studio/Images/Visible.png"));
  private_->visible_off_ = QIcon(QString::fromUtf8(":/Studio/Images/VisibleOff.png"));
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const {
  Q_ASSERT(index.isValid());


  if (index.column() == LandmarkColumns::COLOR_E) {
    //    qDebug() << "value: " << index.model()->data(index, Qt::DisplayRole);
    //    qDebug() << "decorator value: " << index.model()->data(index, Qt::DecorationRole);

    // Use paint function to set color of cell so that color doesn't change when cell is
    // highlighted (selected).
    QColor measurement_color = index.model()->data(index, Qt::DecorationRole).value<QColor>();
    painter->fillRect(option.rect, measurement_color);
    return;

  } else if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    private_->button_->setGeometry(option.rect);
    //btn->setText("...");
    if (option.state & QStyle::State_Selected) {
      painter->fillRect(option.rect, option.palette.highlight());
      private_->button_->setChecked(true);
    } else {
      private_->button_->setChecked(false);
    }
    QPixmap map = private_->button_->grab();
    painter->drawPixmap(option.rect.x(), option.rect.y(), map);

    return;
  }

  /*
     if ( index.column() == LandmarkColumns::COLOR_E )
     {
     // Use paint function to set color of cell so that color doesn't change when cell is
     // highlighted (selected).
     QColor measurement_color = index.model()->data( index, Qt::DecorationRole ).value< QColor >();
     painter->fillRect( option.rect, measurement_color );
     }
     else if ( index.column() == LandmarkColumns::NAME_E || index.column() == LandmarkColumns::POSITION_E )
     {
     QStyledItemDelegate::paint( painter, option, index );
     return;
     QString text = index.model()->data( index, Qt::DisplayRole ).toString();
     painter->save();
     painter->setBackgroundMode( Qt::OpaqueMode );
     painter->setBackground( index.model()->data( index, Qt::BackgroundRole ).value<QBrush>() );
     painter->restore();
     }
     else
     {
     QStyledItemDelegate::paint( painter, option, index );
     }
   */
  QStyledItemDelegate::paint(painter, option, index);
}

//---------------------------------------------------------------------------
QWidget* LandmarkItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const {
  qDebug() << "create Editor: " << index;

  if (index.column() == LandmarkColumns::POSITION_E) {
    QLineEdit* line_edit = new QLineEdit(parent);
    QDoubleValidator* double_validator = new QDoubleValidator(parent);
    line_edit->setValidator(double_validator);
    return line_edit;
  } else if (index.column() == LandmarkColumns::COLOR_E) {
    std::cerr << "color button\n";
    //QColor measurement_color = index.model()->data(index, Qt::DecorationRole).value<QColor>();
    //this->private_->new_color_ = QColorDialog::getColor(measurement_color, parent->parentWidget()->parentWidget());

    // In the case that the user presses cancel instead, we'll abort the edit
    //if (!this->private_->new_color_.isValid()) {
//      return 0;
//    }
    return 0;
  } else if (index.column() == LandmarkColumns::NAME_E) {
    QLineEdit* line_edit = new QLineEdit(parent);
    return line_edit;
  } else if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    std::cerr << "other button\n";
    QPushButton* button = new QPushButton("set", parent);

    private_->mapper_.setMapping(button, index.row());
    connect(button, SIGNAL(clicked()), &private_->mapper_, SLOT(map()));

    return button;
    //l_view->setIndexWidget(index, l_button);
  } else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  qDebug() << "setEditorData: " << editor << ", index: " << index;
  if (index.column() == LandmarkColumns::NAME_E) {
    QString model_text = index.model()->data(index, Qt::DisplayRole).toString();
    QLineEdit* line_edit = qobject_cast<QLineEdit*>(editor);
    line_edit->setText(model_text);
  } else if (index.column() == LandmarkColumns::COLOR_E) {
    //Here we do nothing since our editor has already finished.
  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                        const QModelIndex& index) const {
  qDebug() << "setModelData: " << editor << ", index: " << index;
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
  //return QSize(25,25);
  QSize size_hint = QStyledItemDelegate::sizeHint(option, index);
  // We don't want a tall cell when the text has line breaks, so set height to 0 and rely
  // on other columns to set reasonable size.
  //size_hint.setHeight(0);
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
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::cell_entered(QModelIndex index) {
  if (index.column() == LandmarkColumns::SET_BUTTON_E) {
    if (private_->isOneCellInEditMode) {
      private_->table_view_->closePersistentEditor(private_->currentEditedCellIndex);
    }
    qDebug() << "Opening persistent editor!\n";
    private_->table_view_->openPersistentEditor(index);
    private_->isOneCellInEditMode = true;
    private_->currentEditedCellIndex = index;
  } else {
    if (private_->isOneCellInEditMode) {
      private_->isOneCellInEditMode = false;
      qDebug() << "Closing persistent editor!\n";
      private_->table_view_->closePersistentEditor(private_->currentEditedCellIndex);
    }
  }
}


//---------------------------------------------------------------------------
void LandmarkItemDelegate::set_button_text(std::string button_text) {
  private_->button_text_ = button_text;
}

//---------------------------------------------------------------------------
void LandmarkItemDelegate::set_model(std::shared_ptr<LandmarkTableModel> model)
{
  this->model_ = model;
}
}  // namespace shapeworks
