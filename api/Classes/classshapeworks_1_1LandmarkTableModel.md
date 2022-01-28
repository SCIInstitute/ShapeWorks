---
title: shapeworks::LandmarkTableModel

---

# shapeworks::LandmarkTableModel





Inherits from QAbstractTableModel

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[new_landmark](../Classes/classshapeworks_1_1LandmarkTableModel.md#slot-new-landmark)**()<br>create a new landmark  |
| void | **[handle_click](../Classes/classshapeworks_1_1LandmarkTableModel.md#slot-handle-click)**(const QModelIndex & index)<br>Handler for when table cells are clicked.  |
| void | **[handle_double_click](../Classes/classshapeworks_1_1LandmarkTableModel.md#slot-handle-double-click)**(const QModelIndex & index)<br>Handler for when table cells are double clicked.  |
| void | **[handle_header_click](../Classes/classshapeworks_1_1LandmarkTableModel.md#slot-handle-header-click)**(int index)<br>Handle header click.  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[LandmarkTableModel](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-landmarktablemodel)**(QObject * parent =0)<br>Constructor.  |
| virtual | **[~LandmarkTableModel](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-~landmarktablemodel)**()<br>Destructor.  |
| void | **[set_session](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>Attach to a session.  |
| void | **[store_landmarks](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-store-landmarks)**()<br>Store landmarks back to project.  |
| void | **[set_active_domain](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-set-active-domain)**(int domain)<br>Set the currently active domain.  |
| void | **[update_table](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-update-table)**()<br>Update the table of landmarks from the project.  |
| void | **[update_cells](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-update-cells)**()<br>Update table cells.  |
| void | **[remove_rows](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-remove-rows)**(const std::vector< int > & rows)<br>Remove landmarks with specified row indices.  |
| void | **[toggle_visible](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-toggle-visible)**()<br>Toggle tri-state button for visibility in horizontal header.  |
| void | **[set_placing_landmark](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-set-placing-landmark)**(int row)<br>Set the currently placing landmark.  |
| void | **[delete_landmarks](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-delete-landmarks)**(const QModelIndexList & list)<br>delete the selected landmarks  |
| int | **[rowCount](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-rowcount)**(const QModelIndex & index) const override<br>QAbstractTableModel::rowCount implementation.  |
| int | **[columnCount](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-columncount)**(const QModelIndex & index) const override<br>QAbstractTableModel::columnCount implementation.  |
| QVariant | **[data](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-data)**(const QModelIndex & index, int role) const override<br>QAbstractTableModel::data implementation.  |
| bool | **[setData](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-setdata)**(const QModelIndex & index, const QVariant & value, int role) override<br>QAbstractTableModel::setData implementation.  |
| QVariant | **[headerData](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-headerdata)**(int section, Qt::Orientation orientation, int role) const override<br>QAbstractTableModel::headerData implementation.  |
| Qt::ItemFlags | **[flags](../Classes/classshapeworks_1_1LandmarkTableModel.md#function-flags)**(const QModelIndex & index) const override<br>QAbstractTableModel::flags implementation.  |

## Public Slots Documentation

### slot new_landmark

```cpp
void new_landmark()
```

create a new landmark 

### slot handle_click

```cpp
void handle_click(
    const QModelIndex & index
)
```

Handler for when table cells are clicked. 

### slot handle_double_click

```cpp
void handle_double_click(
    const QModelIndex & index
)
```

Handler for when table cells are double clicked. 

### slot handle_header_click

```cpp
void handle_header_click(
    int index
)
```

Handle header click. 

## Public Functions Documentation

### function LandmarkTableModel

```cpp
LandmarkTableModel(
    QObject * parent =0
)
```

Constructor. 

### function ~LandmarkTableModel

```cpp
virtual ~LandmarkTableModel()
```

Destructor. 

### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

Attach to a session. 

### function store_landmarks

```cpp
void store_landmarks()
```

Store landmarks back to project. 

### function set_active_domain

```cpp
void set_active_domain(
    int domain
)
```

Set the currently active domain. 

### function update_table

```cpp
void update_table()
```

Update the table of landmarks from the project. 

### function update_cells

```cpp
void update_cells()
```

Update table cells. 

### function remove_rows

```cpp
void remove_rows(
    const std::vector< int > & rows
)
```

Remove landmarks with specified row indices. 

### function toggle_visible

```cpp
void toggle_visible()
```

Toggle tri-state button for visibility in horizontal header. 

### function set_placing_landmark

```cpp
void set_placing_landmark(
    int row
)
```

Set the currently placing landmark. 

### function delete_landmarks

```cpp
void delete_landmarks(
    const QModelIndexList & list
)
```

delete the selected landmarks 

### function rowCount

```cpp
int rowCount(
    const QModelIndex & index
) const override
```

QAbstractTableModel::rowCount implementation. 

### function columnCount

```cpp
int columnCount(
    const QModelIndex & index
) const override
```

QAbstractTableModel::columnCount implementation. 

### function data

```cpp
QVariant data(
    const QModelIndex & index,
    int role
) const override
```

QAbstractTableModel::data implementation. 

### function setData

```cpp
bool setData(
    const QModelIndex & index,
    const QVariant & value,
    int role
) override
```

QAbstractTableModel::setData implementation. 

### function headerData

```cpp
QVariant headerData(
    int section,
    Qt::Orientation orientation,
    int role
) const override
```

QAbstractTableModel::headerData implementation. 

### function flags

```cpp
Qt::ItemFlags flags(
    const QModelIndex & index
) const override
```

QAbstractTableModel::flags implementation. 

-------------------------------

Updated on 2022-01-28 at 21:13:52 +0000