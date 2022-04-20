// std
#include <iostream>

// qt includes
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QPainter>

// studio
#include <Analysis/AnalysisTool.h>
#include <Applications/Configuration.h>
#include <Data/StudioLog.h>
#include <Visualization/ColorSchemes.h>
#include <Visualization/Visualizer.h>

#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ExportImageDialog::ExportImageDialog(QWidget* parent, Preferences& prefs, QSharedPointer<AnalysisTool> analysis_tool,
                                     QSharedPointer<Visualizer> visualizer, bool pca_mode)
    : QDialog(parent), visualizer_(visualizer), prefs_(prefs), pca_mode_(pca_mode), analysis_tool_(analysis_tool) {
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

  // load state from prefs
  QIntValidator* size_validator = new QIntValidator(1, 65535, this);
  ui_->override_width->setValidator(size_validator);
  ui_->override_height->setValidator(size_validator);
  ui_->override_width->setText(QString::number(prefs_.get_export_override_size().width()));
  ui_->override_height->setText(QString::number(prefs_.get_export_override_size().height()));
  ui_->override_window_size->setChecked(prefs_.get_export_override_size_enabled());
  ui_->show_corner_widget->setChecked(prefs_.get_export_show_orientation_marker());
  ui_->show_color_scale->setChecked(prefs_.get_export_show_color_scale());
  ui_->pca_num_images->setValue(prefs_.get_export_num_pca_images());
  ui_->pca_range->setValue(prefs_.get_export_pca_range());

  connect(ui_->export_button, &QPushButton::clicked, this, &ExportImageDialog::export_clicked);
  connect(ui_->cancel_button, &QPushButton::clicked, this, &ExportImageDialog::reject);

  connect(&update_preview_timer_, &QTimer::timeout, this, &ExportImageDialog::update_preview);
  update_preview_timer_.setSingleShot(true);

  auto start_timer = [=]() {
    ui_->progress_widget->show();
    update_preview_timer_.start(1000);
  };
  connect(ui_->override_width, &QLineEdit::textChanged, start_timer);
  connect(ui_->override_height, &QLineEdit::textChanged, start_timer);
  connect(ui_->override_window_size, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->transparent_background, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_corner_widget, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_color_scale, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->pca_num_images, qOverload<int>(&QSpinBox::valueChanged), this, start_timer);
  connect(ui_->pca_range, qOverload<double>(&QDoubleSpinBox::valueChanged), this, start_timer);

  ui_->pca_widget->setVisible(pca_mode);

  ui_->progress_widget->hide();
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
  prefs_.set_export_num_pca_images(ui_->pca_num_images->value());
  prefs_.set_export_pca_range(ui_->pca_range->value());
  int num_pca_steps = ui_->pca_num_images->value();
  double pca_range = ui_->pca_range->value();

  if (!prefs_.get_export_override_size_enabled()) {
    size = visualizer_->get_render_size();
  }

  bool transparent = ui_->transparent_background->isChecked();

  bool all_ready = true;

  ColorSchemes color_schemes;
  ColorScheme colors = color_schemes[prefs_.get_color_scheme()];

  if (pca_mode_) {
    int num_images = 2 * num_pca_steps + 1;
    double increment = pca_range / num_pca_steps;
    double margin = size.height() * 0.2;
    auto canvas = QPixmap(size.width() * num_images, size.height() + margin);  // extra 20% for labels

    canvas.fill(colors.background_qcolor(transparent ? 0 : 255));
    int x = 0;

    int mode = analysis_tool_->get_pca_mode();

    for (int step = -num_pca_steps; step <= num_pca_steps; step++) {
      double pca_value = step * increment;
      visualizer_->display_shape(analysis_tool_->get_mode_shape(mode, pca_value));
      bool ready = false;

      bool orientation_marker = ui_->show_corner_widget->isChecked() && step == num_pca_steps;
      bool color_scale = ui_->show_color_scale->isChecked() && step == num_pca_steps;
      auto pixmap = visualizer_->export_to_pixmap(size, transparent, orientation_marker, color_scale, ready);
      if (!ready) {
        all_ready = false;
      }

      QString text = QString::number(pca_value, 'g', 2) + " SD";
      if (step == 0) {
        text = "Mean Shape";
      }

      QPainter painter(&canvas);
      painter.drawPixmap(x, 0, pixmap);
      painter.setPen(colors.get_text_color());
      QFont font = painter.font();
      font.setPixelSize(margin * 0.75);
      painter.setFont(font);

      auto metrics = QFontMetrics(font);
      auto rect = metrics.boundingRect(text);

      double mid = x + pixmap.width() / 2.0;

      QRect draw_rect = QRect(QPoint(x, pixmap.height()), QPoint(x + pixmap.width(), pixmap.height() + margin));

      QPointF anchor(mid - rect.width() / 2.0, pixmap.height() + (margin / 2.0));

      painter.drawText(draw_rect, Qt::AlignCenter, text);
      x += size.width();
    }

    pixmap_ = canvas;

  } else {
    pixmap_ = visualizer_->export_to_pixmap(size, ui_->transparent_background->isChecked(),
                                            ui_->show_corner_widget->isChecked(), ui_->show_color_scale->isChecked(),
                                            all_ready);
  }

  ui_->progress_widget->setVisible(!all_ready);
  if (!all_ready) {
    update_preview_timer_.start(2000);
  }

  ui_->preview->setPixmap(pixmap_);
}

}  // namespace shapeworks
