#pragma once

/**
 * @file MeshWorker.h
 * @brief Worker class for parallel mesh reconstruction
 *
 * The MeshWorker implements each thread's mesh construction management
 */


#include <QObject>
#include <QWaitCondition>

#include <Data/MeshWorkQueue.h>
#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>

class MeshWorker : public QObject
{
  Q_OBJECT

public:
  MeshWorker(Preferences& prefs,
             const MeshWorkItem &item,
             MeshWorkQueue* queue,
             MeshCache* cache);
  ~MeshWorker();
  MeshGenerator* get_mesh_generator();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void finished();

private:
  Preferences& prefs_;
  MeshGenerator mesh_generator_;
  MeshWorkItem item_;
  MeshWorkQueue* queue_;
  MeshCache* cache_;
};
