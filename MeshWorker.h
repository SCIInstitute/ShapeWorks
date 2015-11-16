/*
 * Shapeworks license
 */

/**
 * @file MeshWorker.h
 * @brief Worker class for parallel mesh reconstruction
 * 
 * The MeshWorker implements each threads mesh construction management
 */

#ifndef MESH_WORKER_H
#define MESH_WORKER_H

#include <QObject>
#include <QWaitCondition>

#include <MeshWorkQueue.h>
#include <MeshCache.h>
#include <MeshGenerator.h>

class MeshWorker : public QObject
{
  Q_OBJECT

public:
  void setWorkQueue( MeshWorkQueue* queue );
  void setWorkingQueue( MeshWorkQueue* queue );
  void setWorkDoneCondition( QWaitCondition* condition );
  void setMeshCache( MeshCache* cache );

  MeshGenerator* getMeshGenerator();

public Q_SLOTS:
  void threadBegin();

private:

  MeshGenerator meshGenerator;

  MeshWorkQueue* workQueue;
  MeshWorkQueue* workingQueue;
  MeshCache* meshCache;

  QWaitCondition* workDoneCondition;
};

#endif // ifndef MESH_WORKER_H
