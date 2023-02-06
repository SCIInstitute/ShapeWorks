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

/**
 * @brief Class to manage meshes
 *
 * The MeshManager handles all aspects of mesh generation and caching.
 * It houses the cache and manages threads to work on mesh generation
 * in the background.
 */
class MeshManager : public QObject {
  Q_OBJECT

 public:
  MeshManager();
  ~MeshManager();

  //! Set if the cache should be enabled
  void set_cache_enabled(bool enabled);

  //! Set the memory cache size
  void set_cache_memory_percent(int percent);

  //! Set if parallel reconstruction should be enabled
  void set_parallel_enabled(bool enabled) { parallel_enabled_ = enabled; }

  //! Set the number of threads for parallel reconstruction
  void set_num_threads(int num_threads) { num_threads_ = num_threads; }

  //! generate and cache a mesh for this shape in a different thread
  void generate_mesh(const MeshWorkItem item);

  //! get a mesh for a MeshWorkItem
  MeshHandle get_mesh(const MeshWorkItem& item, bool wait = false);

  //! get a mesh for a set of points
  MeshHandle get_mesh(const Eigen::VectorXd& points, int domain, bool wait = false);

  //! return the surface reconstructor for a given domain
  std::shared_ptr<SurfaceReconstructor> get_surface_reconstructor(int domain);

  //! return the mesh warper for a given domain
  std::shared_ptr<MeshWarper> get_mesh_warper(int domain);

  //! return the mesh generator
  std::shared_ptr<MeshGenerator> get_mesh_generator() { return this->mesh_generator_; }

  //! clear the cache
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
