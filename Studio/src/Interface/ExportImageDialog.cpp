// std
#include <iostream>

// qt includes
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>

// studio
#include <Applications/Configuration.h>
#include <Data/StudioLog.h>
#include <Visualization/Visualizer.h>

#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ExportImageDialog::ExportImageDialog(QWidget* parent, Preferences& prefs, QSharedPointer<Visualizer> visualizer)
    : QDialog(parent), visualizer_(visualizer), prefs_(prefs) {
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

  connect(ui_->override_window_size, &QCheckBox::toggled, this, &ExportImageDialog::update_preview);
  connect(ui_->override_width, &QLineEdit::textChanged, this, &ExportImageDialog::update_preview);
  connect(ui_->override_height, &QLineEdit::textChanged, this, &ExportImageDialog::update_preview);

  update_preview();
}

//---------------------------------------------------------------------------
void ExportImageDialog::export_clicked() {
  QString filter = "PNG files (*.png);;JPG files (*.jpg);;BMP files (*.bmp)";

  auto dir = prefs_.get_last_directory() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Image"), dir + "Untitled", filter);
  if (filename.isEmpty()) {
    return;
  }

  pixmap_.save(filename);
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

  if (!prefs_.get_export_override_size_enabled()) {
    size = visualizer_->get_render_size();
  }

  pixmap_ = visualizer_->export_to_pixmap(size, true);
  ui_->preview->setPixmap(pixmap_);
}

}  // namespace shapeworks
