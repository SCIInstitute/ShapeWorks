#include "Telemetry.h"

#include <Applications/Configuration.h>
#include <Logging.h>
#include <Utils/StudioUtils.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QUrlQuery>

namespace shapeworks {

Telemetry::Telemetry(Preferences& prefs) : prefs_(prefs) {
  QObject::connect(&network_, &QNetworkAccessManager::finished, this, &Telemetry::handle_network_reply);
}

void Telemetry::record_event(const QString& name, const QVariantMap& params) {
  if (!prefs_.get_telemetry_asked()) {
    // ask the user if it's ok to collect anonymous usage data
    int ret = QMessageBox::question(nullptr, "Data Collection",
                                    "Would you like to help improve ShapeWorks by sending anonymous usage data?",
                                    QMessageBox::Yes | QMessageBox::No);

    prefs_.set_telemetry_asked(true);
    prefs_.set_telemetry_enabled(ret == QMessageBox::Yes);
  }

  auto device_id = prefs_.get_device_id();
  auto params_json = QJsonObject{
      {"device_id", device_id}, {"platform", StudioUtils::get_platform_string()}, {"version", SHAPEWORKS_VERSION}};

  QString measurement_id{GA_MEASUREMENT_ID};
  QString api_secret{GA_API_SECRET};

  if (measurement_id.isEmpty() || api_secret.isEmpty()) {
    SW_LOG("Telemetry disabled, no measurement id or api secret");
    return;
  }

  if (!prefs_.get_telemetry_enabled()) {
    SW_LOG("Telemetry disabled by preferences");
    return;
  }

  for (auto it = params.constKeyValueBegin(); it != params.constKeyValueEnd(); it++) {
    params_json.insert(it->first, QJsonValue::fromVariant(it->second));
  }

  auto payload = QJsonObject{{"client_id", device_id},
                             {"timestamp_micros", QDateTime::currentMSecsSinceEpoch() * 1000},
                             {"events", QJsonArray{QJsonObject{{"name", name}, {"params", params_json}}}}};

  QNetworkRequest request(QUrl("https://www.google-analytics.com/mp/collect?&measurement_id=" + measurement_id +
                               "&api_secret=" + api_secret));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  network_.post(request, QJsonDocument(payload).toJson());
}

void Telemetry::handle_network_reply(QNetworkReply* reply) {
  std::string response = QString(reply->readAll()).toStdString();
  // SW_LOG("Telemetry::handleNetworkReply: {}", response);
}

}  // namespace shapeworks