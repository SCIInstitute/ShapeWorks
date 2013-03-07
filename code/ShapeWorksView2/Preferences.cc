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
