#pragma once

/**
 * @file MeshWorker.h
 * @brief Worker class for parallel mesh reconstruction
 *
 * The MeshWorker implements each thread's mesh construction management
 */

#include <QObject>
#include <QWaitCondition>
#include <QMetaType>
#include <QRunnable>

#include <Data/MeshWorkQueue.h>
#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>

Q_DECLARE_METATYPE(vtkSmartPointer<vtkPolyData>);

class MeshWorker : public QObject, public QRunnable
{
  Q_OBJECT

public:
  MeshWorker(Preferences& prefs,
             const MeshWorkItem &item,
             MeshWorkQueue* queue,
             MeshCache* cache);
  ~MeshWorker();
  MeshGenerator* get_mesh_generator();

  void run();

Q_SIGNALS:
  void result_ready(const MeshWorkItem &item, vtkSmartPointer<vtkPolyData> mesh);
  void finished();

private:
  Preferences& prefs_;
  MeshGenerator mesh_generator_;
  MeshWorkItem item_;
  MeshWorkQueue* queue_;
  MeshCache* cache_;
};
