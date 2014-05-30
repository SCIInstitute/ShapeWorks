#ifndef STUDIO_ANALYSIS_ANALYSISTOOL_H
#define STUDIO_ANALYSIS_ANALYSISTOOL_H

#include <QSharedPointer>
#include <QWidget>

class Project;
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

public Q_SLOTS:


private:

  Ui_AnalysisTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;
};

#endif /* STUDIO_ANALYSIS_ANALYSISTOOL_H */
