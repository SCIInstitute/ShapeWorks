---
title: shapeworks::JsonProjectReader
summary: JSON file format reader for Project. 

---

# shapeworks::JsonProjectReader



JSON file format reader for [Project]().  [More...](#detailed-description)


`#include <JsonProjectReader.h>`

Inherits from [shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[JsonProjectReader](../Classes/classshapeworks_1_1JsonProjectReader.md#function-jsonprojectreader)**([Project](../Classes/classshapeworks_1_1Project.md) & project) |
| | **[~JsonProjectReader](../Classes/classshapeworks_1_1JsonProjectReader.md#function-~jsonprojectreader)**() |
| virtual bool | **[read_project](../Classes/classshapeworks_1_1JsonProjectReader.md#function-read-project)**(std::string filename) override<br>Read a project from a file.  |
| virtual StringMap | **[get_parameters](../Classes/classshapeworks_1_1JsonProjectReader.md#function-get-parameters)**(std::string name) override |
| virtual StringMultiMap | **[get_multi_parameters](../Classes/classshapeworks_1_1JsonProjectReader.md#function-get-multi-parameters)**(std::string name) override |

## Additional inherited members

**Public Types inherited from [shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)**

|                | Name           |
| -------------- | -------------- |
| using project::types::StringMapList | **[StringMapList](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmaplist)**  |
| using project::types::StringMap | **[StringMap](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmap)**  |
| using project::types::StringList | **[StringList](../Classes/classshapeworks_1_1ProjectReader.md#using-stringlist)**  |
| using project::types::StringMultiMap | **[StringMultiMap](../Classes/classshapeworks_1_1ProjectReader.md#using-stringmultimap)**  |

**Public Functions inherited from [shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md#function-projectreader)**([Project](../Classes/classshapeworks_1_1Project.md) & project)<br>Constructor.  |
| virtual | **[~ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md#function-~projectreader)**() =default<br>Destructor.  |

**Protected Functions inherited from [shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[load_subjects](../Classes/classshapeworks_1_1ProjectReader.md#function-load-subjects)**(StringMapList list)<br>Load subjects from string map list.  |
| void | **[load_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-load-parameters)**() |
| void | **[load_parameter](../Classes/classshapeworks_1_1ProjectReader.md#function-load-parameter)**(std::string name, StringMap map) |
| void | **[load_landmark_definitions](../Classes/classshapeworks_1_1ProjectReader.md#function-load-landmark-definitions)**(StringMapList list) |
| bool | **[contains](../Classes/classshapeworks_1_1ProjectReader.md#function-contains)**(StringMap map, std::string key) |
| StringList | **[get_keys](../Classes/classshapeworks_1_1ProjectReader.md#function-get-keys)**(StringMap map) |

**Protected Attributes inherited from [shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)**

|                | Name           |
| -------------- | -------------- |
| [Project](../Classes/classshapeworks_1_1Project.md) & | **[project_](../Classes/classshapeworks_1_1ProjectReader.md#variable-project-)**  |


## Detailed Description

```cpp
class shapeworks::JsonProjectReader;
```

JSON file format reader for [Project](). 

This class enables reader of [Project](../Classes/classshapeworks_1_1Project.md) objects from JSON file format 

## Public Functions Documentation

### function JsonProjectReader

```cpp
JsonProjectReader(
    Project & project
)
```


### function ~JsonProjectReader

```cpp
~JsonProjectReader()
```


### function read_project

```cpp
virtual bool read_project(
    std::string filename
) override
```

Read a project from a file. 

**Reimplements**: [shapeworks::ProjectReader::read_project](../Classes/classshapeworks_1_1ProjectReader.md#function-read-project)


### function get_parameters

```cpp
virtual StringMap get_parameters(
    std::string name
) override
```


**Reimplements**: [shapeworks::ProjectReader::get_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-get-parameters)


### function get_multi_parameters

```cpp
virtual StringMultiMap get_multi_parameters(
    std::string name
) override
```


**Reimplements**: [shapeworks::ProjectReader::get_multi_parameters](../Classes/classshapeworks_1_1ProjectReader.md#function-get-multi-parameters)


-------------------------------

Updated on 2023-07-15 at 03:21:17 +0000