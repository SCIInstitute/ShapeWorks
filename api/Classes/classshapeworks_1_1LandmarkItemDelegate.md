---
title: shapeworks::LandmarkItemDelegate

---

# shapeworks::LandmarkItemDelegate





Inherits from QStyledItemDelegate

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[button_clicked](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#signal-button-clicked)**(int row) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[LandmarkItemDelegate](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-landmarkitemdelegate)**(QObject * parent =0) |
| void | **[paint](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-paint)**(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override |
| QWidget * | **[createEditor](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-createeditor)**(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override |
| void | **[setEditorData](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-seteditordata)**(QWidget * editor, const QModelIndex & index) const override |
| void | **[setModelData](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-setmodeldata)**(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override |
| QSize | **[sizeHint](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-sizehint)**(const QStyleOptionViewItem & option, const QModelIndex & index) const override |
| void | **[updateEditorGeometry](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-updateeditorgeometry)**(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override |
| void | **[set_button_text](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-set-button-text)**(std::string button_text) |
| void | **[set_model](../Classes/classshapeworks_1_1LandmarkItemDelegate.md#function-set-model)**(std::shared_ptr< [LandmarkTableModel](../Classes/classshapeworks_1_1LandmarkTableModel.md) > model) |

## Public Signals Documentation

### signal button_clicked

```cpp
void button_clicked(
    int row
)
```


## Public Functions Documentation

### function LandmarkItemDelegate

```cpp
LandmarkItemDelegate(
    QObject * parent =0
)
```


### function paint

```cpp
void paint(
    QPainter * painter,
    const QStyleOptionViewItem & option,
    const QModelIndex & index
) const override
```


### function createEditor

```cpp
QWidget * createEditor(
    QWidget * parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index
) const override
```


### function setEditorData

```cpp
void setEditorData(
    QWidget * editor,
    const QModelIndex & index
) const override
```


### function setModelData

```cpp
void setModelData(
    QWidget * editor,
    QAbstractItemModel * model,
    const QModelIndex & index
) const override
```


### function sizeHint

```cpp
QSize sizeHint(
    const QStyleOptionViewItem & option,
    const QModelIndex & index
) const override
```


### function updateEditorGeometry

```cpp
void updateEditorGeometry(
    QWidget * editor,
    const QStyleOptionViewItem & option,
    const QModelIndex & index
) const override
```


### function set_button_text

```cpp
void set_button_text(
    std::string button_text
)
```


### function set_model

```cpp
void set_model(
    std::shared_ptr< LandmarkTableModel > model
)
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000