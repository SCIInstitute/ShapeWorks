// std
#include <algorithm>
#include <cmath>
#include <iostream>

// qt includes
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QPainter>
#include <QScopedValueRollback>
#include <QSignalBlocker>

// studio
#include <Analysis/AnalysisTool.h>
#include <Applications/Configuration.h>
#include <Logging.h>
#include <Visualization/ColorSchemes.h>
#include <Visualization/Visualizer.h>

#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

namespace shapeworks {

namespace {
//! Bounds memory, not the GPU: tiling lifted the hardware limit.  32768 x 32768, 4 GB as ARGB.
const qint64 MAX_EXPORT_PIXELS = 1024LL * 1024 * 1024;

//! Sanity bound, well inside the width at which QImage's int bytes-per-line would overflow
const int MAX_EXPORT_DIMENSION = 65536;

//! The preview only fills a label; rendering it full size would touch gigabytes per keystroke
const int MAX_PREVIEW_DIMENSION = 2048;

//---------------------------------------------------------------------------
QSize clamp_export_size(QSize size) {
  QSize result(clamp(size.width(), 1, MAX_EXPORT_DIMENSION), clamp(size.height(), 1, MAX_EXPORT_DIMENSION));
  qint64 pixels = static_cast<qint64>(result.width()) * result.height();
  if (pixels > MAX_EXPORT_PIXELS) {
    double factor = std::sqrt(static_cast<double>(MAX_EXPORT_PIXELS) / static_cast<double>(pixels));
    result = QSize(std::max(1, static_cast<int>(result.width() * factor)),
                   std::max(1, static_cast<int>(result.height() * factor)));
  }
  return result;
}
}  // namespace

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
  QIntValidator* size_validator = new QIntValidator(1, MAX_EXPORT_DIMENSION, this);
  ui_->override_width->setValidator(size_validator);
  ui_->override_height->setValidator(size_validator);
  // a size stored by an older version, or stored while it was still being typed, may be unrenderable
  QSize export_size = clamp_export_size(prefs_.get_export_override_size());
  ui_->override_width->setText(QString::number(export_size.width()));
  ui_->override_height->setText(QString::number(export_size.height()));
  ui_->override_window_size->setChecked(prefs_.get_export_override_size_enabled());
  ui_->window_size_widget->setEnabled(ui_->override_window_size->isChecked());
  ui_->show_corner_widget->setChecked(prefs_.get_export_show_orientation_marker());
  ui_->show_color_scale->setChecked(prefs_.get_export_show_color_scale());
  ui_->pca_num_images->setValue(prefs_.get_export_num_pca_images());
  ui_->pca_range->setValue(prefs_.get_export_pca_range());
  ui_->pca_modes->setText(prefs_.get_export_pca_modes());
  ui_->pca_modes->setValidator(new QRegExpValidator(QRegExp("[0-9\\,\\-]*"), ui_->pca_modes));

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
  auto clear_size_message = [=]() { ui_->size_message->clear(); };
  connect(ui_->override_width, &QLineEdit::textEdited, clear_size_message);
  connect(ui_->override_height, &QLineEdit::textEdited, clear_size_message);
  connect(ui_->override_window_size, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->override_window_size, &QCheckBox::toggled, ui_->window_size_widget, &QWidget::setEnabled);
  connect(ui_->preset_1x, &QPushButton::clicked, this, [=]() { apply_size_preset(1); });
  connect(ui_->preset_2x, &QPushButton::clicked, this, [=]() { apply_size_preset(2); });
  connect(ui_->preset_4x, &QPushButton::clicked, this, [=]() { apply_size_preset(4); });
  connect(ui_->preset_8x, &QPushButton::clicked, this, [=]() { apply_size_preset(8); });
  connect(ui_->transparent_background, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_corner_widget, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->show_color_scale, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->pca_num_images, qOverload<int>(&QSpinBox::valueChanged), this, start_timer);
  connect(ui_->pca_range, qOverload<double>(&QDoubleSpinBox::valueChanged), this, start_timer);
  connect(ui_->pca_modes, &QLineEdit::textChanged, this, start_timer);

  ui_->pca_widget->setVisible(pca_mode);

  ui_->progress_widget->hide();
  update_preview();
}

//---------------------------------------------------------------------------
void ExportImageDialog::export_clicked() {
  QString filter = "PNG files (*.png);;JPG files (*.jpg);;BMP files (*.bmp)";

  QString filename;
  try {
    auto dir = prefs_.get_last_directory() + "/";
    filename = QFileDialog::getSaveFileName(this, tr("Export Image"), dir, filter);
    if (filename.isEmpty()) {
      return;
    }
    if (QFileInfo(filename).suffix() == "") {
      filename = filename + ".png";  // default PNG
    }
    prefs_.set_last_directory(QFileInfo(filename).absolutePath());

    // the preview renders small, so render again here at the full requested size
    QScopedValueRollback<bool> guard(updating_preview_, true);
    bool all_ready = true;
    QPixmap image = render_image(get_export_size(), all_ready);
    if (image.isNull()) {
      SW_ERROR("Unable to generate the image to export");
      return;
    }
    if (image.save(filename)) {
      SW_LOG("Saved: " + filename.toStdString());
    } else {
      SW_ERROR("Error saving: " + filename.toStdString());
    }
  } catch (std::exception& e) {
    SW_ERROR("Error saving: " + filename.toStdString() + " : " + e.what());
  }

  accept();
}

//---------------------------------------------------------------------------
void ExportImageDialog::begin_progress(int maximum, QString message) {
  ui_->progress_label->setText(message);
  ui_->progress->setMaximum(maximum);
  ui_->progress->setValue(0);
  ui_->progress_widget->show();
  QApplication::setOverrideCursor(Qt::WaitCursor);
  // last chance to paint before the render blocks the GUI thread; input stays excluded so a click
  // cannot re-enter the render or close the dialog under it
  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

//---------------------------------------------------------------------------
void ExportImageDialog::step_progress(int value) {
  ui_->progress->setValue(value);
  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

//---------------------------------------------------------------------------
void ExportImageDialog::end_progress() {
  QApplication::restoreOverrideCursor();
  ui_->progress_widget->hide();
}

//---------------------------------------------------------------------------
QSize ExportImageDialog::get_export_size() {
  if (!ui_->override_window_size->isChecked()) {
    return visualizer_->get_render_size();
  }
  return clamp_export_size(QSize(ui_->override_width->text().toInt(), ui_->override_height->text().toInt()));
}

//---------------------------------------------------------------------------
void ExportImageDialog::apply_size_preset(int multiplier) {
  QSize view_size = visualizer_->get_render_size();
  QSize size = clamp_export_size(QSize(view_size.width() * multiplier, view_size.height() * multiplier));

  // set every field without signals, then render once, rather than once per field
  QSignalBlocker block_override(ui_->override_window_size);
  QSignalBlocker block_width(ui_->override_width);
  QSignalBlocker block_height(ui_->override_height);
  ui_->override_window_size->setChecked(true);
  ui_->window_size_widget->setEnabled(true);
  ui_->override_width->setText(QString::number(size.width()));
  ui_->override_height->setText(QString::number(size.height()));
  ui_->size_message->clear();

  update_preview();
}

//---------------------------------------------------------------------------
QPixmap ExportImageDialog::render_image(QSize size, bool& all_ready) {
  all_ready = true;
  bool transparent = ui_->transparent_background->isChecked();

  ColorSchemes color_schemes;
  ColorScheme colors = color_schemes[prefs_.get_color_scheme()];

  if (!pca_mode_) {
    // a single render has no intermediate steps to report, so this is a busy indicator
    begin_progress(0, "Generating image");
    auto pixmap = visualizer_->export_to_pixmap(size, transparent, ui_->show_corner_widget->isChecked(),
                                                ui_->show_color_scale->isChecked(), all_ready);
    end_progress();
    return pixmap;
  }

  int num_pca_steps = ui_->pca_num_images->value();
  double pca_range = ui_->pca_range->value();
  auto mode_list = get_modes(ui_->pca_modes->text());

  int num_columns = 2 * num_pca_steps + 1;
  int num_rows = mode_list.size();
  double increment = pca_range / num_pca_steps;
  double margin = size.height() * 0.2;
  double side_margin = size.width() * 0.2;

  // extra 20% for labels
  qint64 canvas_width = static_cast<qint64>(size.width()) * num_columns + side_margin;
  qint64 canvas_height = static_cast<qint64>(size.height()) * num_rows + margin;
  if (canvas_width * canvas_height > MAX_EXPORT_PIXELS) {
    SW_ERROR("Image too large: reduce the export size, the number of modes, or the number of images");
    return QPixmap{};
  }

  auto canvas = QPixmap(static_cast<int>(canvas_width), static_cast<int>(canvas_height));
  if (canvas.isNull()) {
    SW_ERROR("Unable to allocate a {}x{} image", canvas_width, canvas_height);
    return QPixmap{};
  }
  canvas.fill(colors.background_qcolor(transparent ? 0 : 255));

  begin_progress(num_rows * num_columns, "Generating images");
  int completed = 0;

  int y = 0;
  for (int mode_idx = 0; mode_idx < mode_list.size(); mode_idx++) {
    int mode = mode_list[mode_idx];
    int x = side_margin;
    for (int step = -num_pca_steps; step <= num_pca_steps; step++) {
      double pca_value = step * increment;
      visualizer_->display_shape(analysis_tool_->get_mode_shape(mode, pca_value));
      bool ready = false;

      bool orientation_marker = ui_->show_corner_widget->isChecked() && step == num_pca_steps && mode_idx == 0;
      bool color_scale = ui_->show_color_scale->isChecked() && step == num_pca_steps;
      auto pixmap = visualizer_->export_to_pixmap(size, transparent, orientation_marker, color_scale, ready);
      if (!ready) {
        all_ready = false;
      }
      step_progress(++completed);

      QString text = QString::number(pca_value, 'g', 3) + " SD";
      if (step == 0) {
        text = "Mean Shape";
      }

      QPainter painter(&canvas);
      painter.drawPixmap(x, y, pixmap);
      painter.setPen(colors.get_text_color());
      QFont font = painter.font();
      font.setPixelSize(margin * 0.75);
      painter.setFont(font);

      if (mode_idx == mode_list.size() - 1) {
        QRect rect = QRect(QPoint(x, y + pixmap.height()), QPoint(x + pixmap.width(), y + pixmap.height() + margin));
        painter.drawText(rect, Qt::AlignCenter, text);
      }

      // draw rotated "mode x" string
      QString mode_string = "Mode " + QString::number(mode + 1);
      QPointF anchor(0, y + size.height());
      QRect rect = QRect(0, 0, pixmap.height(), side_margin);
      drawRotatedText(painter, mode_string, anchor, -90, rect);

      x += size.width();
    }
    y += size.height();
  }
  end_progress();
  return canvas;
}

//---------------------------------------------------------------------------
void ExportImageDialog::update_preview() {
  // processEvents() still delivers timer events, so the update timer can land back in here
  if (updating_preview_) {
    return;
  }
  QScopedValueRollback<bool> guard(updating_preview_, true);

  QSize typed_size(ui_->override_width->text().toInt(), ui_->override_height->text().toInt());
  if (typed_size.width() < 1 || typed_size.height() < 1) {
    ui_->preview->setPixmap(QPixmap{});
    return;
  }
  QSize requested_size = clamp_export_size(typed_size);
  if (requested_size != typed_size) {
    // QIntValidator calls an over-range number Intermediate, so the fields accept it as you type
    QSignalBlocker block_width(ui_->override_width);
    QSignalBlocker block_height(ui_->override_height);
    ui_->override_width->setText(QString::number(requested_size.width()));
    ui_->override_height->setText(QString::number(requested_size.height()));
    // shown here rather than logged: this dialog is modal, so a warning dialog would open behind it
    ui_->size_message->setText(
        QString("Limited to %1 x %2 (maximum 1 gigapixel)").arg(requested_size.width()).arg(requested_size.height()));
  }

  prefs_.set_export_override_size_enabled(ui_->override_window_size->isChecked());
  prefs_.set_export_show_orientation_marker(ui_->show_corner_widget->isChecked());
  prefs_.set_export_show_color_scale(ui_->show_color_scale->isChecked());
  prefs_.set_export_num_pca_images(ui_->pca_num_images->value());
  prefs_.set_export_pca_range(ui_->pca_range->value());
  prefs_.set_export_pca_modes(ui_->pca_modes->text());

  // The export renders at the full size; the preview only has to fill a label.
  QSize preview_size = get_export_size();
  if (preview_size.width() > MAX_PREVIEW_DIMENSION || preview_size.height() > MAX_PREVIEW_DIMENSION) {
    preview_size.scale(MAX_PREVIEW_DIMENSION, MAX_PREVIEW_DIMENSION, Qt::KeepAspectRatio);
  }

  bool all_ready = true;
  pixmap_ = render_image(preview_size, all_ready);
  if (pixmap_.isNull()) {
    end_progress();
    ui_->preview->setPixmap(QPixmap{});
    ui_->preview->setText("Unable to generate the preview.  See the log for details.");
    return;
  }

  // a shape was still loading, so keep an indicator up and come back for another pass
  ui_->progress_widget->setVisible(!all_ready);
  if (!all_ready) {
    ui_->progress_label->setText("Waiting for shapes");
    ui_->progress->setMaximum(0);
    update_preview_timer_.start(2000);
  }

  ui_->preview->setPixmap(pixmap_);

  prefs_.set_export_override_size(requested_size);
}

//---------------------------------------------------------------------------
void ExportImageDialog::drawRotatedText(QPainter& painter, QString text, QPointF point, qreal angle, QRect rect) {
  painter.save();
  painter.translate(point);
  painter.rotate(-90);
  painter.drawText(rect, Qt::AlignCenter, text);
  painter.restore();
}

//---------------------------------------------------------------------------
QVector<int> ExportImageDialog::get_modes(QString string) {
  QVector<int> list;
  int max_mode = analysis_tool_->get_session()->get_num_shapes() - 2;

  QStringList items = string.split(",");
  Q_FOREACH (auto item, items) {
    if (item.contains("-")) {  // range

      int start = clamp(item.split("-")[0].toInt() - 1, 0, max_mode);
      int end = clamp(item.split("-")[1].toInt() - 1, 0, max_mode);
      for (int i = start; i <= end; i++) {
        if (!list.contains(i)) {
          list.push_back(i);
        }
      }
    } else {  // single number
      int i = clamp(item.toInt() - 1, 0, max_mode);
      if (!list.contains(i)) {
        list.push_back(i);
      }
    }
  }

  return list;
}

}  // namespace shapeworks
