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

#include <Data/MeshWorkQueue.h>
#include <Data/MeshCache.h>
#include <Data/MeshGenerator.h>
#include <Reconstruction.h>

class MeshWorker : public QObject
{
  Q_OBJECT

public:
	MeshWorker(Preferences& prefs, 
		const vnl_vector<double> shape,
    Reconstruction& construct,
		MeshWorkQueue *queue,
		MeshCache * cache);
	~MeshWorker();
  MeshGenerator* getMeshGenerator();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void finished();

private:
  Preferences& prefs_;
  MeshGenerator meshGenerator_;
  vnl_vector<double> shape_;
  MeshWorkQueue *queue_;
  MeshCache * cache_;
};

#endif // ifndef MESH_WORKER_H
