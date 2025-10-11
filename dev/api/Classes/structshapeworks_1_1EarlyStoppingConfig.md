---
title: shapeworks::EarlyStoppingConfig
summary: Configuration for early stopping criteria. 

---

# shapeworks::EarlyStoppingConfig



Configuration for early stopping criteria. 


`#include <EarlyStoppingConfig.h>`

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[enabled](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-enabled)**  |
| int | **[frequency](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-frequency)** <br>how often (in iterations) to check the stopping criterion  |
| int | **[window_size](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-window-size)** <br>number of past values to consider in relative difference or EMA  |
| double | **[threshold](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-threshold)**  |
| [EarlyStoppingStrategy](../Namespaces/namespaceshapeworks.md#enum-earlystoppingstrategy) | **[strategy](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-strategy)** <br>strategy used for determining early stopping.  |
| double | **[ema_alpha](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-ema-alpha)** <br>higher value of alpha give more weight to recent iterations  |
| bool | **[enable_logging](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-enable-logging)**  |
| std::string | **[logger_name](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-logger-name)**  |
| int | **[warmup_iters](../Classes/structshapeworks_1_1EarlyStoppingConfig.md#variable-warmup-iters)** <br>to prevent premature stoppin  |

## Public Attributes Documentation

### variable enabled

```cpp
bool enabled = false;
```


### variable frequency

```cpp
int frequency = 100;
```

how often (in iterations) to check the stopping criterion 

### variable window_size

```cpp
int window_size = 5;
```

number of past values to consider in relative difference or EMA 

### variable threshold

```cpp
double threshold = 0.0001;
```


Threshold for stopping: For RelativeDifference: use ~1e-4 or smaller. For EMA: use ~1e-1 or smaller. 


### variable strategy

```cpp
EarlyStoppingStrategy strategy = EarlyStoppingStrategy::RelativeDifference;
```

strategy used for determining early stopping. 

### variable ema_alpha

```cpp
double ema_alpha = 0.2;
```

higher value of alpha give more weight to recent iterations 

### variable enable_logging

```cpp
bool enable_logging = false;
```


### variable logger_name

```cpp
std::string logger_name = "early_stopping_log_stats";
```


### variable warmup_iters

```cpp
int warmup_iters = 1000;
```

to prevent premature stoppin 

-------------------------------

Updated on 2025-10-11 at 18:16:33 +0000