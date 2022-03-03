---
title: shapeworks::MeshWorkQueue

---

# shapeworks::MeshWorkQueue





## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::list< [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) > | **[WorkList](../Classes/classshapeworks_1_1MeshWorkQueue.md#using-worklist)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshWorkQueue](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-meshworkqueue)**() |
| | **[~MeshWorkQueue](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-~meshworkqueue)**() |
| void | **[push](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-push)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item) |
| [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) * | **[get_next_work_item](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-get-next-work-item)**() |
| bool | **[is_inside](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-is-inside)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item) |
| void | **[remove](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-remove)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item) |
| bool | **[is_empty](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-is-empty)**() |
| int | **[size](../Classes/classshapeworks_1_1MeshWorkQueue.md#function-size)**() |

## Public Types Documentation

### using WorkList

```cpp
using shapeworks::MeshWorkQueue::WorkList =  std::list<MeshWorkItem>;
```


## Public Functions Documentation

### function MeshWorkQueue

```cpp
MeshWorkQueue()
```


### function ~MeshWorkQueue

```cpp
~MeshWorkQueue()
```


### function push

```cpp
void push(
    const MeshWorkItem & item
)
```


### function get_next_work_item

```cpp
MeshWorkItem * get_next_work_item()
```


### function is_inside

```cpp
bool is_inside(
    const MeshWorkItem & item
)
```


### function remove

```cpp
void remove(
    const MeshWorkItem & item
)
```


### function is_empty

```cpp
bool is_empty()
```


### function size

```cpp
int size()
```


-------------------------------

Updated on 2022-03-03 at 07:50:36 +0000