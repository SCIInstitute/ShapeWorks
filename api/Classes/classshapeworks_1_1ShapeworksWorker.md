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
| void | **[error_message](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-warning-message)**(QString ) |
| void | **[message](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-message)**(QString ) |
| void | **[finished](../Classes/classshapeworks_1_1ShapeworksWorker.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeworksWorker](../Classes/classshapeworks_1_1ShapeworksWorker.md#function-shapeworksworker)**(ThreadType type, QSharedPointer< [QGroom](../Classes/classshapeworks_1_1QGroom.md) > groom, QSharedPointer< [Optimize](../Classes/classshapeworks_1_1Optimize.md) > optimize, QSharedPointer< [OptimizeParameters](../Classes/classshapeworks_1_1OptimizeParameters.md) > optimize_parameters, QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, std::vector< std::vector< itk::Point< double >>> local_pts =std::vector< std::vector< itk::Point< double >>>(), std::vector< std::vector< itk::Point< double >>> global_pts =std::vector< std::vector< itk::Point< double >>>(), std::vector< std::string > distance_transform =std::vector< std::string >(), double maxAngle =45., float decimationPercent =0.3f, int numClusters =5) |
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


### signal error_message

```cpp
void error_message(
    QString 
)
```


### signal warning_message

```cpp
void warning_message(
    QString 
)
```


### signal message

```cpp
void message(
    QString 
)
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
    QSharedPointer< QGroom > groom,
    QSharedPointer< Optimize > optimize,
    QSharedPointer< OptimizeParameters > optimize_parameters,
    QSharedPointer< Session > session,
    std::vector< std::vector< itk::Point< double >>> local_pts =std::vector< std::vector< itk::Point< double >>>(),
    std::vector< std::vector< itk::Point< double >>> global_pts =std::vector< std::vector< itk::Point< double >>>(),
    std::vector< std::string > distance_transform =std::vector< std::string >(),
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

Updated on 2021-12-07 at 17:10:24 +0000