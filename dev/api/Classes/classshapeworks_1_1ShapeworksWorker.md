---
title: shapeworks::ShapeworksWorker

---

# shapeworks::ShapeworksWorker





Inherits from QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[ThreadType](../Classes/classshapeworks_1_1ShapeworksWorker.md#enum-threadtype)** { GroomType, OptimizeType, ReconstructType} |

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[process](../Classes/classshapeworks_1_1ShapeworksWorker.md#slot-process)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[result_ready](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-result-ready)**() |
| void | **[failure](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-failure)**() |
| void | **[finished](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeworksWorker](../Classes/classshapeworks_1_1ShapeworksWorker.md#function-shapeworksworker)**(ThreadType type, QSharedPointer< [Groom](../Classes/classshapeworks_1_1Groom.md) > groom, QSharedPointer< [Optimize](../Classes/classshapeworks_1_1Optimize.md) > optimize, QSharedPointer< [OptimizeParameters](../Classes/classshapeworks_1_1OptimizeParameters.md) > optimize_parameters, QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, double maxAngle =45., float decimationPercent =0.3f, int numClusters =5) |
| | **[~ShapeworksWorker](../Classes/classshapeworks_1_1ShapeworksWorker.md#function-~shapeworksworker)**() |

## Public Types Documentation

### enum ThreadType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| GroomType | |   |
| OptimizeType | |   |
| ReconstructType | |   |




## Public Slots Documentation

### slot process

```cpp
void process()
```


## Public Signals Documentation

### signal result_ready

```cpp
void result_ready()
```


### signal failure

```cpp
void failure()
```


### signal finished

```cpp
void finished()
```


## Public Functions Documentation

### function ShapeworksWorker

```cpp
ShapeworksWorker(
    ThreadType type,
    QSharedPointer< Groom > groom,
    QSharedPointer< Optimize > optimize,
    QSharedPointer< OptimizeParameters > optimize_parameters,
    QSharedPointer< Session > session,
    double maxAngle =45.,
    float decimationPercent =0.3f,
    int numClusters =5
)
```


### function ~ShapeworksWorker

```cpp
~ShapeworksWorker()
```


-------------------------------

Updated on 2024-01-16 at 18:21:10 +0000