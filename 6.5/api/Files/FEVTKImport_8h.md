---
title: Libs/Mesh/PreviewMeshQC/FEVTKImport.h

---

# Libs/Mesh/PreviewMeshQC/FEVTKImport.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FEVTKimport](../Classes/classFEVTKimport.md)**  |




## Source code

```cpp
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <istream>
using namespace std;

class FEMesh;
class vtkPolyData;

class FEVTKimport
{
public:
  FEVTKimport();
  ~FEVTKimport(void);

  FEMesh* Load(const char* szfile);

  FEMesh* Load(istream& stream);

  FEMesh* Load(vtkPolyData *polydata);


protected:
  bool BuildMesh();
  FEMesh* errf(const char* sz, ...);
  void Close();

private:
  FILE*   m_fp;

};
```


-------------------------------

Updated on 2024-01-17 at 16:55:16 +0000