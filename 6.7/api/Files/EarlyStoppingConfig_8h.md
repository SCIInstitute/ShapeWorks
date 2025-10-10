---
title: Libs/Optimize/EarlyStoppingConfig.h

---

# Libs/Optimize/EarlyStoppingConfig.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[shapeworks::EarlyStoppingConfig](../Classes/structshapeworks_1_1EarlyStoppingConfig.md)** <br>Configuration for early stopping criteria.  |




## Source code

```cpp
#pragma once

namespace shapeworks {

enum class EarlyStoppingStrategy {
  RelativeDifference,
  ExponentialMovingAverage
};

struct EarlyStoppingConfig {
  bool enabled = false;

  int frequency = 100;

  int window_size = 5;

  double threshold = 0.0001;

  EarlyStoppingStrategy strategy = EarlyStoppingStrategy::RelativeDifference;

  double ema_alpha = 0.2;

  bool enable_logging = false;

  std::string logger_name = "early_stopping_log_stats";

  int warmup_iters = 1000;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-10 at 18:16:01 +0000
