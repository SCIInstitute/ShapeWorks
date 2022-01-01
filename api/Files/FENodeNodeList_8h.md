---
title: Libs/Mesh/PreviewMeshQC/FENodeNodeList.h

---

# Libs/Mesh/PreviewMeshQC/FENodeNodeList.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FENodeNodeList](../Classes/classFENodeNodeList.md)**  |




## Source code

```cpp
#pragma once

#include <vector>
using namespace std;

class FEMesh;

class FENodeNodeList
{
public:
    FENodeNodeList(FEMesh* pm);
    ~FENodeNodeList();

    void Build();

    int Valence(int n) { return m_val[n]; }
    int Node(int n, int j) { return m_node[ m_off[n] + j]; }

protected:
    FEMesh* m_pm;

    vector<int> m_val;
    vector<int> m_off;
    vector<int> m_node;
};
```


-------------------------------

Updated on 2022-01-01 at 17:59:59 +0000
