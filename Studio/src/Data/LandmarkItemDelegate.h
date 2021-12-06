#pragma once

// Boost includes
#include <boost/shared_ptr.hpp>

// Qt includes
#include <QStyledItemDelegate>

namespace shapeworks
{

class LandmarkItemDelegatePrivate;

// Delegate for editing length and note columns in measurement table.  Note that only one delegate
// can be set per view, so this delegate handles all custom columns in the table.
class LandmarkItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  LandmarkItemDelegate(QObject* parent = 0);

  //
  // Inherited functions
  //

  void paint(QPainter* painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;
  void setEditorData(QWidget* editor, const QModelIndex &index) const;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

  void updateEditorGeometry(QWidget* editor,
                            const QStyleOptionViewItem &option, const QModelIndex &index) const;

  void set_button_text(std::string button_text);

public:
Q_SIGNALS:
  void button_clicked(int row);

private Q_SLOTS:
  void set_button_pressed(int row);

private:
  boost::shared_ptr< LandmarkItemDelegatePrivate > private_;
};
} // end namespace Seg3D
