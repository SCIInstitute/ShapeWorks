---
title: Libs/Analyze/MeshManager.h

---

# Libs/Analyze/MeshManager.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshManager](../Classes/classshapeworks_1_1MeshManager.md)** <br>Class to manage meshes.  |




## Source code

```cpp
#pragma once

#include "MeshCache.h"
#include "MeshGenerator.h"
#include "MeshWorkQueue.h"
#include "MeshWorker.h"

#include <vtkSmartPointer.h>

#include <QThreadPool>
#include <QWaitCondition>

class SurfaceReconstructor;

namespace shapeworks {

class MeshWarper;

class MeshManager : public QObject {
  Q_OBJECT

 public:
  MeshManager();
  ~MeshManager();

  void set_cache_enabled(bool enabled);

  void set_cache_memory_percent(int percent);

  void set_parallel_enabled(bool enabled) { parallel_enabled_ = enabled; }

  void set_num_threads(int num_threads) { num_threads_ = num_threads; }

  void generate_mesh(const MeshWorkItem item);

  MeshHandle get_mesh(const MeshWorkItem& item, bool wait = false);

  MeshHandle get_mesh(const Eigen::VectorXd& points, int domain, bool wait = false);

  std::shared_ptr<SurfaceReconstructor> get_surface_reconstructor(int domain);

  std::shared_ptr<MeshWarper> get_mesh_warper(int domain);

  std::shared_ptr<MeshGenerator> get_mesh_generator() { return this->mesh_generator_; }

  void clear_cache();

 public Q_SLOTS:

  void handle_thread_complete(const MeshWorkItem& item, MeshHandle mesh);

  void handle_warper_progress();

 Q_SIGNALS:

  void new_mesh();

  void error_encountered(QString message);

  void progress(int);
  void status(std::string);

 private:
  std::shared_ptr<MeshReconstructors> reconstructors_ = std::make_shared<MeshReconstructors>();

  void check_error_status(MeshHandle mesh);

  // cache of shape meshes
  MeshCache mesh_cache_;

  // the mesh generator
  std::shared_ptr<MeshGenerator> mesh_generator_ = std::make_shared<MeshGenerator>();

  // queue of meshes to build
  MeshWorkQueue work_queue_;

  QThreadPool thread_pool_;

  bool cache_enabled_ = true;
  bool parallel_enabled_ = true;
  int num_threads_ = 1;

  bool error_emitted_ = false;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-03 at 17:42:44 +0000
