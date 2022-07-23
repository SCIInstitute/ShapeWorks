---
title: shapeworks::ExportUtils
summary: Export utilities for Studio. 

---

# shapeworks::ExportUtils



Export utilities for Studio. 


`#include <ExportUtils.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| QString | **[get_save_filename](../Classes/classshapeworks_1_1ExportUtils.md#function-get-save-filename)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * parent, QString title, QString filetypes, QString default_ext) |
| void | **[export_all_subjects_particle_scalars](../Classes/classshapeworks_1_1ExportUtils.md#function-export-all-subjects-particle-scalars)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * parent, QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session) |
| bool | **[write_scalars](../Classes/classshapeworks_1_1ExportUtils.md#function-write-scalars)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app, vtkSmartPointer< vtkPolyData > poly_data, QString filename) |
| bool | **[write_particle_scalars](../Classes/classshapeworks_1_1ExportUtils.md#function-write-particle-scalars)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app, QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) > shape, QString filename) |

## Public Functions Documentation

### function get_save_filename

```cpp
static QString get_save_filename(
    ShapeWorksStudioApp * parent,
    QString title,
    QString filetypes,
    QString default_ext
)
```


### function export_all_subjects_particle_scalars

```cpp
static void export_all_subjects_particle_scalars(
    ShapeWorksStudioApp * parent,
    QSharedPointer< Session > session
)
```


### function write_scalars

```cpp
static bool write_scalars(
    ShapeWorksStudioApp * app,
    vtkSmartPointer< vtkPolyData > poly_data,
    QString filename
)
```


### function write_particle_scalars

```cpp
static bool write_particle_scalars(
    ShapeWorksStudioApp * app,
    QSharedPointer< Shape > shape,
    QString filename
)
```


-------------------------------

Updated on 2022-07-23 at 17:50:03 -0600