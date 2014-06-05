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





public Q_SLOTS:
  void on_mean_overall_clicked();

  //--------------------------------------------------
  // visualization panel
  //--------------------------------------------------
  void on_show_surface_stateChanged();
  void on_show_glyphs_stateChanged();
  void on_glyph_size_valueChanged( int value );
  void on_glyph_quality_valueChanged( int value );

private:


  void update_from_preferences();  


  Ui_AnalysisTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;

  VisualizerHandle visualizer_;





};

#endif /* STUDIO_ANALYSIS_ANALYSISTOOL_H */
