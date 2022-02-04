---
title: Studio/src/Data/LandmarkItemDelegate.h

---

# Studio/src/Data/LandmarkItemDelegate.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LandmarkItemDelegate](../Classes/classshapeworks_1_1LandmarkItemDelegate.md)**  |




## Source code

```cpp
#pragma once

#include <memory>

// Qt includes
#include <QStyledItemDelegate>

namespace shapeworks {

class LandmarkItemDelegatePrivate;
class LandmarkTableModel;

// Delegate for editing length and note columns in measurement table.  Note that only one delegate
// can be set per view, so this delegate handles all custom columns in the table.
class LandmarkItemDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  LandmarkItemDelegate(QObject* parent = 0);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor,
                            const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  void set_button_text(std::string button_text);

  void set_model(std::shared_ptr<LandmarkTableModel> model);

 Q_SIGNALS:
  void button_clicked(int row);

 private Q_SLOTS:
  void set_button_pressed(int row);
  void cell_entered(QModelIndex index);

 private:
  std::shared_ptr<LandmarkItemDelegatePrivate> private_;
  std::shared_ptr<LandmarkTableModel> model_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-02-04 at 06:14:24 +0000
