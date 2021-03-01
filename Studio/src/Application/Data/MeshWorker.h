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

namespace shapeworks {

class MeshWorker : public QObject, public QRunnable {
Q_OBJECT

public:
  MeshWorker(MeshWorkQueue* queue, QSharedPointer<MeshGenerator> generator);
  ~MeshWorker();

  void run();

Q_SIGNALS:
  void result_ready(const MeshWorkItem& item, MeshHandle mesh);
  void finished();

private:
  QSharedPointer<MeshGenerator> mesh_generator_;
  MeshWorkQueue* queue_;
};
}