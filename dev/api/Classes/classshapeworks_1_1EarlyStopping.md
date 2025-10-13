---
title: shapeworks::EarlyStopping

---

# shapeworks::EarlyStopping





## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1EarlyStopping.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[EarlyStopping](../Classes/classshapeworks_1_1EarlyStopping.md#function-earlystopping)**() |
| void | **[SetConfigParams](../Classes/classshapeworks_1_1EarlyStopping.md#function-setconfigparams)**(int frequency, int window_size, double threshold, [EarlyStoppingStrategy](../Namespaces/namespaceshapeworks.md#enum-earlystoppingstrategy) strategy =[EarlyStoppingStrategy::RelativeDifference](../Namespaces/namespaceshapeworks.md#enumvalue-relativedifference), double ema_alpha =0.2, bool enable_logging =false, const std::string & logger_name ="", int warmup_iters =1000) |
| void | **[reset](../Classes/classshapeworks_1_1EarlyStopping.md#function-reset)**() |
| void | **[update](../Classes/classshapeworks_1_1EarlyStopping.md#function-update)**(int iteration, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| bool | **[ShouldStop](../Classes/classshapeworks_1_1EarlyStopping.md#function-shouldstop)**() const |
| bool | **[SetControlShapes](../Classes/classshapeworks_1_1EarlyStopping.md#function-setcontrolshapes)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| Eigen::MatrixXd | **[GetTestShapes](../Classes/classshapeworks_1_1EarlyStopping.md#function-gettestshapes)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1EarlyStopping.md#variable-vdimension)**  |

## Public Types Documentation

### typedef PointType

```cpp
typedef ParticleSystem::PointType shapeworks::EarlyStopping::PointType;
```


## Public Functions Documentation

### function EarlyStopping

```cpp
EarlyStopping()
```


### function SetConfigParams

```cpp
void SetConfigParams(
    int frequency,
    int window_size,
    double threshold,
    EarlyStoppingStrategy strategy =EarlyStoppingStrategy::RelativeDifference,
    double ema_alpha =0.2,
    bool enable_logging =false,
    const std::string & logger_name ="",
    int warmup_iters =1000
)
```


### function reset

```cpp
void reset()
```


### function update

```cpp
void update(
    int iteration,
    const ParticleSystem * p
)
```


### function ShouldStop

```cpp
bool ShouldStop() const
```


### function SetControlShapes

```cpp
bool SetControlShapes(
    const ParticleSystem * p
)
```


### function GetTestShapes

```cpp
Eigen::MatrixXd GetTestShapes(
    const ParticleSystem * p
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
```


-------------------------------

Updated on 2025-10-13 at 18:47:49 +0000