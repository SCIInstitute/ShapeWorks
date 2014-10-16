#ifndef STUDIO_ANALYSIS_ANALYSISTOOL_H
#define STUDIO_ANALYSIS_ANALYSISTOOL_H

#include <QSharedPointer>
#include <QWidget>

#include <Visualization/Visualizer.h>

#include <Data/Shape.h>


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


  void set_visualizer( VisualizerHandle visualizer);

  void set_shapes( ShapeList shapes );


  void activate();


public Q_SLOTS:
  void on_mean_overall_clicked();

  void on_tabWidget_currentChanged();

  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();


private:


  double get_pca_value( int slider_value );

  void compute_mode_shape();

  void update_analysis_mode();

  Ui_AnalysisTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;

  VisualizerHandle visualizer_;

  /// itk particle shape statistics
  ParticleShapeStatistics<3> stats;




};

#endif /* STUDIO_ANALYSIS_ANALYSISTOOL_H */
