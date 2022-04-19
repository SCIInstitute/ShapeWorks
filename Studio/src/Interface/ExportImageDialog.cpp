// std
#include <iostream>

// qt includes
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QPainter>

// studio
#include <Applications/Configuration.h>
#include <Data/StudioLog.h>
#include <Visualization/Visualizer.h>

#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ExportImageDialog::ExportImageDialog(QWidget* parent, Preferences& prefs, QSharedPointer<Visualizer> visualizer,
                                     bool pca_mode)
    : QDialog(parent), visualizer_(visualizer), prefs_(prefs), pca_mode_(pca_mode) {
  ui_ = new Ui_ExportImageDialog;
  ui_->setupUi(this);

  setObjectName(QString::fromUtf8("Export Image Dialog"));
  setWindowFlags(Qt::Dialog);
  setModal(true);

  // make the same size as parent minus 10% margins
  QRect rect = parent->geometry();
  auto height_margin = rect.height() * 0.1;
  auto width_margin = rect.width() * 0.1;
  rect = rect.marginsRemoved(QMargins(width_margin, height_margin, width_margin, height_margin));
  setGeometry(rect);

  connect(ui_->export_button, &QPushButton::clicked, this, &ExportImageDialog::export_clicked);
  connect(ui_->cancel_button, &QPushButton::clicked, this, &ExportImageDialog::reject);

  // load state from prefs
  QIntValidator* size_validator = new QIntValidator(1, 65535, this);
  ui_->override_width->setValidator(size_validator);
  ui_->override_height->setValidator(size_validator);
  ui_->override_width->setText(QString::number(prefs_.get_export_override_size().width()));
  ui_->override_height->setText(QString::number(prefs_.get_export_override_size().height()));
  ui_->override_window_size->setChecked(prefs_.get_export_override_size_enabled());
  ui_->show_corner_widget->setChecked(prefs_.get_export_show_orientation_marker());
  ui_->show_color_scale->setChecked(prefs_.get_export_show_color_scale());
  ui_->pca_num_images_slider->setValue(prefs_.get_export_num_pca_images());

  connect(ui_->override_window_size, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->override_width, &QLineEdit::textChanged, this, &ExportImageDialog::update_preview);
  connect(ui_->override_height, &QLineEdit::textChanged, this, &ExportImageDialog::update_preview);
  connect(ui_->transparent_background, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_corner_widget, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_color_scale, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->pca_num_images_slider, &QSlider::valueChanged, this, &ExportImageDialog::update_preview);

  ui_->pca_widget->setVisible(pca_mode);

  update_preview();
}

//---------------------------------------------------------------------------
void ExportImageDialog::export_clicked() {
  QString filter = "PNG files (*.png);;JPG files (*.jpg);;BMP files (*.bmp)";

  try {
    auto dir = prefs_.get_last_directory() + "/";
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Image"), dir, filter);
    if (filename.isEmpty()) {
      return;
    }
    prefs_.set_last_directory(QFileInfo(filename).absolutePath());
    pixmap_.save(filename);
  } catch (std::exception& e) {
    STUDIO_LOG_ERROR(e.what());
  }

  accept();
}

//---------------------------------------------------------------------------
void ExportImageDialog::update_preview() {
  QSize size(ui_->override_width->text().toInt(), ui_->override_height->text().toInt());
  if (size.width() < 1 || size.height() < 1) {
    ui_->preview->setPixmap(QPixmap{});
    return;
  }
  prefs_.set_export_override_size(size);
  prefs_.set_export_override_size_enabled(ui_->override_window_size->isChecked());
  prefs_.set_export_show_orientation_marker(ui_->show_corner_widget->isChecked());
  prefs_.set_export_show_color_scale(ui_->show_color_scale->isChecked());
  prefs_.set_export_num_pca_images(ui_->pca_num_images_slider->value());
  int num_pca_steps = ui_->pca_num_images_slider->value();
  ui_->pca_num_images_label->setText(QString::number(num_pca_steps));

  if (!prefs_.get_export_override_size_enabled()) {
    size = visualizer_->get_render_size();
  }

  if (pca_mode_) {
    int num_images = 2 * num_pca_steps + 1;
    auto canvas = QPixmap(size.width() * num_images, size.height());
    canvas.fill(QColor(0,0,0,0));
    int x = 0;

    for (int i=0;i<num_images;i++) {
      //visualizer_->display_shape(analysis_tool_->get_mode_shape(pca_mode, pca_value));
      auto pixmap =
          visualizer_->export_to_pixmap(size, ui_->transparent_background->isChecked(),
                                        ui_->show_corner_widget->isChecked(), ui_->show_color_scale->isChecked());
      QPainter painter(&canvas);
      painter.drawPixmap(x, 0, pixmap);
      x += size.width();
    }

    pixmap_ = canvas;

  } else {
    pixmap_ = visualizer_->export_to_pixmap(size, ui_->transparent_background->isChecked(),
                                            ui_->show_corner_widget->isChecked(), ui_->show_color_scale->isChecked());
  }
  ui_->preview->setPixmap(pixmap_);
}

}  // namespace shapeworks
