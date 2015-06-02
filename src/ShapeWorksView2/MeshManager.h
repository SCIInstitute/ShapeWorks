/*
 * Shapeworks license
 */

/**
 * @file MeshManager.h
 * @brief Class to manage meshes
 *
 * The MeshManager handles all aspects of mesh generation and caching.
 * It houses the cache and manages threads to work on mesh generation
 * in the background.
 */

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <vtkSmartPointer.h>

#include <QWaitCondition>

#include <MeshCache.h>
#include <MeshGenerator.h>
#include <MeshWorkQueue.h>
#include <MeshWorker.h>
#include <Preferences.h>

class MeshManager : public QObject
{
  Q_OBJECT

public:
  MeshManager(Preferences& prefs);
  ~MeshManager();

  vtkSmartPointer<vtkPolyData> getMesh( const vnl_vector<double>& shape );

  void clear_cache();

public Q_SLOTS:
  void handle_thread_complete();

private:

  // generate and cache a mesh for this shape in a queue
  void generateMesh( const vnl_vector<double>& shape );

  Preferences& prefs_;

  // cache of shape meshes
  MeshCache meshCache_;

  // our own mesh generator
  MeshGenerator meshGenerator_;

  // queue of meshes to build
  MeshWorkQueue workQueue_;
  
  // the workers
  std::vector<QThread*> threads_;
  
  size_t thread_count_;
};

#endif // ifndef MESH_Manager_H
