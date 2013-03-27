/*
 * Shapeworks license
 */

// qt
#include <QThread.h>

#include <Preferences.h>

// default values
const bool DEFAULT_CACHE_ENABLED = true;
const bool DEFAULT_PARALLEL_ENABLED = true;
const int DEFAULT_CACHE_MEMORY = 25;
const int DEFAULT_COLOR_SCHEME = 0;
const float DEFAULT_GLYPH_SIZE = 1.0f;
const float DEFAULT_GLYPH_QUALITY = 5.0f;
const float DEFAULT_NUM_THREADS = 100.0f;

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

void Preferences::closeWindow()
{
  this->preferencesWindow.close();
}

bool Preferences::getCacheEnabled()
{
  return this->settings.value( "MeshCache/Enabled", DEFAULT_CACHE_ENABLED ).toBool();
}

void Preferences::setCacheEnabled( bool enabled )
{
  this->settings.setValue( "MeshCache/Enabled", enabled );
}

int Preferences::getCacheMemory()
{
  return this->settings.value( "MeshCache/Memory", DEFAULT_CACHE_MEMORY ).toInt();
}

void Preferences::setCacheMemory( int value )
{
  this->settings.setValue( "MeshCache/Memory", value );
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

int Preferences::getNumThreads()
{
  // the number of threads is stored as a percentage of the available threads.  This way, if you
  // upgrade ot move to a different machine and had it set at all available threads, it will still be that way
  float numThreadsRatio = this->settings.value( "MeshCache/NumThreads", DEFAULT_NUM_THREADS ).toFloat() / 100;
  int numThreads = (float)QThread::idealThreadCount() * numThreadsRatio;
  return numThreads;
}

void Preferences::setNumThreads( int value )
{
  float ratio = (float) value / (float) QThread::idealThreadCount() * 100.0f;
  this->settings.setValue( "MeshCache/NumThreads", ratio );
  this->threadingChangedSignal();
}

bool Preferences::getParallelEnabled()
{
  return this->settings.value( "MeshCache/Parallel", DEFAULT_PARALLEL_ENABLED ).toBool();
}

void Preferences::setParallelEnabled( bool enabled )
{
  this->settings.setValue( "MeshCache/Parallel", enabled );
  this->threadingChangedSignal();
}

void Preferences::restoreDefaults()
{
  this->settings.setValue( "MeshCache/Enabled", DEFAULT_CACHE_ENABLED );
  this->settings.setValue( "MeshCache/Memory", DEFAULT_CACHE_MEMORY );
  this->settings.setValue( "MeshCache/NumThreads", DEFAULT_NUM_THREADS );
  this->settings.setValue( "MeshCache/Parallel", DEFAULT_PARALLEL_ENABLED );
  this->settings.setValue( "Visualization/ColorScheme", DEFAULT_COLOR_SCHEME );
  this->settings.setValue( "Visualization/GlyphSize", DEFAULT_GLYPH_SIZE );
  this->settings.setValue( "Visualization/GlyphQuality", DEFAULT_GLYPH_QUALITY );
}
