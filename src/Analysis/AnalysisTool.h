#ifndef STUDIO_ANALYSIS_ANALYSISTOOL_H
#define STUDIO_ANALYSIS_ANALYSISTOOL_H

#include <QSharedPointer>
#include <QWidget>

#include <Data/Shape.h>

//#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include <Analysis/itkParticleShapeStatistics.h>

class Project;
class Lightbox;
class ShapeWorksStudioApp;
class Ui_AnalysisTool;

class AnalysisTool : public QWidget
{
  Q_OBJECT;
public:

  AnalysisTool();
  ~AnalysisTool();

  /// export XML for ShapeWorksAnalysis
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  /// set the pointer to the application
  void set_app( ShapeWorksStudioApp* app );

  void set_lightbox( QSharedPointer<Lightbox> lightbox );

  void set_shapes( ShapeList shapes );

public Q_SLOTS:
  void on_mean_overall_clicked();

private:

  void update_shape_stats();

  Ui_AnalysisTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;

  /// todo: change to handle
  QSharedPointer<Lightbox> lightbox_;

  ShapeList shapes_;

  ParticleShapeStatistics<3> stats;
  //itk::ParticleShapeLinearRegressionMatrixAttribute<double, 3>::Pointer regression;


};

#endif /* STUDIO_ANALYSIS_ANALYSISTOOL_H */
