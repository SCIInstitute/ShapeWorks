---
title: shapeworks::JsonProjectWriter
summary: JSON file format writer for Project. 

---

# shapeworks::JsonProjectWriter



JSON file format writer for [Project]().  [More...](#detailed-description)


`#include <JsonProjectWriter.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[write_project](../Classes/classshapeworks_1_1JsonProjectWriter.md#function-write-project)**([Project](../Classes/classshapeworks_1_1Project.md) & project, std::string filename) |

## Detailed Description

```cpp
class shapeworks::JsonProjectWriter;
```

JSON file format writer for [Project](). 

This class enables writing of [Project](../Classes/classshapeworks_1_1Project.md) objects to JSON file format 

## Public Functions Documentation

### function write_project

```cpp
static bool write_project(
    Project & project,
    std::string filename
)
```


-------------------------------

Updated on 2024-03-16 at 21:19:05 +0000