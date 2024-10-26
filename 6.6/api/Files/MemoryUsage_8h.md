---
title: Libs/Optimize/Utils/MemoryUsage.h

---

# Libs/Optimize/Utils/MemoryUsage.h



## Functions

|                | Name           |
| -------------- | -------------- |
| void | **[process_mem_usage](../Files/MemoryUsage_8h.md#function-process-mem-usage)**(double & vm_usage, double & resident_set) |


## Functions Documentation

### function process_mem_usage

```cpp
void process_mem_usage(
    double & vm_usage,
    double & resident_set
)
```




## Source code

```cpp
#pragma once

//TODO: Figure out memory logging in Windows or get rid of this
// Disabled by default because this code is *nix specific.
// #define LOG_MEMORY_USAGE

void process_mem_usage(double& vm_usage, double& resident_set);
```


-------------------------------

Updated on 2024-10-26 at 19:00:31 +0000
