---
title: Libs/Mesh/PreviewMeshQC/FEVTKExport.h

---

# Libs/Mesh/PreviewMeshQC/FEVTKExport.h



## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[VTKEXPORT](../Classes/structVTKEXPORT.md)**  |
| class | **[FEVTKExport](../Classes/classFEVTKExport.md)**  |




## Source code

```cpp
#pragma once

#include <string>
#include <ostream>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class FEMesh;

//-----------------------------------------------------------------------------
struct VTKEXPORT
{
  bool bshellthick;             // shell thickness
  bool bscalar_data;            // user scalar data
};

class FEVTKExport
{
public:
  FEVTKExport();
  ~FEVTKExport() = default;

  bool Export(FEMesh& mesh, const char* szfile);
  std::string ExportToString(FEMesh& mesh);
  bool ExportToStream(FEMesh& mesh, std::ostream& out);
  void SetOptions(VTKEXPORT o) { m_ops = o; }

  vtkSmartPointer<vtkPolyData> ExportToVTK(FEMesh &mesh);

private:
  VTKEXPORT m_ops;
};
```


-------------------------------

Updated on 2022-01-07 at 20:14:46 +0000
