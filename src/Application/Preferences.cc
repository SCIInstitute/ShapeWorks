/*
 * Shapeworks license
 */

// qt
#include <QThread>
#include <QSize>

#include <Application/Preferences.h>

// default values
const bool DEFAULT_CACHE_ENABLED = true;
const bool DEFAULT_PARALLEL_ENABLED = true;
const int DEFAULT_CACHE_MEMORY = 25;
const int DEFAULT_COLOR_SCHEME = 0;
const float DEFAULT_GLYPH_SIZE = 1.0f;
const float DEFAULT_GLYPH_QUALITY = 5.0f;
const float DEFAULT_NUM_THREADS = 100.0f;
const float DEFAULT_PCA_RANGE = 2.0f;
const int DEFAULT_PCA_STEPS = 40;
const int DEFAULT_REGRESSION_STEPS = 50;
const float DEFAULT_SMOOTHING_AMOUNT = 0.f;
const float DEFAULT_CACHE_EPSILON = 1e-3f;
const float DEFAULT_SPACING = 1.f;
const int DEFAULT_NEIGHBORHOOD = 5;

//-----------------------------------------------------------------------------
Preferences::Preferences()
  : settings( "Scientific Computing and Imaging Institute", "ShapeWorksStudio" )
{}

//-----------------------------------------------------------------------------
QSize Preferences::get_main_window_size()
{
  return this->settings.value( "MainWindow/Size", QSize( 1280, 720 ) ).toSize();
}

//-----------------------------------------------------------------------------
void Preferences::set_main_window_size( QSize size )
{
  this->settings.setValue( "MainWindow/Size", size );
}

//-----------------------------------------------------------------------------
QString Preferences::get_groom_location()
{
  return this->settings.value( "General/GroomLocation", "" ).toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_groom_location( QString location )
{
  this->settings.setValue( "General/GroomLocation", location );
}

//-----------------------------------------------------------------------------
QString Preferences::get_optimize_location()
{
  return this->settings.value( "General/OptimizeLocation", "" ).toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_optimize_location( QString location )
{
  this->settings.setValue( "General/OptimizeLocation", location );
}

//-----------------------------------------------------------------------------
QString Preferences::get_last_directory()
{
  return this->settings.value( "General/LastDirectory", "" ).toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_last_directory( QString location )
{
  this->settings.setValue( "General/LastDirectory", location );
}

//-----------------------------------------------------------------------------
bool Preferences::get_cache_enabled()
{
  return this->settings.value( "MeshCache/Enabled", DEFAULT_CACHE_ENABLED ).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_cache_enabled( bool enabled )
{
  this->settings.setValue( "MeshCache/Enabled", enabled );
}

//-----------------------------------------------------------------------------
int Preferences::get_cache_memory()
{
  return this->settings.value( "MeshCache/Memory", DEFAULT_CACHE_MEMORY ).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::setCacheMemory( int value )
{
  this->settings.setValue( "MeshCache/Memory", value );
}

//-----------------------------------------------------------------------------
int Preferences::get_color_scheme()
{
  return this->settings.value( "Visualization/ColorScheme", DEFAULT_COLOR_SCHEME ).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_color_scheme( int value )
{
  this->settings.setValue( "Visualization/ColorScheme", value );
  this->color_scheme_changed( value );
}

//-----------------------------------------------------------------------------
float Preferences::get_glyph_size()
{
  return this->settings.value( "Visualization/GlyphSize", DEFAULT_GLYPH_SIZE ).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_size( float value )
{
  this->settings.setValue( "Visualization/GlyphSize", value );
  this->glyph_properties_changed();
}

//-----------------------------------------------------------------------------
float Preferences::get_glyph_quality()
{
  return this->settings.value( "Visualization/GlyphQuality", DEFAULT_GLYPH_QUALITY ).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_quality( float value )
{
  this->settings.setValue( "Visualization/GlyphQuality", value );
  this->glyph_properties_changed();
}

//-----------------------------------------------------------------------------
int Preferences::get_num_threads()
{
  // the number of threads is stored as a percentage of the available threads.  This way, if you
  // upgrade ot move to a different machine and had it set at all available threads, it will still be that way
  float num_threads_ratio = this->settings.value( "MeshCache/NumThreads", DEFAULT_NUM_THREADS ).toFloat() / 100;
  int num_threads = (float)QThread::idealThreadCount() * num_threads_ratio;
  return num_threads;
}

//-----------------------------------------------------------------------------
void Preferences::set_num_threads( int value )
{
  float ratio = (float) value / (float) QThread::idealThreadCount() * 100.0f;
  this->settings.setValue( "MeshCache/NumThreads", ratio );
  emit this->threading_changed_signal();
}

//-----------------------------------------------------------------------------
bool Preferences::get_parallel_enabled()
{
  return this->settings.value( "MeshCache/Parallel", DEFAULT_PARALLEL_ENABLED ).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_parallel_enabled( bool enabled )
{
  this->settings.setValue( "MeshCache/Parallel", enabled );
  emit this->threading_changed_signal();
}

//-----------------------------------------------------------------------------
float Preferences::get_pca_range()
{
  return this->settings.value( "Sliders/PcaRange", DEFAULT_PCA_RANGE ).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_pca_range( float value )
{
  this->settings.setValue( "Sliders/PcaRange", value );
  emit this->sliders_changed_signal();
}

//-----------------------------------------------------------------------------
int Preferences::get_num_pca_steps()
{
  return this->settings.value( "Sliders/PcaSteps", DEFAULT_PCA_STEPS ).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_num_pca_steps( int value )
{
  this->settings.setValue( "Sliders/PcaSteps", value );
  emit this->sliders_changed_signal();
}

//-----------------------------------------------------------------------------
int Preferences::get_num_regression_rteps()
{
  return this->settings.value( "Sliders/RegressionSteps", DEFAULT_REGRESSION_STEPS ).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_num_regression_steps( int value )
{
  this->settings.setValue( "Sliders/RegressionSteps", value );
  emit this->sliders_changed_signal();
}

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_files()
{
  return this->settings.value( "General/RecentFileList" ).toStringList();
}

//-----------------------------------------------------------------------------
void Preferences::add_recent_file( QString file )
{
  QStringList files = this->get_recent_files();
  files.removeAll( file );
  files.prepend( file );
  while ( files.size() > Preferences::MAX_RECENT_FILES )
  {
    files.removeLast();
  }

  settings.setValue( "General/RecentFileList", files );
}

float Preferences::get_smoothing_amount() {
	return this->settings.value( "Mesh/SmoothingAmount", DEFAULT_SMOOTHING_AMOUNT ).toFloat();
}

void Preferences::set_smoothing_amount(float b) {
  this->settings.setValue( "Mesh/SmoothingAmount", b );
}
float Preferences::get_cache_epsilon() {
	return this->settings.value( "Mesh/CachingEpsilon", DEFAULT_CACHE_EPSILON ).toFloat();
}
void Preferences::set_cache_epsilon(float value) {
  this->settings.setValue( "Mesh/CachingEpsilon", value );
}

float Preferences::get_spacing() {
	return this->settings.value( "Mesh/Spacing", DEFAULT_SPACING ).toFloat();
}
void Preferences::set_spacing(float value) {
  this->settings.setValue( "Mesh/Spacing", value );
}

int Preferences::get_neighborhood() {
	return this->settings.value( "Mesh/Neighborhood", DEFAULT_NEIGHBORHOOD ).toInt();
}
void Preferences::set_neighborhood(int value) {
  this->settings.setValue( "Mesh/Neighborhood", value );
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults()
{
  this->settings.setValue( "MeshCache/Enabled", DEFAULT_CACHE_ENABLED );
  this->settings.setValue( "MeshCache/Memory", DEFAULT_CACHE_MEMORY );
  this->settings.setValue( "MeshCache/NumThreads", DEFAULT_NUM_THREADS );
  this->settings.setValue( "MeshCache/Parallel", DEFAULT_PARALLEL_ENABLED );
  this->settings.setValue( "Visualization/ColorScheme", DEFAULT_COLOR_SCHEME );
  this->settings.setValue( "Visualization/GlyphSize", DEFAULT_GLYPH_SIZE );
  this->settings.setValue( "Visualization/GlyphQuality", DEFAULT_GLYPH_QUALITY );
  this->settings.setValue( "Sliders/PcaRange", DEFAULT_PCA_RANGE );
  this->settings.setValue( "Sliders/PcaSteps", DEFAULT_PCA_STEPS );
  this->settings.setValue( "Sliders/RegressionSteps", DEFAULT_REGRESSION_STEPS );
  this->settings.setValue( "Mesh/SmoothingAmount", DEFAULT_SMOOTHING_AMOUNT );
  this->settings.setValue( "Mesh/CachingEpsilon", DEFAULT_CACHE_EPSILON );
}
