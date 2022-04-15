#pragma once

// qt
#include <QDialog>

// studio
#include <Data/Preferences.h>

class Ui_ExportImageDialog;

namespace shapeworks {

/*!
 * The ExportImageDialog is a QDialog that implements the user interface for exporting images
 */
class ExportImageDialog : public QDialog {
  Q_OBJECT

 public:
  //! constructor
  ExportImageDialog(QWidget* parent, Preferences& prefs);

  //! destructor
  virtual ~ExportImageDialog();

 private Q_SLOTS:

  void export_clicked();

 Q_SIGNALS:

 private:
  Ui_ExportImageDialog* ui_;

  Preferences& prefs_;
};

}  // namespace shapeworks
