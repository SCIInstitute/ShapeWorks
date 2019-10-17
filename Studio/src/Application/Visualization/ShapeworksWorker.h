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
#include <Groom/ShapeWorksGroom.h>
#include <Optimize/ShapeWorksOptimize.h>
#include <Data/Project.h>

class ShapeworksWorker : public QObject
{
  Q_OBJECT

public:
  enum ThreadType { Groom, Optimize, Reconstruct };
  ShapeworksWorker(ThreadType type, ShapeWorksGroom* groom,
                   ShapeWorksOptimize* optimize,
                   QSharedPointer<Project> project,
                   std::vector<std::vector<itk::Point<double>>> local_pts =
                     std::vector<std::vector<itk::Point<double>>>(),
                   std::vector<std::vector<itk::Point<double>>> global_pts =
                     std::vector<std::vector<itk::Point<double>>>(),
                   std::vector<ImageType::Pointer> distance_transform =
                     std::vector<ImageType::Pointer>(),
                   double maxAngle = 45.,
                   float decimationPercent = 0.3f,
                   int numClusters = 5);
  ~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void error_message(std::string);
  void warning_message(std::string);
  void step_made(int val);
  void finished();
  void message(std::string);

private:
  ShapeWorksGroom* groom_;
  ShapeWorksOptimize* optimize_;
  QSharedPointer<Project> project_;
  ThreadType type_;
  std::vector<std::vector<itk::Point<double>>> local_pts_;
  std::vector<std::vector<itk::Point<double>>> global_pts_;
  std::vector<ImageType::Pointer> distance_transform_;
  float decimationPercent_;
  double maxAngle_;
  int numClusters_;
};

#endif // ifndef SHAPEWORKS_WORKER_H
