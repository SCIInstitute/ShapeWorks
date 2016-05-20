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
  enum ThreadType { Groom, Optimize, Reconstruct };
  ShapeworksWorker(ThreadType type, ShapeWorksGroom* groom,
    ShapeWorksOptimize* optimize,
    QSharedPointer<Project> project,
    std::vector<std::vector<itk::Point<float> > > local_pts =
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<std::vector<itk::Point<float> > > global_pts =
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<ImageType::Pointer> distance_transform =
    std::vector<ImageType::Pointer>() );
	~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void error_message(std::string);
  void step_made(int val);
  void finished();
  void message(std::string);

private:
  ShapeWorksGroom* groom_;
  ShapeWorksOptimize*optimize_;
  QSharedPointer<Project> project_;
  ThreadType type_;
  std::vector<std::vector<itk::Point<float> > > local_pts_;
  std::vector<std::vector<itk::Point<float> > > global_pts_;
  std::vector<ImageType::Pointer> distance_transform_;
};

#endif // ifndef SHAPEWORKS_WORKER_H