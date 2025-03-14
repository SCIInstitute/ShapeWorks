---
title: monailabel::MonaiLabelUtils

---

# monailabel::MonaiLabelUtils





## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[createDir](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-createdir)**(const QString & dirPath) |
| bool | **[copySegmentation](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-copysegmentation)**(const QString & sourcePath, const QString & destinationPath) |
| bool | **[deleteTempFile](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-deletetempfile)**(const QString & filePath) |
| std::string | **[getFeatureName](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-getfeaturename)**(QSharedPointer< [shapeworks::Session](../Classes/classshapeworks_1_1Session.md) > session) |

## Public Functions Documentation

### function createDir

```cpp
static bool createDir(
    const QString & dirPath
)
```


### function copySegmentation

```cpp
static bool copySegmentation(
    const QString & sourcePath,
    const QString & destinationPath
)
```


### function deleteTempFile

```cpp
static bool deleteTempFile(
    const QString & filePath
)
```


### function getFeatureName

```cpp
static std::string getFeatureName(
    QSharedPointer< shapeworks::Session > session
)
```


-------------------------------

Updated on 2025-03-14 at 04:44:47 +0000