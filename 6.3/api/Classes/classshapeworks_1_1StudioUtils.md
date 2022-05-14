---
title: shapeworks::StudioUtils
summary: Utilities for Studio. 

---

# shapeworks::StudioUtils



Utilities for Studio. 


`#include <StudioUtils.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[ask_multiple_domains_as_single](../Classes/classshapeworks_1_1StudioUtils.md#function-ask-multiple-domains-as-single)**(QWidget * parent, std::shared_ptr< [Project](../Classes/classshapeworks_1_1Project.md) > project) |
| QStringList | **[to_string_list](../Classes/classshapeworks_1_1StudioUtils.md#function-to-string-list)**(std::vector< std::string > vector)<br>convert a vector of std::string to QStringList  |
| QImage | **[vtk_image_to_qimage](../Classes/classshapeworks_1_1StudioUtils.md#function-vtk-image-to-qimage)**(vtkSmartPointer< vtkImageData > image_data)<br>convert vtkImageData to QImage  |

## Public Functions Documentation

### function ask_multiple_domains_as_single

```cpp
static bool ask_multiple_domains_as_single(
    QWidget * parent,
    std::shared_ptr< Project > project
)
```


### function to_string_list

```cpp
static QStringList to_string_list(
    std::vector< std::string > vector
)
```

convert a vector of std::string to QStringList 

### function vtk_image_to_qimage

```cpp
static QImage vtk_image_to_qimage(
    vtkSmartPointer< vtkImageData > image_data
)
```

convert vtkImageData to QImage 

-------------------------------

Updated on 2022-05-14 at 00:09:09 +0000