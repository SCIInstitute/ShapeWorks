---
title: Studio/src/Data/ItkToVtk.h

---

# Studio/src/Data/ItkToVtk.h



## Functions

|                | Name           |
| -------------- | -------------- |
| template <typename ITK_Exporter ,typename VTK_Importer \> <br>void | **[ConnectPipelines](../Files/ItkToVtk_8h.md#function-connectpipelines)**(ITK_Exporter exporter, VTK_Importer * importer) |


## Functions Documentation

### function ConnectPipelines

```cpp
template <typename ITK_Exporter ,
typename VTK_Importer >
void ConnectPipelines(
    ITK_Exporter exporter,
    VTK_Importer * importer
)
```


This function will connect the given itk::VTKImageExport filter to the given vtkImageImport filter. 




## Source code

```cpp
#pragma once

template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}
```


-------------------------------

Updated on 2022-01-27 at 02:24:33 +0000
