---
title: shapeworks::ExportImageDialog

---

# shapeworks::ExportImageDialog



 [More...](#detailed-description)


`#include <ExportImageDialog.h>`

Inherits from QDialog

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ExportImageDialog](../Classes/classshapeworks_1_1ExportImageDialog.md#function-exportimagedialog)**(QWidget * parent, [Preferences](../Classes/classPreferences.md) & prefs, QSharedPointer< [AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md) > analysis_tool, QSharedPointer< [Visualizer](../Classes/classshapeworks_1_1Visualizer.md) > visualizer, bool pca_mode) |

## Detailed Description

```cpp
class shapeworks::ExportImageDialog;
```


The [ExportImageDialog](../Classes/classshapeworks_1_1ExportImageDialog.md) is a QDialog that implements the user interface for exporting images 

## Public Functions Documentation

### function ExportImageDialog

```cpp
ExportImageDialog(
    QWidget * parent,
    Preferences & prefs,
    QSharedPointer< AnalysisTool > analysis_tool,
    QSharedPointer< Visualizer > visualizer,
    bool pca_mode
)
```


-------------------------------

Updated on 2022-06-13 at 08:58:03 +0000