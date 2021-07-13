#pragma once

#include <Libs/Mesh/MeshWarper.h>
#include <QObject>
#include <QMutex>
#include <QElapsedTimer>

namespace shapeworks {

//! Wraps MeshWarper as a QObject
class QMeshWarper : public QObject, public MeshWarper {
Q_OBJECT;

public:
  QMeshWarper(QObject* parent = nullptr);

  float get_progress();

protected:

  void update_progress(float p) override;

Q_SIGNALS:
  void progress();

private:
  float current_progress_ = 0;
};

}