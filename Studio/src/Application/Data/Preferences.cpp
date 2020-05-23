// qt
#include <QThread>
#include <QSize>
#include <Data/Preferences.h>

#include <iostream>

//-----------------------------------------------------------------------------
Preferences::Preferences()
  : settings_("Scientific Computing and Imaging Institute", "ShapeWorksStudio")
{
  this->settings_.setFallbacksEnabled(false);
}

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_geometry()
{
  return this->settings_.value("Studio/window_geometry", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_geometry(QByteArray geometry)
{
  this->settings_.setValue("Studio/window_geometry", geometry);
}

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_state()
{
  return this->settings_.value("Studio/window_state", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_state(QByteArray state)
{
  this->settings_.setValue("Studio/window_state", state);
}

//-----------------------------------------------------------------------------
bool Preferences::get_cache_enabled()
{
  return this->settings_.value("Studio/cache_enabled", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_cache_enabled(bool value)
{
  this->settings_.setValue("Studio/cache_enabled", value);
}

//-----------------------------------------------------------------------------
bool Preferences::get_parallel_enabled()
{
  return this->settings_.value("Studio/parallel_enabled", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_parallel_enabled(bool value)
{
  this->settings_.setValue("Studio/parallel_enabled", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_memory_cache_percent()
{
  return this->settings_.value("Studio/memory_cache_percent", 25).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_memory_cache_percent(int value)
{
  this->settings_.setValue("Studio/memory_cache_percent", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_num_threads()
{
  return this->settings_.value("Studio/num_threads", QThread::idealThreadCount()).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_num_threads(int num_threads)
{
  this->settings_.setValue("Studio/num_threads", num_threads);
}

//-----------------------------------------------------------------------------
float Preferences::get_glyph_size()
{
  return this->settings_.value("Project/glyph_size", 5.0).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_size(float value)
{
  this->settings_.setValue("Project/glyph_size", value);
}

//-----------------------------------------------------------------------------
bool Preferences::not_saved()
{
  return !this->saved_;
}

//-----------------------------------------------------------------------------
void Preferences::set_saved(bool saved)
{
  this->saved_ = saved;
}

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_files()
{
  return this->settings_.value("Main/recentFileList").toStringList();
}

//-----------------------------------------------------------------------------
void Preferences::add_recent_file(QString file)
{
  QStringList files = this->get_recent_files();
  files.removeAll(file);
  files.prepend(file);
  while (files.size() > Preferences::MAX_RECENT_FILES) {
    files.removeLast();
  }

  this->settings_.setValue("Main/recentFileList", files);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults(bool force)
{
  this->set_cache_enabled(true);
  this->set_parallel_enabled(true);
  this->set_memory_cache_percent(25);
  this->set_num_threads(QThread::idealThreadCount());

  this->set_glyph_size(5.0);
  if (!this->settings_.contains("glyph_quality") || force) {
    this->settings_.setValue("glyph_quality", 5.);
  }
  if (!this->settings_.contains("pca_range") || force) {
    this->settings_.setValue("pca_range", 2.f);
  }
  if (!this->settings_.contains("pca_steps") || force) {
    this->settings_.setValue("pca_steps", 20);
  }
  if (!this->settings_.contains("regression_steps") || force) {
    this->settings_.setValue("regression_steps", 50);
  }
  if (!this->settings_.contains("smoothing_amount") || force) {
    this->settings_.setValue("smoothing_amount", 0.);
  }
  if (!this->settings_.contains("cache_epsilon") || force) {
    this->settings_.setValue("cache_epsilon", 1e-3f);
  }
  if (!this->settings_.contains("spacing") || force) {
    this->settings_.setValue("spacing", 9.f);
  }
  if (!this->settings_.contains("neighborhood") || force) {
    this->settings_.setValue("neighborhood", 9);
  }
  if (!this->settings_.contains("color_scheme") || force) {
    this->settings_.setValue("color_scheme", 0);
  }

  this->saved_ = true;
}
