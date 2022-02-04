---
title: FEVTKExport

---

# FEVTKExport





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEVTKExport](../Classes/classFEVTKExport.md#function-fevtkexport)**() |
| | **[~FEVTKExport](../Classes/classFEVTKExport.md#function-~fevtkexport)**() =default |
| bool | **[Export](../Classes/classFEVTKExport.md#function-export)**([FEMesh](../Classes/classFEMesh.md) & mesh, const char * szfile) |
| std::string | **[ExportToString](../Classes/classFEVTKExport.md#function-exporttostring)**([FEMesh](../Classes/classFEMesh.md) & mesh) |
| bool | **[ExportToStream](../Classes/classFEVTKExport.md#function-exporttostream)**([FEMesh](../Classes/classFEMesh.md) & mesh, std::ostream & out) |
| void | **[SetOptions](../Classes/classFEVTKExport.md#function-setoptions)**([VTKEXPORT](../Classes/structVTKEXPORT.md) o) |
| vtkSmartPointer< vtkPolyData > | **[ExportToVTK](../Classes/classFEVTKExport.md#function-exporttovtk)**([FEMesh](../Classes/classFEMesh.md) & mesh) |

## Public Functions Documentation

### function FEVTKExport

```cpp
FEVTKExport()
```


### function ~FEVTKExport

```cpp
~FEVTKExport() =default
```


### function Export

```cpp
bool Export(
    FEMesh & mesh,
    const char * szfile
)
```


### function ExportToString

```cpp
std::string ExportToString(
    FEMesh & mesh
)
```


### function ExportToStream

```cpp
bool ExportToStream(
    FEMesh & mesh,
    std::ostream & out
)
```


### function SetOptions

```cpp
inline void SetOptions(
    VTKEXPORT o
)
```


### function ExportToVTK

```cpp
vtkSmartPointer< vtkPolyData > ExportToVTK(
    FEMesh & mesh
)
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000