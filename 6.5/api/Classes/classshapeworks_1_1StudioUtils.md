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
| vtkSmartPointer< vtkPolyData > | **[reverse_poly_data](../Classes/classshapeworks_1_1StudioUtils.md#function-reverse-poly-data)**(vtkSmartPointer< vtkPolyData > poly_data)<br>reverse a poly data  |
| QString | **[get_platform_string](../Classes/classshapeworks_1_1StudioUtils.md#function-get-platform-string)**()<br>return platform string  |
| void | **[add_viewport_border](../Classes/classshapeworks_1_1StudioUtils.md#function-add-viewport-border)**(vtkRenderer * renderer, double * color, double line_width =6.0)<br>add a color border to a viewport  |
| bool | **[write_mesh](../Classes/classshapeworks_1_1StudioUtils.md#function-write-mesh)**(vtkSmartPointer< vtkPolyData > poly_data, QString filename)<br>write a mesh to file  |

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

### function reverse_poly_data

```cpp
static vtkSmartPointer< vtkPolyData > reverse_poly_data(
    vtkSmartPointer< vtkPolyData > poly_data
)
```

reverse a poly data 

### function get_platform_string

```cpp
static QString get_platform_string()
```

return platform string 

### function add_viewport_border

```cpp
static void add_viewport_border(
    vtkRenderer * renderer,
    double * color,
    double line_width =6.0
)
```

add a color border to a viewport 

### function write_mesh

```cpp
static bool write_mesh(
    vtkSmartPointer< vtkPolyData > poly_data,
    QString filename
)
```

write a mesh to file 

-------------------------------

Updated on 2024-01-17 at 16:55:15 +0000