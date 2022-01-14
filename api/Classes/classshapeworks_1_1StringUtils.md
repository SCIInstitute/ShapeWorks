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
| std::string | **[getFilename](../Classes/classshapeworks_1_1StringUtils.md#function-getfilename)**(std::string const & pathname)<br>Remove path from filename and return.  |
| std::string | **[getBaseFilenameWithoutExtension](../Classes/classshapeworks_1_1StringUtils.md#function-getbasefilenamewithoutextension)**(std::string const & pathname)<br>Get just the filename without path and without extension.  |
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

Remove path from filename and return. 

### function getBaseFilenameWithoutExtension

```cpp
static std::string getBaseFilenameWithoutExtension(
    std::string const & pathname
)
```

Get just the filename without path and without extension. 

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

Updated on 2022-01-14 at 02:28:13 +0000