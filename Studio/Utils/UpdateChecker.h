#pragma once

#include <QObject>
#include <QNetworkReply>

namespace shapeworks {

//! Update Checker
/*!
 * This class checks for new versions of ShapeWorks to inform the user
 */
class UpdateChecker : public QObject {
 Q_OBJECT

 public:
  UpdateChecker();
  ~UpdateChecker();

  void runAutoUpdateCheck();
  void runManualUpdateCheck();

 public Q_SLOTS:
  void handleNetworkReply(QNetworkReply* reply);

 private:
  QNetworkAccessManager manager_;
};

}  // namespace shapeworks