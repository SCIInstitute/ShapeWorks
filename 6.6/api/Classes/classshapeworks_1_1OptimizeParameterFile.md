---
title: shapeworks::OptimizeParameterFile

---

# shapeworks::OptimizeParameterFile



 [More...](#detailed-description)


`#include <OptimizeParameterFile.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[OptimizeParameterFile](../Classes/classshapeworks_1_1OptimizeParameterFile.md#function-optimizeparameterfile)**() |
| bool | **[load_parameter_file](../Classes/classshapeworks_1_1OptimizeParameterFile.md#function-load-parameter-file)**(std::string filename, [Optimize](../Classes/classshapeworks_1_1Optimize.md) * optimize)<br>Load a parameter file and set the values on an [Optimize](../Classes/classshapeworks_1_1Optimize.md) object.  |

## Detailed Description

```cpp
class shapeworks::OptimizeParameterFile;
```


**Par**: Overview of OptimizeParameterFile

Implementation of [Optimize](../Classes/classshapeworks_1_1Optimize.md) library parameter file


The [OptimizeParameterFile](../Classes/classshapeworks_1_1OptimizeParameterFile.md) class reads in a parameter file and sets the appropriate values for an [Optimize](../Classes/classshapeworks_1_1Optimize.md) object. 

## Public Functions Documentation

### function OptimizeParameterFile

```cpp
OptimizeParameterFile()
```


### function load_parameter_file

```cpp
bool load_parameter_file(
    std::string filename,
    Optimize * optimize
)
```

Load a parameter file and set the values on an [Optimize](../Classes/classshapeworks_1_1Optimize.md) object. 

-------------------------------

Updated on 2024-04-10 at 06:25:51 +0000