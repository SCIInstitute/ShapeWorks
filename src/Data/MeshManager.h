#ifndef STUDIO_DATA_MESHMANAGER_H
#define STUDIO_DATA_MESHMANAGER_H

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>

#include <Data/MeshSettings.h>
#include <Data/MeshCache.h>
#include <Data/MeshWorkQueue.h>
#include <Data/MeshWorker.h>
#include <Data/MeshGenerator.h>
#include "Application/Preferences.h"

class Mesh;

class MeshManager;
typedef QSharedPointer< MeshManager > MeshManagerHandle;

//! Creates, stores, and manages meshes.
/*!
 * The MeshManager class is responsible for creating, storing and managing all meshes.
 *
 */
class MeshManager : public QObject
{
  Q_OBJECT;

public:

  /// Constructor
  MeshManager(Preferences& prefs);

  /// Destructor
  ~MeshManager();

  /// Queue a mesh to be generated from an image file
  void queue_mesh_creation( QString filename, MeshSettings settings );

  /// Queue a mesh to be generated from a set of correspondence points
  void queue_mesh_creation( const vnl_vector<double>& vnl_points, MeshSettings settings );

  /// Get mesh for an image file
  QSharedPointer<Mesh> get_mesh( QString filename, MeshSettings settings, bool wait );

  /// Get mesh fora set of correspondence points
  QSharedPointer<Mesh> get_mesh( const vnl_vector<double>& vnl_points, MeshSettings settings, bool wait );

private Q_SLOTS:
  void initialize_threads();

private:

  Preferences & preferences_;

  void shutdown_threads();

  // cache of shape meshes
  MeshCache meshCache;

  // our own mesh generator
  MeshGenerator meshGenerator;

  // queue of meshes to build
  MeshWorkQueue workQueue;

  // list of meshes that are currently being worked on in other threads
  MeshWorkQueue workingQueue;

  // the workers
  std::vector<QThread*> threads;
  std::vector<MeshWorker*> workers;

  // condition to wake us up when work is completed by one of the workers
  // used when we are asked for a mesh that is currently being worked on by another thread
  QWaitCondition workDoneCondition;

  // mesh generation settings
  int neighborhoodSize;
  double sampleSpacing;
  bool usePowerCrust;
  float smoothingAmount;
};

#endif /* STUDIO_DATA_MESHMANAGER_H */
