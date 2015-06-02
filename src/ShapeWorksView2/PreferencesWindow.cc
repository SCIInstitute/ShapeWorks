/*
 * Shapeworks license
 */

// qt
#include <QtGui>

#include <PreferencesWindow.h>
#include <Preferences.h>

// The interface from the designer
#include "ui_PreferencesWindow.h"

PreferencesWindow::PreferencesWindow(Preferences& prefs, QWidget* parent /*= 0 */ ) : prefs_(prefs)
{
  this->ui = new Ui_PreferencesWindow;
  this->ui->setupUi( this );

  this->ui->numThreadsSlider->setMaximum( QThread::idealThreadCount() );
  this->ui->numThreadsMaxLabel->setText( QString::number( QThread::idealThreadCount() ) );

  QPushButton* resetButton = this->ui->buttonBox->button( QDialogButtonBox::RestoreDefaults );
  QObject::connect( resetButton, SIGNAL( clicked() ), this, SLOT( restoreDefaults() ) );
}


void PreferencesWindow::show_window()
{
  this->setValuesFromPreferences();
  this->show();
}

void PreferencesWindow::close_window()
{
  this->close();
}

void PreferencesWindow::updateLabels()
{
  this->ui->glyphQualityLabel->setText( QString::number( prefs_.getGlyphQuality() ) );
  this->ui->glyphSizeLabel->setText( QString::number( prefs_.getGlyphSize() ) );
}

void PreferencesWindow::on_meshCacheEnabled_stateChanged( int state )
{
  prefs_.setCacheEnabled( this->ui->meshCacheEnabled->isChecked() );
  emit clear_cache();
}

void PreferencesWindow::on_meshCacheMemory_valueChanged( int value )
{
  prefs_.setCacheMemory( value );
}

void PreferencesWindow::on_colorScheme_currentIndexChanged( int index )
{
  prefs_.setColorScheme( index );
  emit update_view();
}

void PreferencesWindow::on_glyphSize_valueChanged( int value )
{
  prefs_.setGlyphSize( value / 10.0 );
  this->updateLabels();
  emit update_view();
}

void PreferencesWindow::on_glyphQuality_valueChanged( int value )
{
  prefs_.setGlyphQuality( value );
  this->updateLabels();
  emit update_view();
}

void PreferencesWindow::on_numThreadsSlider_valueChanged( int value )
{
  prefs_.setNumThreads( value );
}

void PreferencesWindow::on_parallelEnabled_stateChanged( int state )
{
  prefs_.setParallelEnabled( this->ui->parallelEnabled->isChecked() );
}


void PreferencesWindow::on_pcaRangeSpinBox_valueChanged( double value )
{
  prefs_.setPcaRange(this->ui->pcaRangeSpinBox->value());
  emit update_pca();
}

void PreferencesWindow::on_pcaStepsSpinBox_valueChanged( int value )
{
  prefs_.setNumPcaSteps(this->ui->pcaStepsSpinBox->value());
  emit update_pca();
}

void PreferencesWindow::on_regressionStepsSpinBox_valueChanged( int value )
{
  prefs_.setNumRegressionSteps(this->ui->regressionStepsSpinBox->value());
}


void PreferencesWindow::restoreDefaults()
{
  prefs_.restoreDefaults();
  this->setValuesFromPreferences();
}

void PreferencesWindow::setValuesFromPreferences()
{
  this->ui->meshCacheEnabled->setChecked( prefs_.getCacheEnabled() );
  this->ui->meshCacheMemory->setValue( prefs_.getCacheMemory() );
  this->ui->colorScheme->setCurrentIndex( prefs_.getColorScheme() );

  this->ui->glyphQuality->setValue( prefs_.getGlyphQuality() );
  this->ui->glyphSize->setValue( prefs_.getGlyphSize() * 10 );

  this->ui->numThreadsSlider->setValue( prefs_.getNumThreads() );
  this->ui->parallelEnabled->setChecked( prefs_.getParallelEnabled() );

  this->ui->pcaRangeSpinBox->setValue( prefs_.getPcaRange() );
  this->ui->pcaStepsSpinBox->setValue( prefs_.getNumPcaSteps() );
  this->ui->regressionStepsSpinBox->setValue( prefs_.getNumRegressionSteps() );

  this->updateLabels();
}
