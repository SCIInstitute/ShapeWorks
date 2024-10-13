---
title: shapeworks::ProjectReader
summary: Base class for Project readers. 

---

# shapeworks::ProjectReader



Base class for [Project](../Classes/classshapeworks_1_1Project.md) readers.  [More...](#detailed-description)


`#include <ProjectReader.h>`

Inherited by [shapeworks::ExcelProjectReader](../Classes/classshapeworks_1_1ExcelProjectReader.md), [shapeworks::JsonProjectReader](../Classes/classshapeworks_1_1JsonProjectReader.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using project::types::StringMapList | **[StringMapList](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmaplist)**  |
| using project::types::StringMap | **[StringMap](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmap)**  |
| using project::types::StringList | **[StringList](../Classes/classshapeworks_1_1ProjectReader.md#using-stringlist)**  |
| using project::types::StringMultiMap | **[StringMultiMap](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmultimap)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md#function-projectreader)**([Project](../Classes/classshapeworks_1_1Project.md) & project)<br>Constructor.  |
| virtual | **[~ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md#function-~projectreader)**() =default<br>Destructor.  |
| virtual bool | **[read_project](../Classes/classshapeworks_1_1ProjectReader.md#function-read-project)**(std::string filename) =0<br>Read a project from a file.  |
| virtual StringMap | **[get_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-get-parameters)**(std::string name) =0 |
| virtual StringMultiMap | **[get_multi_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-get-multi-parameters)**(std::string name) =0 |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[load_subjects](../Classes/classshapeworks_1_1ProjectReader.md#function-load-subjects)**(StringMapList list)<br>Load subjects from string map list.  |
| void | **[load_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-load-parameters)**() |
| void | **[load_parameter](../Classes/classshapeworks_1_1ProjectReader.md#function-load-parameter)**(std::string name, StringMap map) |
| void | **[load_landmark_definitions](../Classes/classshapeworks_1_1ProjectReader.md#function-load-landmark-definitions)**(StringMapList list) |
| bool | **[contains](../Classes/classshapeworks_1_1ProjectReader.md#function-contains)**(StringMap map, std::string key) |
| StringList | **[get_keys](../Classes/classshapeworks_1_1ProjectReader.md#function-get-keys)**(StringMap map) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [Project](../Classes/classshapeworks_1_1Project.md) & | **[project_](../Classes/classshapeworks_1_1ProjectReader.md#variable-project-)**  |

## Detailed Description

```cpp
class shapeworks::ProjectReader;
```

Base class for [Project](../Classes/classshapeworks_1_1Project.md) readers. 

This class serves as an abstract base class for project readers 

## Public Types Documentation

### using StringMapList

```cpp
using shapeworks::ProjectReader::StringMapList =  project::types::StringMapList;
```


### using StringMap

```cpp
using shapeworks::ProjectReader::StringMap =  project::types::StringMap;
```


### using StringList

```cpp
using shapeworks::ProjectReader::StringList =  project::types::StringList;
```


### using StringMultiMap

```cpp
using shapeworks::ProjectReader::StringMultiMap =  project::types::StringMultiMap;
```


## Public Functions Documentation

### function ProjectReader

```cpp
ProjectReader(
    Project & project
)
```

Constructor. 

### function ~ProjectReader

```cpp
virtual ~ProjectReader() =default
```

Destructor. 

### function read_project

```cpp
virtual bool read_project(
    std::string filename
) =0
```

Read a project from a file. 

**Reimplemented by**: [shapeworks::ExcelProjectReader::read_project](../Classes/classshapeworks_1_1ExcelProjectReader.md#function-read-project), [shapeworks::JsonProjectReader::read_project](../Classes/classshapeworks_1_1JsonProjectReader.md#function-read-project)


### function get_parameters

```cpp
virtual StringMap get_parameters(
    std::string name
) =0
```


**Reimplemented by**: [shapeworks::ExcelProjectReader::get_parameters](../Classes/classshapeworks_1_1ExcelProjectReader.md#function-get-parameters), [shapeworks::JsonProjectReader::get_parameters](../Classes/classshapeworks_1_1JsonProjectReader.md#function-get-parameters)


### function get_multi_parameters

```cpp
virtual StringMultiMap get_multi_parameters(
    std::string name
) =0
```


**Reimplemented by**: [shapeworks::ExcelProjectReader::get_multi_parameters](../Classes/classshapeworks_1_1ExcelProjectReader.md#function-get-multi-parameters), [shapeworks::JsonProjectReader::get_multi_parameters](../Classes/classshapeworks_1_1JsonProjectReader.md#function-get-multi-parameters)


## Protected Functions Documentation

### function load_subjects

```cpp
void load_subjects(
    StringMapList list
)
```

Load subjects from string map list. 

### function load_parameters

```cpp
void load_parameters()
```


### function load_parameter

```cpp
void load_parameter(
    std::string name,
    StringMap map
)
```


### function load_landmark_definitions

```cpp
void load_landmark_definitions(
    StringMapList list
)
```


### function contains

```cpp
bool contains(
    StringMap map,
    std::string key
)
```


### function get_keys

```cpp
static StringList get_keys(
    StringMap map
)
```


## Protected Attributes Documentation

### variable project_

```cpp
Project & project_;
```


-------------------------------

Updated on 2024-10-13 at 06:37:12 +0000