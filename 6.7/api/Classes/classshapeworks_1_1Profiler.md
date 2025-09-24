---
title: shapeworks::Profiler

---

# shapeworks::Profiler





## Public Functions

|                | Name           |
| -------------- | -------------- |
| [Profiler](../Classes/classshapeworks_1_1Profiler.md) & | **[instance](../Classes/classshapeworks_1_1Profiler.md#function-instance)**() |
| void | **[start_timer](../Classes/classshapeworks_1_1Profiler.md#function-start-timer)**(const QString & name) |
| void | **[stop_timer](../Classes/classshapeworks_1_1Profiler.md#function-stop-timer)**(const QString & name) |
| void | **[finalize](../Classes/classshapeworks_1_1Profiler.md#function-finalize)**() |
| bool | **[is_profiling_enabled](../Classes/classshapeworks_1_1Profiler.md#function-is-profiling-enabled)**() const |
| bool | **[is_tracing_enabled](../Classes/classshapeworks_1_1Profiler.md#function-is-tracing-enabled)**() const |

## Public Functions Documentation

### function instance

```cpp
static Profiler & instance()
```


### function start_timer

```cpp
void start_timer(
    const QString & name
)
```


### function stop_timer

```cpp
void stop_timer(
    const QString & name
)
```


### function finalize

```cpp
void finalize()
```


### function is_profiling_enabled

```cpp
inline bool is_profiling_enabled() const
```


### function is_tracing_enabled

```cpp
inline bool is_tracing_enabled() const
```


-------------------------------

Updated on 2025-09-24 at 22:11:18 +0000