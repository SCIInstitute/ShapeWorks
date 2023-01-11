#pragma once

#include <QDialog>
#include <QNetworkReply>

#include <Data/Preferences.h>

namespace Ui {
class UpdateChecker;
}

namespace shapeworks {

class UpdateChecker : public QDialog {
  Q_OBJECT

 public:
  explicit UpdateChecker(Preferences& prefs, QWidget* parent = nullptr);
  ~UpdateChecker();

  void runAutoUpdateCheck();
  void runManualUpdateCheck();


 public Q_SLOTS:
  void handleNetworkReply(QNetworkReply* reply);

 private:
  void run_update_check();

  bool manual_trigger_{false};
  QNetworkAccessManager manager_;

  Ui::UpdateChecker* ui_;

  Preferences& prefs_;

};

}  // namespace shapeworks