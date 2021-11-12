---
title: BarGraph

---

# BarGraph





Inherits from QWidget

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum class| **[ChartType](../Classes/classBarGraph.md#enum-charttype)** { ExplainedVariance, Evaluation} |

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[hover_timer_event](../Classes/classBarGraph.md#slot-hover-timer-event)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[BarGraph](../Classes/classBarGraph.md#function-bargraph)**(QWidget * parent =0) |
| | **[~BarGraph](../Classes/classBarGraph.md#function-~bargraph)**() |
| void | **[set_chart_type](../Classes/classBarGraph.md#function-set-chart-type)**(ChartType chart_type) |
| void | **[set_data](../Classes/classBarGraph.md#function-set-data)**(const std::vector< double > & values) |
| void | **[set_data](../Classes/classBarGraph.md#function-set-data)**(const Eigen::VectorXd values) |
| void | **[set_log_scale](../Classes/classBarGraph.md#function-set-log-scale)**(bool b) |
| void | **[set_y_label](../Classes/classBarGraph.md#function-set-y-label)**(QString label) |

## Public Types Documentation

### enum ChartType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| ExplainedVariance | |   |
| Evaluation | |   |




## Public Slots Documentation

### slot hover_timer_event

```cpp
void hover_timer_event()
```


## Public Functions Documentation

### function BarGraph

```cpp
BarGraph(
    QWidget * parent =0
)
```


### function ~BarGraph

```cpp
~BarGraph()
```


### function set_chart_type

```cpp
void set_chart_type(
    ChartType chart_type
)
```


### function set_data

```cpp
void set_data(
    const std::vector< double > & values
)
```


### function set_data

```cpp
void set_data(
    const Eigen::VectorXd values
)
```


### function set_log_scale

```cpp
void set_log_scale(
    bool b
)
```


### function set_y_label

```cpp
void set_y_label(
    QString label
)
```


-------------------------------

Updated on 2021-11-12 at 04:35:44 +0000