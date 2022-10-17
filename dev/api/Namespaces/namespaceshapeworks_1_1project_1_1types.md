---
title: shapeworks::project::types

---

# shapeworks::project::types



## Types

|                | Name           |
| -------------- | -------------- |
| using tsl::ordered_map< std::string, std::string > | **[StringMap](../Namespaces/namespaceshapeworks_1_1project_1_1types.md#using-stringmap)**  |
| using std::vector< std::string > | **[StringList](../Namespaces/namespaceshapeworks_1_1project_1_1types.md#using-stringlist)**  |
| using std::vector< StringMap > | **[StringMapList](../Namespaces/namespaceshapeworks_1_1project_1_1types.md#using-stringmaplist)**  |
| using std::map< std::string, StringMap > | **[StringMultiMap](../Namespaces/namespaceshapeworks_1_1project_1_1types.md#using-stringmultimap)**  |

## Types Documentation

### using StringMap

```cpp
using shapeworks::project::types::StringMap = typedef tsl::ordered_map<std::string, std::string>;
```


### using StringList

```cpp
using shapeworks::project::types::StringList = typedef std::vector<std::string>;
```


### using StringMapList

```cpp
using shapeworks::project::types::StringMapList = typedef std::vector<StringMap>;
```


### using StringMultiMap

```cpp
using shapeworks::project::types::StringMultiMap = typedef std::map<std::string, StringMap>;
```







-------------------------------

Updated on 2022-10-17 at 08:46:38 +0000