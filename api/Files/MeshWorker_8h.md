---
title: Studio/src/Data/MeshWorker.h
summary: Worker class for parallel mesh reconstruction. 

---

# Studio/src/Data/MeshWorker.h

Worker class for parallel mesh reconstruction.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWorker](../Classes/classshapeworks_1_1MeshWorker.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_DECLARE_METATYPE](../Files/MeshWorker_8h.md#function-q-declare-metatype)**(vtkSmartPointer< vtkPolyData > ) |

## Detailed Description

Worker class for parallel mesh reconstruction. 

The MeshWorker implements each thread's mesh construction management 


## Functions Documentation

### function Q_DECLARE_METATYPE

```cpp
Q_DECLARE_METATYPE(
    vtkSmartPointer< vtkPolyData > 
)
```




## Source code

```cpp
#pragma once

#include <QObject>
#include <QWaitCondition>
#include <QMetaType>
#include <QRunnable>

#include <Data/MeshWorkQueue.h>
#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>

Q_DECLARE_METATYPE(vtkSmartPointer<vtkPolyData>);

namespace shapeworks {

class MeshWorker : public QObject, public QRunnable {
Q_OBJECT

public:
  MeshWorker(MeshWorkQueue* queue, std::shared_ptr<MeshGenerator> generator);
  ~MeshWorker();

  void run();

Q_SIGNALS:
  void result_ready(const MeshWorkItem& item, MeshHandle mesh);
  void finished();

private:
  std::shared_ptr<MeshGenerator> mesh_generator_;
  MeshWorkQueue* queue_;
};
}
```


-------------------------------

Updated on 2022-01-14 at 02:28:14 +0000
