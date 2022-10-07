---
title: Libs/Analyze/MeshWorkQueue.h

---

# Libs/Analyze/MeshWorkQueue.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md)** <br>Provides concurrent access to a list of shapes to work needing reconstruction.  |
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
#include <QMetaType>
#include <QMutex>

// eigen
#include <Eigen/Eigen>

namespace shapeworks {

class MeshWorkItem {
 public:
  std::string filename;
  Eigen::VectorXd points;
  int domain{0};

  size_t memory_size{0};

  friend bool operator<(const MeshWorkItem &a, const MeshWorkItem &b);

  friend bool operator==(const MeshWorkItem &a, const MeshWorkItem &b);
};

class MeshWorkQueue {
 public:
  using WorkList = std::list<MeshWorkItem>;

  MeshWorkQueue();
  ~MeshWorkQueue();

  void push(const MeshWorkItem &item);

  MeshWorkItem *get_next_work_item();

  bool is_inside(const MeshWorkItem &item);

  void remove(const MeshWorkItem &item);

  bool is_empty();

  int size();

 private:
  bool in_inside_list(const MeshWorkItem &item, const WorkList &list);

  // for concurrent access
  QMutex mutex_;

  WorkList work_list_;

  WorkList processing_list_;
};
}  // namespace shapeworks

Q_DECLARE_METATYPE(shapeworks::MeshWorkItem);
```


-------------------------------

Updated on 2022-10-07 at 04:24:32 +0000
