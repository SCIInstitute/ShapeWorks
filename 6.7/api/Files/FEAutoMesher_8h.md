---
title: Libs/Mesh/PreviewMeshQC/FEAutoMesher.h

---

# Libs/Mesh/PreviewMeshQC/FEAutoMesher.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FEAutoMesher](../Classes/classFEAutoMesher.md)**  |




## Source code

```cpp
#pragma once

class FEMesh;

//-----------------------------------------------------------------------------
class FEAutoMesher
{
public:
    FEAutoMesher();
    void BuildMesh(FEMesh* pm);

public:
    void AutoPartitionSurface(FEMesh& m);
    void AutoPartitionEdges(FEMesh& m);
    void AutoPartitionNodes(FEMesh& m);

    void Repartition(FEMesh& m);

    void SetSmoothingAngle(double w) { m_wsmooth = w; }
    double GetSmoothingAngle() { return m_wsmooth; }

protected:
    void BuildFaces(FEMesh& m);
    void BuildEdges(FEMesh& m);

protected:
    FEMesh*     m_pm;

    double  m_wsmooth;
};
```


-------------------------------

Updated on 2025-09-26 at 16:33:40 +0000
