#pragma once
/**
 * @file MeshManager.h
 * @brief Class to manage meshes
 *
 * The MeshManager handles all aspects of mesh generation and caching.
 * It houses the cache and manages threads to work on mesh generation
 * in the background.
 */

#include <vtkSmartPointer.h>

#include <QWaitCondition>
#include <QThreadPool>

#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>
#include <Data/MeshWorkQueue.h>
#include <Data/MeshWorker.h>
#include <Data/Preferences.h>
#include <Data/SurfaceReconstructor.h>

class MeshManager : public QObject
{
  Q_OBJECT

public:
  MeshManager(Preferences& prefs);
  ~MeshManager();

  //! generate and cache a mesh for this shape in a different thread
  void generate_mesh(const MeshWorkItem item);

  //! get a mesh for a shape
  vtkSmartPointer<vtkPolyData> get_mesh(const MeshWorkItem& shape);

  //! return the surface reconstructor
  QSharedPointer<SurfaceReconstructor> get_surface_reconstructor();

  //! clear the cache
  void clear_cache();

  void shutdown_threads();

public Q_SLOTS:
  void handle_thread_complete(const MeshWorkItem &item, vtkSmartPointer<vtkPolyData> mesh);

Q_SIGNALS:
  void new_mesh();

private:

  void do_work();

  Preferences& prefs_;

  // cache of shape meshes
  MeshCache mesh_cache_;

  // our own mesh generator
  MeshGenerator mesh_generator_;

  // queue of meshes to build
  MeshWorkQueue work_queue_;

  // the workers
  std::queue<QThread*> threads_;

  int thread_count_;

  QSharedPointer<SurfaceReconstructor> surface_reconstructor_;

  QThreadPool thread_pool_;
};
