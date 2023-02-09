#pragma once

#include <QDialog>
#include <QNetworkAccessManager>

#include <Data/Preferences.h>

namespace Ui {
class UpdateChecker;
}

namespace shapeworks {

//! Update Checker
/*!
 * This class checks for new versions of ShapeWorks and informs the user with a dialog
 */
class UpdateChecker : public QDialog {
  Q_OBJECT

 public:
  explicit UpdateChecker(Preferences& prefs, QWidget* parent = nullptr);
  ~UpdateChecker();

  void run_auto_update_check();
  void run_manual_update_check();


 public Q_SLOTS:
  void handleNetworkReply(QNetworkReply* reply);

 private:
  void run_update_check();

  bool manual_trigger_{false};
  QNetworkAccessManager network_;

  Ui::UpdateChecker* ui_;

  Preferences& prefs_;

};

}  // namespace shapeworks