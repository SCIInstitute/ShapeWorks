#ifndef STUDIO_OPTIMIZE_OPTIMIZETOOL_H
#define STUDIO_OPTIMIZE_OPTIMIZETOOL_H

#include <QSharedPointer>
#include <QWidget>

class Project;
class ShapeWorksStudioApp;
class Ui_OptimizeTool;

class OptimizeTool : public QWidget
{
  Q_OBJECT;
public:

  OptimizeTool();
  ~OptimizeTool();

  /// export XML for ShapeWorksOptimize
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  /// set the pointer to the application
  void set_app( ShapeWorksStudioApp* app );

public Q_SLOTS:

  /// Export XML
  void on_export_xml_button_clicked();

  /// Run optimize tool
  void on_run_optimize_button_clicked();

private:

  Ui_OptimizeTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;
};

#endif /* STUDIO_OPTIMIZE_OPTIMIZETOOL_H */
