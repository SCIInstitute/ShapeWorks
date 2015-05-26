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

  //connect( this->ui_->allSamplesRadio, SIGNAL( clicked() ), this, SLOT( handle_display_setting_changed() ) );
  //connect( this->ui_->singleSampleRadio, SIGNAL( clicked() ), this, SLOT( handle_display_setting_changed() ) );
  //connect( this->ui_->meanRadio, SIGNAL( clicked() ), this, SLOT( handle_display_setting_changed() ) );
  //connect( this->ui_->pcaRadio, SIGNAL( clicked() ), this, SLOT( handle_display_setting_changed() ) );
  //connect( this->ui_->regressionRadio, SIGNAL( clicked() ), this, SLOT( handle_display_setting_changed() ) );
}

//---------------------------------------------------------------------------
std::string AnalysisTool::getAnalysisMode() {
	if (this->ui_->allSamplesRadio->isChecked()) return "all samples";
	if (this->ui_->allSamplesRadio->isChecked()) return "one samples";
	if (this->ui_->allSamplesRadio->isChecked()) return "mean";
	if (this->ui_->allSamplesRadio->isChecked()) return "pca";
	if (this->ui_->allSamplesRadio->isChecked()) return "regression";
	return "";
}

//---------------------------------------------------------------------------
int AnalysisTool::getPCAMode() {
	return this->ui_->pcaModeSpinBox->value();
}

//---------------------------------------------------------------------------
bool AnalysisTool::pcaAnimate() {
	return this->ui_->pcaAnimateCheckBox->isChecked();
}

//---------------------------------------------------------------------------
void AnalysisTool::setLabels(QString which, QString value) {
	if (which == QString("pca"))
		this->ui_->pcaValueLabel->setText( value );
	else if (which == QString("eigen"))
		this->ui_->pcaEigenValueLabel->setText( value );
	else if (which == QString("lambda"))
		this->ui_->pcaLambdaLabel->setText( value );
}

//---------------------------------------------------------------------------
QSlider* AnalysisTool::getPCASlider() {
	return this->ui_->pcaSlider;
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
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape()
{
}
