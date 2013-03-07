#include <Preferences.h>

const bool DEFAULT_CACHE_ENABLED = true;
const int DEFAULT_CACHE_MEMORY = 25;
const int DEFAULT_COLOR_SCHEME = 0;
const float DEFAULT_GLYPH_SIZE = 1.0f;
const float DEFAULT_GLYPH_QUALITY = 5.0f;

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
  this->preferencesWindow.setValuesFromPreferences();
  this->preferencesWindow.show();
}

bool Preferences::getCacheEnabled()
{
  return this->settings.value( "ModelCache/enabled", DEFAULT_CACHE_ENABLED ).toBool();
}

void Preferences::setCacheEnabled( bool enabled )
{
  this->settings.setValue( "ModelCache/enabled", enabled );
}

int Preferences::getCacheMemory()
{
  return this->settings.value( "ModelCache/memory", DEFAULT_CACHE_MEMORY ).toInt();
}

void Preferences::setCacheMemory( int value )
{
  this->settings.setValue( "ModelCache/memory", value );
}

int Preferences::getColorScheme()
{
  return this->settings.value( "Visualization/ColorScheme", DEFAULT_COLOR_SCHEME ).toInt();
}

void Preferences::setColorScheme( int value )
{
  this->settings.setValue( "Visualization/ColorScheme", value );
  this->colorSchemeChanged( value );
}

float Preferences::getGlyphSize()
{
  return this->settings.value( "Visualization/GlyphSize", DEFAULT_GLYPH_SIZE ).toFloat();
}

void Preferences::setGlyphSize( float value )
{
  this->settings.setValue( "Visualization/GlyphSize", value );
  this->glyphPropertiesChanged();
}

float Preferences::getGlyphQuality()
{
  return this->settings.value( "Visualization/GlyphQuality", DEFAULT_GLYPH_QUALITY ).toFloat();
}

void Preferences::setGlyphQuality( float value )
{
  this->settings.setValue( "Visualization/GlyphQuality", value );
  this->glyphPropertiesChanged();
}

void Preferences::restoreDefaults()
{
  this->settings.setValue( "ModelCache/enabled", DEFAULT_CACHE_ENABLED );
  this->settings.setValue( "ModelCache/memory", DEFAULT_CACHE_MEMORY );
  this->settings.setValue( "Visualization/ColorScheme", DEFAULT_COLOR_SCHEME );
  this->settings.setValue( "Visualization/GlyphSize", DEFAULT_GLYPH_SIZE );
  this->settings.setValue( "Visualization/GlyphQuality", DEFAULT_GLYPH_QUALITY );
}
