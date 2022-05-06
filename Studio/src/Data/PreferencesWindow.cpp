// qt
#include <Data/Preferences.h>
#include <Data/PreferencesWindow.h>

#include <QtGui>
#include <cmath>


#include <iostream>


#include <vtkColorSeries.h>
#include <vtkNew.h>

// The interface from the designer
#include "StudioLog.h"
#include "ui_PreferencesWindow.h"

//-----------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow(QWidget* parent, Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_PreferencesWindow;
  ui_->setupUi(this);

  ui_->num_threads->setMaximum(QThread::idealThreadCount());
  ui_->num_threads_max_label->setText(QString::number(QThread::idealThreadCount()));

  QPushButton* reset_button = ui_->button_box->button(QDialogButtonBox::RestoreDefaults);
  QObject::connect(reinterpret_cast<QObject*>(reset_button), SIGNAL(clicked()), this, SLOT(restore_defaults()));

  ui_->log_location->setText(shapeworks::StudioLog::Instance().get_log_filename());

  connect(ui_->orientation_marker_type, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->orientation_marker_corner, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &PreferencesWindow::save_to_preferences);
  connect(ui_->geodesic_cache_multiplier, &QSlider::valueChanged, this, &PreferencesWindow::save_to_preferences);

  vtkNew<vtkColorSeries> colorSeries;
  int colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_3;
  colorSeries->SetColorScheme(colorSeriesEnum);
  //std::cerr << "colors: " << colorSeries->GetColorSchemeName() << "\n";
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged(int state) {
  bool b = ui_->mesh_cache_enabled->isChecked();
  preferences_.set_cache_enabled(b);
  ui_->mesh_cache_memory->setEnabled(b);
  ui_->parallel_enabled->setEnabled(b);
  ui_->num_threads->setEnabled(b);
  emit clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged(int value) {
  preferences_.set_memory_cache_percent(value);
  update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged(int index) {
  preferences_.set_color_scheme(index);
  emit update_view();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged(double value) {
  preferences_.set_pca_range(value);
  emit slider_update();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged(int value) {
  preferences_.set_pca_steps(value);
  emit slider_update();
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
  ui_->num_threads->setValue(preferences_.get_num_threads());
  ui_->parallel_enabled->setChecked(preferences_.get_parallel_enabled());
  ui_->pca_range->setValue(preferences_.get_pca_range());
  ui_->pca_steps->setValue(preferences_.get_pca_steps());
  ui_->orientation_marker_type->setCurrentIndex(preferences_.get_orientation_marker_type());
  ui_->orientation_marker_corner->setCurrentIndex(preferences_.get_orientation_marker_corner());
  ui_->groom_file_template->setText(preferences_.get_groom_file_template());
  ui_->optimize_file_template->setText(preferences_.get_optimize_file_template());
  ui_->geodesic_cache_multiplier->setValue(preferences_.get_geodesic_cache_multiplier());

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
  update_labels();
  emit update_view();
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
