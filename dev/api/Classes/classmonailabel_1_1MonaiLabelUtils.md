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
| ImageType::Pointer | **[loadNRRD](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-loadnrrd)**(const std::string & filePath) |
| ImageType::Pointer | **[extractOrganSegmentation](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-extractorgansegmentation)**(ImageType::Pointer inputImage, int label) |
| void | **[saveNRRD](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-savenrrd)**(ImageType::Pointer image, const std::string & outputPath) |
| bool | **[isOrganPresent](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-isorganpresent)**(ImageType::Pointer image) |
| void | **[processSegmentation](../Classes/classmonailabel_1_1MonaiLabelUtils.md#function-processsegmentation)**(const std::string & segmentationPath, const std::map< int, std::string > & organLabels, const std::string & outputDir, const std::string & sampleId, std::vector< std::string > & organSegmentationPaths) |

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


### function loadNRRD

```cpp
static ImageType::Pointer loadNRRD(
    const std::string & filePath
)
```


### function extractOrganSegmentation

```cpp
static ImageType::Pointer extractOrganSegmentation(
    ImageType::Pointer inputImage,
    int label
)
```


### function saveNRRD

```cpp
static void saveNRRD(
    ImageType::Pointer image,
    const std::string & outputPath
)
```


### function isOrganPresent

```cpp
static bool isOrganPresent(
    ImageType::Pointer image
)
```


### function processSegmentation

```cpp
static void processSegmentation(
    const std::string & segmentationPath,
    const std::map< int, std::string > & organLabels,
    const std::string & outputDir,
    const std::string & sampleId,
    std::vector< std::string > & organSegmentationPaths
)
```


-------------------------------

Updated on 2025-09-03 at 17:42:42 +0000