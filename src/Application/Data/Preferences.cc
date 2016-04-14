/*
 * Shapeworks license
 */

// qt
#include <QThread>
#include <QSize>
#include <Data/Preferences.h>

//-----------------------------------------------------------------------------
Preferences::Preferences(QString name)
  : settings_( "Scientific Computing and Imaging Institute", name ), saved_(true) {
  this->defaults_.insert(std::make_pair("cache_enabled", true));
  this->defaults_.insert(std::make_pair("parallel_enabled", true));
  this->defaults_.insert(std::make_pair("cache_memory", 25));
  this->defaults_.insert(std::make_pair("glyph_size", 1.));
  this->defaults_.insert(std::make_pair("glyph_quality", 5.));
  this->defaults_.insert(std::make_pair("num_threads", 100));
  this->defaults_.insert(std::make_pair("pca_range", 2.f));
  this->defaults_.insert(std::make_pair("pca_steps", 40));
  this->defaults_.insert(std::make_pair("regression_steps", 50));
  this->defaults_.insert(std::make_pair("smoothing_amount", 0.));
  this->defaults_.insert(std::make_pair("cache_epsilon", 1e-3f));
  this->defaults_.insert(std::make_pair("spacing", 1.f));
  this->defaults_.insert(std::make_pair("neighborhood", 5));
  this->defaults_.insert(std::make_pair("color_scheme", 0));
  this->defaults_.insert(std::make_pair("groom_start_reg", 100));
  this->defaults_.insert(std::make_pair("groom_scales", 8));
  this->defaults_.insert(std::make_pair("groom_end_reg", 2));
  this->defaults_.insert(std::make_pair("groom_iters", 1000));
  this->defaults_.insert(std::make_pair("groom_tolerance", 0.01));
  this->defaults_.insert(std::make_pair("groom_decay_span", 0.0));
}

Preferences& Preferences::operator = (const Preferences& o) {
  if (this != &o) { // self-assignment check expected
    this->saved_ = o.saved_;
    this->defaults_ = o.defaults_; 
    QStringList keys = o.settings_.allKeys();
    for (QStringList::iterator i = keys.begin(); i != keys.end(); i++)
    {
      this->settings_.setValue(*i, o.settings_.value(*i));
    }
  }
  return *this;
}

bool Preferences::not_saved() {
  return !this->saved_;
}

void Preferences::set_saved() {
  this->saved_ = true;
}

QStringList Preferences::get_recent_files()
{
  return this->settings_.value("General/RecentFileList").toStringList();
}

void Preferences::add_recent_file(QString file)
{
  QStringList files = this->get_recent_files();
  files.removeAll(file);
  files.prepend(file);
  while (files.size() > Preferences::MAX_RECENT_FILES)
  {
    files.removeLast();
  }

  this->settings_.setValue("General/RecentFileList", files);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults() {
  for (auto &a : this->defaults_) {
    this->settings_.setValue(QString::fromStdString(a.first), a.second);
  }
  this->saved_ = true;
}
