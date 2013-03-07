#include <PreferencesWindow.h>
#include <Preferences.h>

// The interface from the designer
#include "ui_PreferencesWindow.h"

PreferencesWindow::PreferencesWindow( QWidget* parent /*= 0 */ )
{
  this->ui = new Ui_PreferencesWindow;
  this->ui->setupUi( this );

  this->ui->modelCacheEnabled->setChecked( Preferences::Instance().cacheEnabled() );
  this->ui->modelCacheMemory->setValue( Preferences::Instance().cacheMemory() );
  this->ui->colorScheme->setCurrentIndex( Preferences::Instance().colorScheme() );

  this->ui->glyphQuality->setValue( Preferences::Instance().getGlyphQuality() );
  this->ui->glyphSize->setValue( Preferences::Instance().getGlyphSize() * 10 );
  this->updateLabels();
}

void PreferencesWindow::updateLabels()
{
  this->ui->glyphQualityLabel->setText( QString::number( Preferences::Instance().getGlyphQuality() ) );
  this->ui->glyphSizeLabel->setText( QString::number( Preferences::Instance().getGlyphSize() ) );
}

void PreferencesWindow::on_modelCacheEnabled_stateChanged( int state )
{
  Preferences::Instance().setCacheEnabled( this->ui->modelCacheEnabled->isChecked() );
}

void PreferencesWindow::on_modelCacheMemory_valueChanged( int value )
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
