/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
 */

// Qt includes
#include <QColorDialog>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QPainter>

#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QSignalMapper>

#include <Data/LandmarkItemDelegate.h>
// Interface includes
//#include <Corview/Modules/Region/Interface/LandmarkItemDelegate.h>

//#include <Corview/Modules/Region/Interface/LandmarkTableModel.h>

#include <QDebug>


namespace shapeworks
{

class LandmarkItemDelegatePrivate {
public:
  QColor new_color_;
  QSignalMapper mapper_;
  std::string button_text_;
};

LandmarkItemDelegate::LandmarkItemDelegate( QObject* parent /*= 0 */ ) :
  QStyledItemDelegate( parent ),
  private_( new LandmarkItemDelegatePrivate )
{

  connect( &private_->mapper_, SIGNAL( mapped( int ) ), this, SLOT( set_button_pressed( int ) ) );
}

void LandmarkItemDelegate::paint( QPainter* painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index ) const
{
  qDebug() << "item delgate paint called\n";

  if ( index.column() == 1 )
  {
    qDebug() << "value: " << index.model()->data(index, Qt::DisplayRole);
    qDebug() << "decorator value: " << index.model()->data(index, Qt::DecorationRole);

    // Use paint function to set color of cell so that color doesn't change when cell is
    // highlighted (selected).
    QColor measurement_color = index.model()->data( index, Qt::DisplayRole ).value< QColor >();
    painter->fillRect( option.rect, measurement_color );
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
  QStyledItemDelegate::paint( painter, option, index );
}

QWidget* LandmarkItemDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index ) const
{
  /*
  if ( index.column() == LandmarkColumns::POSITION_E )
  {
    QLineEdit* line_edit = new QLineEdit( parent );
    QDoubleValidator* double_validator = new QDoubleValidator( parent );
    line_edit->setValidator( double_validator );
    return line_edit;
  }
  else if ( index.column() == LandmarkColumns::COLOR_E )
  {
    QColor measurement_color = index.model()->data( index, Qt::DecorationRole ).value< QColor >();
    this->private_->new_color_ = QColorDialog::getColor( measurement_color, parent->parentWidget()->parentWidget() );

    // In the case that the user presses cancel instead, we'll abort the edit
    if ( !this->private_->new_color_.isValid() )
    {
      return 0;
    }
    return 0;
  }
  else if ( index.column() == LandmarkColumns::NAME_E )
  {
    QLineEdit* line_edit = new QLineEdit( parent );
    return line_edit;
  }
  else if ( index.column() == LandmarkColumns::SET_BUTTON_E )
  {
    QPushButton* button = new QPushButton( QString::fromStdString( private_->button_text_ ), parent );

    private_->mapper_.setMapping( button, index.row() );
    connect( button, SIGNAL( clicked() ), &private_->mapper_, SLOT( map() ) );

    return button;
    //l_view->setIndexWidget(index, l_button);
  }
  else
  {
    return QStyledItemDelegate::createEditor( parent, option, index );
  }
  */
  return QStyledItemDelegate::createEditor( parent, option, index );
}

void LandmarkItemDelegate::setEditorData( QWidget* editor, const QModelIndex &index ) const
{
  /*
  if ( index.column() == LandmarkColumns::POSITION_E || index.column() == LandmarkColumns::NAME_E )
  {
    QString model_text = index.model()->data( index, Qt::DisplayRole ).toString();
    QLineEdit* line_edit = qobject_cast< QLineEdit* >( editor );
    line_edit->setText( model_text );
  }
  else if ( index.column() == LandmarkColumns::COLOR_E )
  {
    //Here we do nothing since our editor has already finished.
  }
  else
  {
    QStyledItemDelegate::setEditorData( editor, index );
  }
  */
}

void LandmarkItemDelegate::setModelData( QWidget* editor, QAbstractItemModel* model,
                                         const QModelIndex &index ) const
{
  /*
  if ( index.column() == LandmarkColumns::NAME_E || index.column() == LandmarkColumns::POSITION_E )
  {
    QLineEdit* line_edit = qobject_cast< QLineEdit* >( editor );
    QString editor_text = line_edit->text();
    model->setData( index, editor_text );
  }
  else if ( index.column() == LandmarkColumns::COLOR_E )
  {
    // finally, if the user has selected a valid color we set it in the model.
    if ( this->private_->new_color_.isValid() )
    {
      model->setData( index, this->private_->new_color_ );
    }
  }
  else
  {
    QStyledItemDelegate::setModelData( editor, model, index );
  }*/

}

QSize LandmarkItemDelegate::sizeHint( const QStyleOptionViewItem & option,
                                      const QModelIndex & index ) const
{
  QSize size_hint = QStyledItemDelegate::sizeHint( option, index );
  // We don't want a tall cell when the text has line breaks, so set height to 0 and rely
  // on other columns to set reasonable size.
  size_hint.setHeight( 0 );
  return size_hint;
}

void LandmarkItemDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  editor->setGeometry( option.rect );
}

void LandmarkItemDelegate::set_button_pressed( int row )
{
  Q_EMIT button_clicked( row );
}

void LandmarkItemDelegate::set_button_text( std::string button_text )
{
  private_->button_text_ = button_text;
}
} // end namespace Seg3D
