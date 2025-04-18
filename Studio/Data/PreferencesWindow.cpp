// qt
#include <Data/Preferences.h>
#include <Data/PreferencesWindow.h>
#include <Logging.h>
#include <Visualization/ColorMap.h>
#include <Visualization/ParticleColors.h>
#include <vtkColorSeries.h>
#include <vtkNew.h>

#include <QtGui>
#include <cmath>
#include <iostream>

#include "ui_PreferencesWindow.h"

namespace shapeworks {

//-----------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow(QWidget* parent, Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_PreferencesWindow;
  ui_->setupUi(this);

  ui_->num_threads->setMaximum(QThread::idealThreadCount());
  ui_->num_threads_max_label->setText(QString::number(QThread::idealThreadCount()));

  QPushButton* reset_button = ui_->button_box->button(QDialogButtonBox::RestoreDefaults);
  QObject::connect(reinterpret_cast<QObject*>(reset_button), SIGNAL(clicked()), this, SLOT(restore_defaults()));

  ui_->log_location->setText(QString::fromStdString(shapeworks::Logging::Instance().get_log_filename()));

  ui_->color_map->clear();
  ColorMaps maps;
  Q_FOREACH (auto color_map, maps) {
    ui_->color_map->addItem(color_map.name_);
  }

  ui_->particle_colors->clear();
  QMetaEnum e = QMetaEnum::fromType<ParticleColors::ParticleColorsType>();
  for (int i = 0; i < e.keyCount(); i++) {
    ui_->particle_colors->addItem(e.key(i));
  }

  set_values_from_preferences();

  connect(ui_->orientation_marker_type, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->orientation_marker_corner, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->geodesic_cache_multiplier, &CustomSlider::valueChanged, this, &PreferencesWindow::save_to_preferences);
  connect(ui_->color_map, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->particle_colors, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->discrete_color_mode, &QCheckBox::toggled, this, &PreferencesWindow::save_to_preferences);
  connect(ui_->reverse_color_map, &QCheckBox::toggled, this, &PreferencesWindow::save_to_preferences);
  connect(ui_->auto_update_checkbox, &QCheckBox::toggled, this, &PreferencesWindow::save_to_preferences);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged(int state) {
  bool b = ui_->mesh_cache_enabled->isChecked();
  preferences_.set_cache_enabled(b);
  ui_->mesh_cache_memory->setEnabled(b);
  ui_->parallel_enabled->setEnabled(b);
  ui_->num_threads->setEnabled(b);
  Q_EMIT clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged(int value) {
  preferences_.set_memory_cache_percent(value);
  update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged(int index) {
  preferences_.set_color_scheme(index);
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged(double value) {
  preferences_.set_pca_range(value);
  Q_EMIT slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged(int value) {
  preferences_.set_pca_steps(value);
  Q_EMIT slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::restore_defaults() {
  preferences_.restore_defaults();
  set_values_from_preferences();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::set_values_from_preferences() {
  ui_->mesh_cache_enabled->setChecked(preferences_.get_cache_enabled());
  ui_->mesh_cache_memory->setValue(preferences_.get_memory_cache_percent());
  ui_->color_scheme->setCurrentIndex(preferences_.get_color_scheme());
  ui_->color_map->setCurrentIndex(preferences_.get_color_map());
  ui_->particle_colors->setCurrentIndex(preferences_.get_particle_colors());
  ui_->reverse_color_map->setChecked(preferences_.get_reverse_color_map());
  ui_->discrete_color_mode->setChecked(preferences_.get_discrete_color_mode());
  ui_->num_threads->setValue(preferences_.get_num_threads());
  ui_->parallel_enabled->setChecked(preferences_.get_parallel_enabled());
  ui_->pca_range->setValue(preferences_.get_pca_range());
  ui_->pca_steps->setValue(preferences_.get_pca_steps());
  ui_->orientation_marker_type->setCurrentIndex(preferences_.get_orientation_marker_type());
  ui_->orientation_marker_corner->setCurrentIndex(preferences_.get_orientation_marker_corner());
  ui_->groom_file_template->setText(preferences_.get_groom_file_template());
  ui_->optimize_file_template->setText(preferences_.get_optimize_file_template());
  ui_->geodesic_cache_multiplier->setValue(preferences_.get_geodesic_cache_multiplier());
  ui_->auto_update_checkbox->setChecked(preferences_.get_auto_update_check());
  ui_->telemetry_enabled->setChecked(preferences_.get_telemetry_enabled());
  update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_toggled(bool b) {
  preferences_.set_parallel_enabled(b);
  ui_->num_threads->setEnabled(b);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged(int i) {
  preferences_.set_num_threads(i);
  update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::save_to_preferences() {
  preferences_.set_orientation_marker_type(
      static_cast<Preferences::OrientationMarkerType>(ui_->orientation_marker_type->currentIndex()));
  preferences_.set_orientation_marker_corner(
      static_cast<Preferences::OrientationMarkerCorner>(ui_->orientation_marker_corner->currentIndex()));

  preferences_.set_groom_file_template(ui_->groom_file_template->text());
  preferences_.set_optimize_file_template(ui_->optimize_file_template->text());
  preferences_.set_geodesic_cache_multiplier(ui_->geodesic_cache_multiplier->value());
  preferences_.set_color_map(ui_->color_map->currentIndex());
  preferences_.set_particle_colors(ui_->particle_colors->currentIndex());
  preferences_.set_discrete_color_mode(ui_->discrete_color_mode->isChecked());
  preferences_.set_reverse_color_map(ui_->reverse_color_map->isChecked());
  preferences_.set_auto_update_check(ui_->auto_update_checkbox->isChecked());
  preferences_.set_telemetry_enabled(ui_->telemetry_enabled->isChecked());
  update_labels();
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::closeEvent(QCloseEvent* event) {
  save_to_preferences();
  QDialog::closeEvent(event);
}

//-----------------------------------------------------------------------------
void PreferencesWindow::update_labels() {
  ui_->threads_current_value->setText(QString::number(ui_->num_threads->value()));
  ui_->memory_current_value->setText(QString::number(ui_->mesh_cache_memory->value()) + "%");
  ui_->geodesic_cache_multiplier_current_value->setText(QString::number(ui_->geodesic_cache_multiplier->value()));
}

//-----------------------------------------------------------------------------
void PreferencesWindow::accept() {
  save_to_preferences();
  QDialog::accept();
}

}  // namespace shapeworks
