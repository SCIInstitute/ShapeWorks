---
title: Libs/Mesh/PreviewMeshQC/FENodeElementList.h

---

# Libs/Mesh/PreviewMeshQC/FENodeElementList.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FENodeElementList](../Classes/classFENodeElementList.md)**  |




## Source code

```cpp
#pragma once

#include <vector>
using namespace std;

#include "FEMesh.h"

class FENodeElementList
{
public:
    FENodeElementList(FEMesh* pm);
    ~FENodeElementList();

    void Build();

    int Valence(int n) { return m_val[n]; }
    FEElement* Element(int n, int j) { return m_pelem[m_off[n] + j]; }

protected:
    FEMesh* m_pm;

    vector<int> m_val;
    vector<int> m_off;
    vector<FEElement*>  m_pelem;
};
```


-------------------------------

Updated on 2024-05-02 at 21:06:13 +0000