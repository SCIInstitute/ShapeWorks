// qt
#include <QThread>
#include <QSize>
#include <Data/Preferences.h>

#include <iostream>

//-----------------------------------------------------------------------------
Preferences::Preferences()
  : settings_("Scientific Computing and Imaging Institute", "ShapeWorksStudio")
{
  this->restore_defaults();
  this->settings_.setFallbacksEnabled(false);
}

//-----------------------------------------------------------------------------
std::map<std::string, QVariant> Preferences::get_project_preferences()
{
  std::map<std::string, QVariant> ans;
  for (QString &a : this->settings_.allKeys()) {
    QString key = a;
    if (a.toStdString().find("Project/") != std::string::npos) {
      ans.insert(std::make_pair(a.remove("Project/").toStdString(), this->settings_.value(key)));
      std::cerr << "a = " << key.toStdString() << "\n";
    }
  }
  return ans;
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
bool Preferences::has_entry(QString name)
{
  return this->settings_.contains(name);
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
  if (!this->settings_.contains("groom_center") || force) {
    this->settings_.setValue("groom_center", true);
  }
  if (!this->settings_.contains("groom_antialias") || force) {
    this->settings_.setValue("groom_antialias", true);
  }
  if (!this->settings_.contains("groom_pad") || force) {
    this->settings_.setValue("groom_pad", true);
  }
  if (!this->settings_.contains("groom_fastmarching") || force) {
    this->settings_.setValue("groom_fastmarching", false);
  }
  if (!this->settings_.contains("groom_blur") || force) {
    this->settings_.setValue("groom_blur", true);
  }
  if (!this->settings_.contains("groom_crop") || force) {
    this->settings_.setValue("groom_crop", false);
  }
  if (!this->settings_.contains("groom_isolate") || force) {
    this->settings_.setValue("groom_isolate", true);
  }
  if (!this->settings_.contains("groom_fill_holes") || force) {
    this->settings_.setValue("groom_fill_holes", true);
  }
  if (!this->settings_.contains("groom_antialias_amount") || force) {
    this->settings_.setValue("groom_antialias_amount", 100);
  }
  if (!this->settings_.contains("groom_fastmarching_isovalue") || force) {
    this->settings_.setValue("groom_fastmarching_isovalue", 0.5);
  }
  if (!this->settings_.contains("groom_fastmarching_sigma") || force) {
    this->settings_.setValue("groom_fastmarching_sigma", 2.0);
  }
  if (!this->settings_.contains("groom_blur_sigma") || force) {
    this->settings_.setValue("groom_blur_sigma", 2.0);
  }
  if (!this->settings_.contains("groom_pad_value") || force) {
    this->settings_.setValue("groom_pad_value", 5);
  }
  if (!this->settings_.contains("optimize_start_reg") || force) {
    this->settings_.setValue("optimize_start_reg", 1000);
  }
  if (!this->settings_.contains("optimize_particles") || force) {
    this->settings_.setValue("optimize_particles", 128);
  }
  if (!this->settings_.contains("optimize_decimation") || force) {
    this->settings_.setValue("optimize_decimation", 0.3);
  }
  if (!this->settings_.contains("optimize_end_reg") || force) {
    this->settings_.setValue("optimize_end_reg", 10);
  }
  if (!this->settings_.contains("optimize_iters") || force) {
    this->settings_.setValue("optimize_iters", 1000);
  }
  if (!this->settings_.contains("optimize_iters_optimization") || force) {
    this->settings_.setValue("optimize_iters_optimization", 1000);
  }
  if (!this->settings_.contains("optimize_weight") || force) {
    this->settings_.setValue("optimize_weight", 1.0);
  }
  if (!this->settings_.contains("optimize_procrustes_interval") || force) {
    this->settings_.setValue("optimize_procrustes_interval", 0);
  }
  if (!this->settings_.contains("optimize_maxAngle") || force) {
    this->settings_.setValue("optimize_maxAngle", 30.);
  }
  if (!this->settings_.contains("optimize_clusters") || force) {
    this->settings_.setValue("optimize_clusters", 5);
  }
  this->saved_ = true;
}

//-----------------------------------------------------------------------------
void Preferences::delete_entry(std::string entry)
{
  auto allPref = this->get_project_preferences();
  std::vector<std::string> toRemove;
  for (auto &p : allPref) {
    if (p.first.rfind(entry) != std::string::npos) {
      this->settings_.remove(QString::fromStdString(p.first));
    }
  }
  allPref = this->get_project_preferences();
  this->saved_ = false;
}
