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
#include <ShapeWorksOptimize.h>
#include <Data/Project.h>

class ShapeworksWorker : public QObject
{
  Q_OBJECT

public:
  enum ThreadType { Groom, Optimize };
	ShapeworksWorker(ThreadType type, ShapeWorksGroom& groom,
    ShapeWorksOptimize& optimize,
    QSharedPointer<Project> project);
	~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void error_message(std::string);
  void step_made(int val);
  void finished();

private:
  ShapeWorksGroom& groom_;
  ShapeWorksOptimize& optimize_;
  QSharedPointer<Project> project_;
  ThreadType type_;
};

#endif // ifndef SHAPEWORKS_WORKER_H