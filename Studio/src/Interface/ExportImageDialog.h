#pragma once

// qt
#include <QDialog>

// studio
#include <Data/Preferences.h>

class Ui_ExportImageDialog;

namespace shapeworks {

class Visualizer;

/*!
 * The ExportImageDialog is a QDialog that implements the user interface for exporting images
 */
class ExportImageDialog : public QDialog {
  Q_OBJECT

 public:
  ExportImageDialog(QWidget* parent, Preferences& prefs, QSharedPointer<Visualizer> visualizer);

 private Q_SLOTS:

  void export_clicked();

 Q_SIGNALS:

 private:
  void update_preview();

  Ui_ExportImageDialog* ui_;

  QSharedPointer<Visualizer> visualizer_;

  Preferences& prefs_;

  QPixmap pixmap_;
};

}  // namespace shapeworks
