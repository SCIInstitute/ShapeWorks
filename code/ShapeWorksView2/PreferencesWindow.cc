#include <PreferencesWindow.h>
#include <Preferences.h>

// The interface from the designer
#include "ui_PreferencesWindow.h"

PreferencesWindow::PreferencesWindow( QWidget* parent /*= 0 */ )
{
  this->ui = new Ui_PreferencesWindow;
  this->ui->setupUi( this );

  this->ui->modelCacheEnabled->setChecked(
    Preferences::Instance().getSettings().value( "ModelCache/enabled", true ).toBool() );

  this->ui->modelCacheMemory->setValue(
    Preferences::Instance().getSettings().value( "ModelCache/memory", 25 ).toInt() );
}

void PreferencesWindow::on_modelCacheEnabled_stateChanged( int state )
{
  Preferences::Instance().getSettings().setValue(
    "ModelCache/enabled", this->ui->modelCacheEnabled->isChecked() );
}

void PreferencesWindow::on_modelCacheMemory_valueChanged( int value )
{
  Preferences::Instance().getSettings().setValue(
    "ModelCache/memory", value );
}
