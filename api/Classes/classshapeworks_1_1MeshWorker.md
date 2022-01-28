---
title: shapeworks::MeshWorker

---

# shapeworks::MeshWorker





Inherits from QObject, QRunnable

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[result_ready](../Classes/classshapeworks_1_1MeshWorker.md#signal-result-ready)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item, MeshHandle mesh) |
| void | **[finished](../Classes/classshapeworks_1_1MeshWorker.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshWorker](../Classes/classshapeworks_1_1MeshWorker.md#function-meshworker)**([MeshWorkQueue](../Classes/classshapeworks_1_1MeshWorkQueue.md) * queue, std::shared_ptr< [MeshGenerator](../Classes/classshapeworks_1_1MeshGenerator.md) > generator) |
| | **[~MeshWorker](../Classes/classshapeworks_1_1MeshWorker.md#function-~meshworker)**() |
| void | **[run](../Classes/classshapeworks_1_1MeshWorker.md#function-run)**() |

## Public Signals Documentation

### signal result_ready

```cpp
void result_ready(
    const MeshWorkItem & item,
    MeshHandle mesh
)
```


### signal finished

```cpp
void finished()
```


## Public Functions Documentation

### function MeshWorker

```cpp
MeshWorker(
    MeshWorkQueue * queue,
    std::shared_ptr< MeshGenerator > generator
)
```


### function ~MeshWorker

```cpp
~MeshWorker()
```


### function run

```cpp
void run()
```


-------------------------------

Updated on 2022-01-28 at 07:11:43 +0000