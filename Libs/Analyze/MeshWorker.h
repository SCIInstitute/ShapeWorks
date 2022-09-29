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

#include <MeshWorkQueue.h>
#include <MeshCache.h>
#include <MeshGenerator.h>

Q_DECLARE_METATYPE(vtkSmartPointer<vtkPolyData>);

namespace shapeworks {

class MeshWorker : public QObject, public QRunnable {
Q_OBJECT

public:
  MeshWorker(MeshWorkQueue* queue, std::shared_ptr<MeshGenerator> generator);
  ~MeshWorker();

  void run();

Q_SIGNALS:
  void result_ready(const MeshWorkItem& item, MeshHandle mesh);
  void finished();

private:
  std::shared_ptr<MeshGenerator> mesh_generator_;
  MeshWorkQueue* queue_;
};
}
