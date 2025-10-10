---
title: shapeworks::ExcelProjectWriter
summary: Excel file format writer for Project. 

---

# shapeworks::ExcelProjectWriter



Excel file format writer for [Project]().  [More...](#detailed-description)


`#include <ExcelProjectWriter.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[write_project](../Classes/classshapeworks_1_1ExcelProjectWriter.md#function-write-project)**([Project](../Classes/classshapeworks_1_1Project.md) & project, const std::string & filename) |

## Detailed Description

```cpp
class shapeworks::ExcelProjectWriter;
```

Excel file format writer for [Project](). 

This class enables writing of [Project](../Classes/classshapeworks_1_1Project.md) objects to Excel file format 

## Public Functions Documentation

### function write_project

```cpp
static bool write_project(
    Project & project,
    const std::string & filename
)
```


-------------------------------

Updated on 2025-10-10 at 18:15:59 +0000