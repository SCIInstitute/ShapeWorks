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
