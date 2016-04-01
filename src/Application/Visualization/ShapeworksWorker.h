/*
 * Shapeworks license
 */

/**
 * @file MeshWorker.h
 * @brief Worker class for parallel mesh reconstruction
 *
 * The MeshWorker implements each threads mesh construction management
 */

#ifndef SHAPEWORKS_WORKER_H
#define SHAPEWORKS_WORKER_H

#include <QObject>
#include <QStringList>
#include <ShapeWorksGroom.h>

class ShapeworksWorker : public QObject
{
  Q_OBJECT

public:
  typedef enum ThreadType { Groom, Optimize };
	ShapeworksWorker(ThreadType type, ShapeWorksGroom& groom);
	~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void error_message(std::string);
  void step_made(int val);

private:
  ShapeWorksGroom& groom_;
  ThreadType type_;
};

#endif // ifndef SHAPEWORKS_WORKER_H