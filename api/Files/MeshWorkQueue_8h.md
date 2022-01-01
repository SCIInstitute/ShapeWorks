---
title: Studio/src/Data/MeshWorkQueue.h
summary: Provides concurrent access to a list of shapes to work needing reconstruction. 

---

# Studio/src/Data/MeshWorkQueue.h

Provides concurrent access to a list of shapes to work needing reconstruction. 

## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md)**  |
| class | **[shapeworks::MeshWorkQueue](../Classes/classshapeworks_1_1MeshWorkQueue.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_DECLARE_METATYPE](../Files/MeshWorkQueue_8h.md#function-q-declare-metatype)**([shapeworks::MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) ) |


## Functions Documentation

### function Q_DECLARE_METATYPE

```cpp
Q_DECLARE_METATYPE(
    shapeworks::MeshWorkItem 
)
```




## Source code

```cpp
#pragma once

// stl
#include <list>

// qt
#include <QMutex>
#include <QMetaType>

// vnl
#include "vnl/vnl_vector.h"


namespace shapeworks {

class MeshWorkItem
{
public:
  std::string filename;
  vnl_vector<double> points;
  int domain{0};

  size_t memory_size{0};

  friend bool operator<(const MeshWorkItem &a, const MeshWorkItem &b);

  friend bool operator==(const MeshWorkItem &a, const MeshWorkItem &b);
};


class MeshWorkQueue
{

public:

  using WorkList = std::list<MeshWorkItem>;

  MeshWorkQueue();
  ~MeshWorkQueue();

  void push(const MeshWorkItem &item);

  MeshWorkItem* get_next_work_item();

  bool is_inside(const MeshWorkItem &item);

  void remove(const MeshWorkItem &item);

  bool is_empty();

  int size();

private:

  bool in_inside_list(const MeshWorkItem &item, const WorkList& list);

  // for concurrent access
  QMutex mutex_;

  WorkList work_list_;

  WorkList processing_list_;
};
}

Q_DECLARE_METATYPE(shapeworks::MeshWorkItem);
```


-------------------------------

Updated on 2022-01-01 at 17:29:14 +0000
