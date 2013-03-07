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

int Preferences::colorScheme()
{
  return this->settings.value( "Visualization/ColorScheme", 0 ).toInt();
}

void Preferences::setColorScheme( int value )
{
  this->settings.setValue( "Visualization/ColorScheme", value );
  this->colorSchemeChanged( value );
}

float Preferences::getGlyphSize()
{
  return this->settings.value( "Visualization/GlyphSize", 1.0f ).toFloat();
}

void Preferences::setGlyphSize( float value )
{
  this->settings.setValue( "Visualization/GlyphSize", value );
  this->glyphPropertiesChanged();
}

float Preferences::getGlyphQuality()
{
  return this->settings.value( "Visualization/GlyphQuality", 5.0f ).toFloat();
}

void Preferences::setGlyphQuality( float value )
{
  this->settings.setValue( "Visualization/GlyphQuality", value );
  this->glyphPropertiesChanged();
}
