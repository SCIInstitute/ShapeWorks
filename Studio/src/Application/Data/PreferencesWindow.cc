/*
 * Shapeworks license
 */

// qt
#include <QtGui>
#include <cmath>

#include <Data/PreferencesWindow.h>
#include <Data/Preferences.h>
#include <iostream>

// The interface from the designer
#include "ui_PreferencesWindow.h"

//-----------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow(QWidget* parent, Preferences& prefs) : preferences_(prefs) {
  this->ui_ = new Ui_PreferencesWindow;
  this->ui_->setupUi(this);

  this->ui_->num_threads->setMaximum(QThread::idealThreadCount());
  this->ui_->num_threads_max_label->setText(QString::number(QThread::idealThreadCount()));

  QPushButton* reset_button = this->ui_->button_box->button(QDialogButtonBox::RestoreDefaults);
  QObject::connect(reinterpret_cast<QObject*>(reset_button), SIGNAL(clicked()), this, SLOT(restore_defaults()));

}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged(int state) {
  bool b = this->ui_->mesh_cache_enabled->isChecked();
  preferences_.set_preference("cache_enabled", b);
  this->ui_->mesh_cache_memory->setEnabled(b);
  this->ui_->parallel_enabled->setEnabled(b);
  this->ui_->num_threads->setEnabled(b);
  this->ui_->caching_epsilon->setEnabled(b);
  emit clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged(int value) {
  preferences_.set_preference("cache_memory", value);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged(int index) {
  preferences_.set_preference("color_scheme", index);
  emit update_view();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged(double value) {
  preferences_.set_preference("pca_range", value);
  emit slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged(int value) {
  preferences_.set_preference("pca_steps", value);
  emit slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::restore_defaults() {
  preferences_.restore_defaults();
  this->set_values_from_preferences();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::set_values_from_preferences() {
  this->ui_->mesh_cache_enabled->setChecked(preferences_.get_preference("cache_enabled", true));
  this->ui_->mesh_cache_memory->setValue(preferences_.get_preference("cache_memory", 25));
  this->ui_->caching_epsilon->setValue(
    std::log10(preferences_.get_preference("cache_epsilon", 1e-3f)));
  this->ui_->color_scheme->setCurrentIndex(preferences_.get_preference("color_scheme", 0));
  this->ui_->num_threads->setValue(preferences_.get_preference("num_threads", QThread::idealThreadCount()));
  this->ui_->parallel_enabled->setChecked(preferences_.get_preference("parallel_enabled", true));
  this->ui_->pca_range->setValue(preferences_.get_preference("pca_range", 2.f));
  this->ui_->pca_steps->setValue(preferences_.get_preference("pnum_pca_steps", 20));
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_toggled(bool b) {
  preferences_.set_preference("parallel_enabled", b);
  this->ui_->num_threads->setEnabled(b);
}
//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged(int i) {
  preferences_.set_preference("num_threads", i);
}
//-----------------------------------------------------------------------------
void PreferencesWindow::on_caching_epsilon_valueChanged(int i) {
  preferences_.set_preference("cache_epsilon", std::pow(10., static_cast<double>(i)));
  emit clear_cache();
}
