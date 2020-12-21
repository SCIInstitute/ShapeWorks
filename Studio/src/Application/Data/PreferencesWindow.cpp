// qt
#include <QtGui>
#include <cmath>

#include <Data/PreferencesWindow.h>
#include <Data/Preferences.h>
#include <iostream>

// The interface from the designer
#include "ui_PreferencesWindow.h"
#include "StudioLog.h"

//-----------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow(QWidget* parent, Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_PreferencesWindow;
  this->ui_->setupUi(this);

  this->ui_->num_threads->setMaximum(QThread::idealThreadCount());
  this->ui_->num_threads_max_label->setText(QString::number(QThread::idealThreadCount()));

  QPushButton* reset_button = this->ui_->button_box->button(QDialogButtonBox::RestoreDefaults);
  QObject::connect(reinterpret_cast<QObject*>(reset_button), SIGNAL(clicked()), this,
                   SLOT(restore_defaults()));

  this->ui_->log_location->setText(shapeworks::StudioLog::Instance().get_log_filename());

  this->connect(this->ui_->orientation_marker_type, qOverload<int>(&QComboBox::currentIndexChanged),
                this, &PreferencesWindow::save_to_preferences);
  this->connect(this->ui_->orientation_marker_corner, qOverload<int>(&QComboBox::currentIndexChanged),
                this, &PreferencesWindow::save_to_preferences);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged(int state)
{
  bool b = this->ui_->mesh_cache_enabled->isChecked();
  this->preferences_.set_cache_enabled(b);
  this->ui_->mesh_cache_memory->setEnabled(b);
  this->ui_->parallel_enabled->setEnabled(b);
  this->ui_->num_threads->setEnabled(b);
  this->ui_->caching_epsilon->setEnabled(b);
  emit clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged(int value)
{
  preferences_.set_memory_cache_percent(value);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged(int index)
{
  this->preferences_.set_color_scheme(index);
  emit update_view();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged(double value)
{
  this->preferences_.set_pca_range(value);
  emit slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged(int value)
{
  this->preferences_.set_pca_steps(value);
  emit slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::restore_defaults()
{
  preferences_.restore_defaults();
  this->set_values_from_preferences();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::set_values_from_preferences()
{
  this->ui_->mesh_cache_enabled->setChecked(preferences_.get_cache_enabled());
  this->ui_->mesh_cache_memory->setValue(preferences_.get_memory_cache_percent());
  this->ui_->caching_epsilon->setValue(std::log10(this->preferences_.get_cache_epsilon()));
  this->ui_->color_scheme->setCurrentIndex(preferences_.get_color_scheme());
  this->ui_->num_threads->setValue(preferences_.get_num_threads());
  this->ui_->parallel_enabled->setChecked(preferences_.get_parallel_enabled());
  this->ui_->pca_range->setValue(preferences_.get_pca_range());
  this->ui_->pca_steps->setValue(preferences_.get_pca_steps());
  this->ui_->orientation_marker_type->setCurrentIndex(preferences_.get_orientation_marker_type());
  this->ui_->orientation_marker_corner->setCurrentIndex(
    preferences_.get_orientation_marker_corner());
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_toggled(bool b)
{
  this->preferences_.set_parallel_enabled(b);
  this->ui_->num_threads->setEnabled(b);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged(int i)
{
  this->preferences_.set_num_threads(i);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_caching_epsilon_valueChanged(int i)
{
  this->preferences_.set_cache_epsilon(std::pow(10., static_cast<double>(i)));
  emit clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::save_to_preferences()
{
  this->preferences_.set_orientation_marker_type(
    static_cast<Preferences::OrientationMarkerType>(this->ui_->orientation_marker_type->currentIndex()));
  this->preferences_.set_orientation_marker_corner(
    static_cast<Preferences::OrientationMarkerCorner>(this->ui_->orientation_marker_corner->currentIndex()));
  emit update_view();
}
