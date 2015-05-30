#ifndef STUDIO_ANALYSIS_ANALYSISTOOL_H
#define STUDIO_ANALYSIS_ANALYSISTOOL_H

#include <QSharedPointer>
#include <QWidget>

#include <Visualization/Visualizer.h>

#include <Data/Shape.h>

#include "Application/Preferences.h"

#include <Analysis/itkParticleShapeStatistics.h>

class Project;
class Lightbox;
class ShapeWorksStudioApp;
class Ui_AnalysisTool;

class AnalysisTool : public QWidget
{
  Q_OBJECT;
public:

  AnalysisTool(Preferences& prefs);
  ~AnalysisTool();

  /// export XML for ShapeWorksAnalysis
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  /// set the pointer to the application
  void set_app( ShapeWorksStudioApp* app );

  void set_shapes( ShapeList shapes );

  void activate();

  std::string getAnalysisMode();
  void setAnalysisMode(std::string i);

  QSlider* getPCASlider();

  void setLabels(QString which, QString value);

  int getPCAMode();

  bool pcaAnimate();

  int getSampleNumber();

  bool compute_stats();

  void reset_stats();

  const vnl_vector<double> & getMean();
  
  const vnl_vector<double> & getShape();


public Q_SLOTS:
  void handle_analysis_options();
  void handle_median();

  // PCA
  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();

  void handle_pca_animate_state_changed();
  void handle_pca_timer();

signals:
  void update_view();
  void pca_update();
  
private:
  //private methods
  void pca_labels_changed( QString value, QString eigen, QString lambda );
  void compute_mode_shape();
  double get_pca_value( );
  void update_analysis_mode();
  //private members
  Preferences & preferences_;
  Ui_AnalysisTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;

  VisualizerHandle visualizer_;

  /// itk particle shape statistics
  ParticleShapeStatistics<3> stats_;
  bool stats_ready_;

  vnl_vector<double> empty_shape_;
  vnl_vector<double> temp_shape_;

  bool pcaAnimateDirection;
  QTimer pcaAnimateTimer;


};

#endif /* STUDIO_ANALYSIS_ANALYSISTOOL_H */
