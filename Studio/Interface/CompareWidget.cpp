#include "CompareWidget.h"

#include "ui_CompareWidget.h"

namespace shapeworks {

//---------------------------------------------------------------------------
CompareWidget::CompareWidget(QWidget *parent) : QWidget(parent), ui_(new Ui::CompareWidget) {
  ui_->setupUi(this);

  connect(ui_->surface_distance, &QRadioButton::toggled, this, &CompareWidget::settings_changed);
  connect(ui_->original, &QCheckBox::toggled, this, &CompareWidget::settings_changed);
  connect(ui_->groomed, &QCheckBox::toggled, this, &CompareWidget::settings_changed);
  connect(ui_->reconstructed, &QCheckBox::toggled, this, &CompareWidget::settings_changed);
  connect(ui_->opacity, &CustomSlider::valueChanged, this, &CompareWidget::settings_changed);
  connect(ui_->mean_shape, &QCheckBox::toggled, this, &CompareWidget::settings_changed);
}

//---------------------------------------------------------------------------
CompareWidget::~CompareWidget() { delete ui_; }

//---------------------------------------------------------------------------
CompareSettings CompareWidget::get_settings() {
  CompareSettings settings;
  settings.surface_distance_mode_ = ui_->surface_distance->isChecked();
  settings.original_checked_ = ui_->original->isChecked();
  settings.groomed_checked_ = ui_->groomed->isChecked();
  settings.reconstructed_checked_ = ui_->reconstructed->isChecked();
  settings.mean_shape_checked_ = ui_->mean_shape->isChecked();
  settings.opacity_ = ui_->opacity->value() / 100.0;
  return settings;
}

//---------------------------------------------------------------------------
void CompareWidget::set_available(DisplayMode mode, bool available) {
  if (mode == DisplayMode::Original) {
    ui_->original->setEnabled(available);
  } else if (mode == DisplayMode::Groomed) {
    ui_->groomed->setEnabled(available);
  } else if (mode == DisplayMode::Reconstructed) {
    ui_->reconstructed->setEnabled(available);
  }
}

//---------------------------------------------------------------------------
bool CompareWidget::check_any_available() {
  bool any_available = ui_->original->isEnabled() || ui_->groomed->isEnabled() || ui_->reconstructed->isEnabled();

  if (any_available) {
    // cycle through to make sure we land on a valid selection

    if (ui_->original->isChecked() && !ui_->original->isEnabled()) {
      ui_->groomed->setChecked(true);
    }
    if (ui_->groomed->isChecked() && !ui_->groomed->isEnabled()) {
      ui_->reconstructed->setChecked(true);
    }
    if (ui_->reconstructed->isChecked() && !ui_->reconstructed->isEnabled()) {
      ui_->original->setChecked(true);
    }
    if (ui_->original->isChecked() && !ui_->original->isEnabled()) {
      ui_->groomed->setChecked(true);
    }
  }

  return any_available;
}

//---------------------------------------------------------------------------
void CompareWidget::set_mean_shape_available(bool available) {
  ui_->mean_shape->setEnabled(available);
}

}  // namespace shapeworks
