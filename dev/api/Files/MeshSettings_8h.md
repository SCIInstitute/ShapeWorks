---
title: Studio/Data/MeshSettings.h

---

# Studio/Data/MeshSettings.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[MeshSettings](../Classes/classMeshSettings.md)** <br>Settings for creating a mesh.  |

## Types

|                | Name           |
| -------------- | -------------- |
| typedef QSharedPointer< [MeshSettings](../Classes/classMeshSettings.md) > | **[MeshSettingsHandle](../Files/MeshSettings_8h.md#typedef-meshsettingshandle)**  |

## Types Documentation

### typedef MeshSettingsHandle

```cpp
typedef QSharedPointer< MeshSettings > MeshSettingsHandle;
```





## Source code

```cpp
#pragma once

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>

class MeshSettings;
typedef QSharedPointer< MeshSettings > MeshSettingsHandle;


class MeshSettings
{

public:

  MeshSettings();

  ~MeshSettings();

private:
};
```


-------------------------------

Updated on 2023-01-31 at 20:06:22 +0000
