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
  this->stats_ready_ = false;

  this->pcaAnimateDirection = true;

  connect( this->ui_->allSamplesRadio, SIGNAL( clicked() ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->singleSamplesRadio, SIGNAL( clicked() ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->meanRadio, SIGNAL( clicked() ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->pcaRadio, SIGNAL( clicked() ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->regressionRadio, SIGNAL( clicked() ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->sampleSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( handle_analysis_options() ) );
  connect( this->ui_->medianButton, SIGNAL( clicked() ), this, SLOT( handle_median() ) );
  connect( this->ui_->pcaAnimateCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( handle_pca_animate_state_changed() ) );
  connect( &this->pcaAnimateTimer, SIGNAL( timeout() ), this, SLOT( handle_pca_timer() ) );
}

//---------------------------------------------------------------------------
std::string AnalysisTool::getAnalysisMode() {
	if (this->ui_->allSamplesRadio->isChecked()) return "all samples";
	if (this->ui_->singleSamplesRadio->isChecked()) return "single sample";
	if (this->ui_->meanRadio->isChecked()) return "mean";
	if (this->ui_->pcaRadio->isChecked()) return "pca";
	if (this->ui_->regressionRadio->isChecked()) return "regression";
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
int AnalysisTool::getSampleNumber() {
	return this->ui_->sampleSpinBox->value();
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

//---------------------------------------------------------------------------
void AnalysisTool::handle_analysis_options() { 
	//all samples mode
	if (this->ui_->allSamplesRadio->isChecked()) {
		this->ui_->sampleSpinBox->setEnabled(false);
		this->ui_->medianButton->setEnabled(false);
		this->ui_->pcaSlider->setEnabled(false);
		this->ui_->pcaAnimateCheckBox->setEnabled(false);
		this->ui_->pcaModeSpinBox->setEnabled(false);
	//one sample mode
	} else if(this->ui_->singleSamplesRadio->isChecked()) {
		this->ui_->sampleSpinBox->setEnabled(true);
		this->ui_->medianButton->setEnabled(true);
		this->ui_->pcaSlider->setEnabled(false);
		this->ui_->pcaAnimateCheckBox->setEnabled(false);
		this->ui_->pcaModeSpinBox->setEnabled(false);
	//mean mode
	} else if(this->ui_->meanRadio->isChecked()) {
		this->ui_->sampleSpinBox->setEnabled(false);
		this->ui_->medianButton->setEnabled(false);
		this->ui_->pcaSlider->setEnabled(false);
		this->ui_->pcaAnimateCheckBox->setEnabled(false);
		this->ui_->pcaModeSpinBox->setEnabled(false);
	//pca mode
	} else if(this->ui_->pcaRadio->isChecked()) {
		this->ui_->sampleSpinBox->setEnabled(false);
		this->ui_->medianButton->setEnabled(false);
		this->ui_->pcaSlider->setEnabled(true);
		this->ui_->pcaAnimateCheckBox->setEnabled(true);
		this->ui_->pcaModeSpinBox->setEnabled(true);
	//regression mode
	} else {
		this->ui_->sampleSpinBox->setEnabled(false);
		this->ui_->medianButton->setEnabled(false);
		this->ui_->pcaSlider->setEnabled(false);
		this->ui_->pcaAnimateCheckBox->setEnabled(false);
		this->ui_->pcaModeSpinBox->setEnabled(false);
	}
	emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_median() { 
	if (!this->compute_stats()) return;
	this->ui_->sampleSpinBox->setValue(this->stats_.ComputeMedianShape(-32)); //-32 = both groups
	emit update_view();
}

//-----------------------------------------------------------------------------
bool AnalysisTool::compute_stats()
{
  if ( this->stats_ready_ )
  {
    return true;
  }

  if ( this->project_->get_shapes().size() == 0 || !this->project_->reconstructed_present() )
  {
    return false;
  }

  std::vector < vnl_vector < double > > points;
  foreach( ShapeHandle shape, this->project_->get_shapes() ) {
    points.push_back( shape->get_global_correspondence_points() );
  }

  this->stats_.ImportPoints( points );
  this->stats_.ComputeModes();
  this->stats_ready_ = true;

  return true;
}

//-----------------------------------------------------------------------------
 const vnl_vector<double> & AnalysisTool::getMean()
{
	if (!this->compute_stats()) return this->empty_shape_;
	return this->stats_.Mean();
}

 //-----------------------------------------------------------------------------
const vnl_vector<double> & AnalysisTool::getShape() {
  int mode = this->getPCAMode();
  double value = this->get_pca_value();
  if ( !this->compute_stats() || this->stats_.Eigenvectors().size() <= 1)
    return this->empty_shape_;
  if (mode + 2 > this->stats_.Eigenvalues().size()) mode = this->stats_.Eigenvalues().size() - 2;

  unsigned int m = this->stats_.Eigenvectors().columns() - (mode+1) ;

  vnl_vector<double> e = this->stats_.Eigenvectors().get_column( m );

  double lambda = sqrt( this->stats_.Eigenvalues()[m] );

  this->pca_labels_changed( QString::number( value, 'g', 2 ),
                           QString::number( this->stats_.Eigenvalues()[m] ),
                           QString::number( value * lambda ) );

  this->temp_shape_ = this->stats_.Mean() + ( e * ( value * lambda ) );
  return this->temp_shape_;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaSlider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  emit pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaModeSpinBox_valueChanged()
{
  emit pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_animate_state_changed()
{
  if ( this->pcaAnimate())
  {
    //this->setPregenSteps();
    this->pcaAnimateTimer.setInterval( 10 );
    this->pcaAnimateTimer.start();
  }
  else
  {
    this->pcaAnimateTimer.stop();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_timer()
{
  int value = this->getPCASlider()->value();
  if ( this->pcaAnimateDirection )
  {
    value += this->getPCASlider()->singleStep();
  }
  else
  {
    value -= this->getPCASlider()->singleStep();
  }

  if ( value >= this->getPCASlider()->maximum() || value <= this->getPCASlider()->minimum() )
  {
    this->pcaAnimateDirection = !this->pcaAnimateDirection;
    //this->setPregenSteps();
  }

  this->getPCASlider()->setValue( value );
}

//---------------------------------------------------------------------------
double AnalysisTool::get_pca_value( )
{
  int slider_value = this->ui_->pcaSlider->value();
  float range = preferences_.get_pca_range();
  int halfRange = this->getPCASlider()->maximum();

  double value = (double)slider_value / (double)halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void AnalysisTool::pca_labels_changed( QString value, QString eigen, QString lambda )
{
	this->setLabels(QString("pca"),value);
	this->setLabels(QString("eigen"),eigen);
	this->setLabels(QString("lambda"),lambda);
}

//---------------------------------------------------------------------------
void AnalysisTool::reset_stats()
{
	this->stats_ready_ = false;
	this->compute_stats();
}