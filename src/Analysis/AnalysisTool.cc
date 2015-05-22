#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <Application/ShapeWorksStudioApp.h>

#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Analysis/AnalysisTool.h>

#include <Analysis/itkParticleShapeStatistics.h>

#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>

#include <ui_AnalysisTool.h>

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool(Preferences& prefs) : preferences_(prefs)
{

  this->ui_ = new Ui_AnalysisTool;
  this->ui_->setupUi( this );
}

//---------------------------------------------------------------------------
AnalysisTool::~AnalysisTool()
{}

//---------------------------------------------------------------------------
void AnalysisTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void AnalysisTool::set_app( ShapeWorksStudioApp* app )
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_mean_overall_clicked()
{
  this->visualizer_->display_mean();
}

//---------------------------------------------------------------------------
void AnalysisTool::set_visualizer( VisualizerHandle visualizer )
{
  this->visualizer_ = visualizer;
}

//---------------------------------------------------------------------------
void AnalysisTool::activate()
{

  this->update_analysis_mode();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_analysis_mode()
{
  // update UI

  // groups available
  this->ui_->meanGroup1Button->setVisible( false );
  this->ui_->meanGroup2Button->setVisible( false );
  this->ui_->medianGroup1Button->setVisible( false );
  this->ui_->medianGroup2Button->setVisible( false );
  this->ui_->mean_overall->setChecked(true);
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape()
{
}
