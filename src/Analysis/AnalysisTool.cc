#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <Application/ShapeWorksStudioApp.h>
#include <Application/Preferences.h>

#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Analysis/AnalysisTool.h>

#include <Analysis/itkParticleShapeStatistics.h>

#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>

#include <ui_AnalysisTool.h>

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool()
{

  this->ui_ = new Ui_AnalysisTool;
  this->ui_->setupUi( this );
  this->update_from_preferences();
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
void AnalysisTool::on_tabWidget_currentChanged()
{
  std::cerr << "TabWidget changed!\n";
  this->update_analysis_mode();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_show_surface_stateChanged()
{
  this->visualizer_->set_show_surface( this->ui_->show_surface->isChecked() );
}

//---------------------------------------------------------------------------
void AnalysisTool::on_show_glyphs_stateChanged()
{
  this->visualizer_->set_show_glyphs( this->ui_->show_glyphs->isChecked() );
}

//---------------------------------------------------------------------------
void AnalysisTool::on_glyph_size_valueChanged( int value )
{
  Preferences::Instance().set_glyph_size( value / 10.0 );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_glyph_quality_valueChanged( int value )
{
  Preferences::Instance().set_glyph_quality( value );
  this->update_from_preferences();
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
void AnalysisTool::update_from_preferences()
{
  this->ui_->glyph_quality->setValue( Preferences::Instance().get_glyph_quality() );
  this->ui_->glyph_quality_label->setText( QString::number( Preferences::Instance().get_glyph_quality() ) );
  this->ui_->glyph_size->setValue( Preferences::Instance().get_glyph_size() * 10.0 );
  this->ui_->glyph_size_label->setText( QString::number( Preferences::Instance().get_glyph_size() ) );
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
  this->ui_->pcaGroup1Label->setVisible( false );
  this->ui_->pcaGroup2Label->setVisible( false );
  this->ui_->pcaGroupSlider->setVisible( false );
  this->ui_->pcaGroupAnimateCheckBox->setVisible( false );

  this->ui_->tabWidget->setTabEnabled( 3, false ); // regression available

  this->ui_->pcaAnimateCheckBox->setChecked( false );
  this->ui_->pcaGroupAnimateCheckBox->setChecked( false );
  this->ui_->regressionAnimateCheckBox->setChecked( false );

  // update tabs for hiding/showing
  this->ui_->pcaGridLayout->invalidate();

  // switch back to spheres
  ///this->displaySpheres();

  if ( this->ui_->tabWidget->currentWidget() == this->ui_->meanTab )
  {
    std::cerr << "display mean!\n";
    this->visualizer_->display_mean();
    if ( this->ui_->meanGroup1Button->isChecked() )
    {
      //this->displayShape( this->stats.Group1Mean() );
    }
    else if ( this->ui_->meanGroup2Button->isChecked() )
    {
      //this->displayShape( this->stats.Group2Mean() );
    }
    else if ( this->ui_->meanDifferenceButton->isChecked() )
    {
      //this->displayShape( this->stats.Mean() );
      ///this->displayMeanDifference();
    }
    else
    {
      //this->displayShape( this->stats.Mean() );
    }
  }
  else if ( this->ui_->tabWidget->currentWidget() == this->ui_->samplesTab )
  {
/*
    int sampleNumber = this->ui_->sampleSpinBox->value();

    // pre-generate
    for ( int step = 0; step < this->pregenSteps.size(); step++ )
    {
      int pregenSample = sampleNumber + this->pregenSteps[step];
      if ( pregenSample >= this->ui_->sampleSpinBox->minimum() && pregenSample <= this->ui_->sampleSpinBox->maximum() )
      {
        vnl_vector<double> shape = this->stats.ShapeMatrix().get_column( pregenSample );
        for ( int i = 0; i < this->numDomains; i++ )
        {
          this->meshManager.generateMesh( this->getDomainShape( shape, i ) );
        }
      }
    }

    this->displayShape( this->stats.ShapeMatrix().get_column( sampleNumber ) );
 */
  }
  else if ( this->ui_->tabWidget->currentWidget() == this->ui_->pcaTab )
  {
    this->compute_mode_shape();
  }
  else if ( this->ui_->tabWidget->currentWidget() == this->ui_->regressionTab )
  {
    ///this->computeRegressionShape();
  }
}

//---------------------------------------------------------------------------
double AnalysisTool::get_pca_value( int slider_value )
{
  float range = Preferences::Instance().get_pca_range();
  int halfRange = this->ui_->pcaSlider->maximum();

  double value = (double)slider_value / (double)halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaSlider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape()
{
  double pcaSliderValue = this->get_pca_value( this->ui_->pcaSlider->value() );
  int mode = this->ui_->pcaModeSpinBox->value() + 1;
  this->visualizer_->display_pca( mode, pcaSliderValue );
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaModeSpinBox_valueChanged()
{
  this->compute_mode_shape();
}
