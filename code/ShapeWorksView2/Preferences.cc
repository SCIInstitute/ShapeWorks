#include <Preferences.h>

Preferences& Preferences::Instance()
{
  static Preferences instance;
  return instance;
}

Preferences::Preferences()
  : settings( "Scientific Computing and Imaging Institute", "ShapeWorksView" )
{}

QSettings& Preferences::getSettings()
{
  return this->settings;
}

void Preferences::showWindow()
{
  this->preferencesWindow.show();
}

bool Preferences::cacheEnabled()
{
  return this->settings.value( "ModelCache/enabled", true ).toBool();
}

void Preferences::setCacheEnabled( bool enabled )
{
  this->settings.setValue( "ModelCache/enabled", enabled );
}

int Preferences::cacheMemory()
{
  return this->settings.value( "ModelCache/memory", 25 ).toInt();
}

void Preferences::setCacheMemory( int value )
{
  this->settings.setValue( "ModelCache/memory", value );
}
