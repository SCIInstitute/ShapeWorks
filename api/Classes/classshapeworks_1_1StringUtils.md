---
title: shapeworks::StringUtils
summary: String utility functions. 

---

# shapeworks::StringUtils



String utility functions. 


`#include <StringUtils.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| std::string | **[removeExtension](../Classes/classshapeworks_1_1StringUtils.md#function-removeextension)**(std::string const & filename) |
| std::string | **[getPath](../Classes/classshapeworks_1_1StringUtils.md#function-getpath)**(std::string const & filename) |
| std::string | **[getFilename](../Classes/classshapeworks_1_1StringUtils.md#function-getfilename)**(std::string const & pathname) |
| std::string | **[toLower](../Classes/classshapeworks_1_1StringUtils.md#function-tolower)**(std::string s) |
| bool | **[hasSuffix](../Classes/classshapeworks_1_1StringUtils.md#function-hassuffix)**(std::string const & filename, std::string const & suffix) |
| std::vector< std::string > | **[getFileNamesFromPaths](../Classes/classshapeworks_1_1StringUtils.md#function-getfilenamesfrompaths)**(const std::vector< std::string > & paths) |
| std::string | **[getFileNameWithoutExtension](../Classes/classshapeworks_1_1StringUtils.md#function-getfilenamewithoutextension)**(std::string path)<br>TODO: is this a duplicate of removeExtension?  |

## Public Functions Documentation

### function removeExtension

```cpp
static std::string removeExtension(
    std::string const & filename
)
```


### function getPath

```cpp
static std::string getPath(
    std::string const & filename
)
```


### function getFilename

```cpp
static std::string getFilename(
    std::string const & pathname
)
```


### function toLower

```cpp
static std::string toLower(
    std::string s
)
```


### function hasSuffix

```cpp
static bool hasSuffix(
    std::string const & filename,
    std::string const & suffix
)
```


### function getFileNamesFromPaths

```cpp
static std::vector< std::string > getFileNamesFromPaths(
    const std::vector< std::string > & paths
)
```


### function getFileNameWithoutExtension

```cpp
static std::string getFileNameWithoutExtension(
    std::string path
)
```

TODO: is this a duplicate of removeExtension? 

-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000