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

Updated on 2022-04-08 at 15:18:26 +0000
