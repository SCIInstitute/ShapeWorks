#include <QtGui>
#include <PreferencesWindow.h>
#include <Preferences.h>

// The interface from the designer
#include "ui_PreferencesWindow.h"

PreferencesWindow::PreferencesWindow( QWidget* parent /*= 0 */ )
{
  this->ui = new Ui_PreferencesWindow;
  this->ui->setupUi( this );

  QPushButton* resetButton = this->ui->buttonBox->button( QDialogButtonBox::RestoreDefaults );
  QObject::connect( resetButton, SIGNAL( clicked() ), this, SLOT( restoreDefaults() ) );
}

void PreferencesWindow::updateLabels()
{
  this->ui->glyphQualityLabel->setText( QString::number( Preferences::Instance().getGlyphQuality() ) );
  this->ui->glyphSizeLabel->setText( QString::number( Preferences::Instance().getGlyphSize() ) );
}

void PreferencesWindow::on_meshCacheEnabled_stateChanged( int state )
{
  Preferences::Instance().setCacheEnabled( this->ui->meshCacheEnabled->isChecked() );
}

void PreferencesWindow::on_meshCacheMemory_valueChanged( int value )
{
  Preferences::Instance().setCacheMemory( value );
}

void PreferencesWindow::on_colorScheme_currentIndexChanged( int index )
{
  Preferences::Instance().setColorScheme( index );
}

void PreferencesWindow::on_glyphSize_valueChanged( int value )
{
  //QCoreApplication::processEvents();
  Preferences::Instance().setGlyphSize( value / 10.0 );
  this->updateLabels();
}

void PreferencesWindow::on_glyphQuality_valueChanged( int value )
{
  //QCoreApplication::processEvents();
  Preferences::Instance().setGlyphQuality( value );
  this->updateLabels();
}

void PreferencesWindow::restoreDefaults()
{
  Preferences::Instance().restoreDefaults();
  this->setValuesFromPreferences();
}

void PreferencesWindow::setValuesFromPreferences()
{
  this->ui->meshCacheEnabled->setChecked( Preferences::Instance().getCacheEnabled() );
  this->ui->meshCacheMemory->setValue( Preferences::Instance().getCacheMemory() );
  this->ui->colorScheme->setCurrentIndex( Preferences::Instance().getColorScheme() );

  this->ui->glyphQuality->setValue( Preferences::Instance().getGlyphQuality() );
  this->ui->glyphSize->setValue( Preferences::Instance().getGlyphSize() * 10 );
  this->updateLabels();
}
