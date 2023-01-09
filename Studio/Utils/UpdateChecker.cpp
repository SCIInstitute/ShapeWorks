#include "UpdateChecker.h"

#include <Logging.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace shapeworks {

class Update {
 public:
};

UpdateChecker::UpdateChecker() {
  connect(&manager_, &QNetworkAccessManager::finished, this, &UpdateChecker::handleNetworkReply);
}

UpdateChecker::~UpdateChecker() {}

void UpdateChecker::runAutoUpdateCheck() {}

void UpdateChecker::runManualUpdateCheck() {
  // check if a new version is available
  QNetworkRequest request(QUrl("https://api.github.com/repos/SCIInstitute/ShapeWorks/releases/latest"));
  manager_.get(request);
}
void UpdateChecker::handleNetworkReply(QNetworkReply* reply) {
  // get the json response
  QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
  QJsonObject jsonObject = jsonResponse.object();

  // get the version
  QString version = jsonObject.value("tag_name").toString();
  SW_LOG("version: " + version.toStdString());
}

}  // namespace shapeworks
