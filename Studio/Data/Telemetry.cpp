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
    QMessageBox message_box(QMessageBox::Question, "Data Collection",
                            "Would you like to help improve ShapeWorks by sending anonymous usage data?<br><br>"
                            "This can be changed at any time in the preferences window.<br><br>"
                            "More information can be found <a "
                            "href=http://sciinstitute.github.io/ShapeWorks/latest/studio/"
                            "getting-started-with-studio.html#data-collection>here</a>");
    message_box.setTextFormat(Qt::RichText);
    message_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    message_box.exec();

    // check if yes was clicked
    prefs_.set_telemetry_enabled(message_box.clickedButton() == message_box.button(QMessageBox::Yes));
    prefs_.set_telemetry_asked(true);
  }

  auto device_id = prefs_.get_device_id();
  auto params_json = QJsonObject{
      {"device_id", device_id}, {"platform", StudioUtils::get_platform_string()}, {"version", SHAPEWORKS_VERSION}};

  QString measurement_id{GA_MEASUREMENT_ID};
  QString api_secret{GA_API_SECRET};

  if (!prefs_.get_telemetry_enabled() || measurement_id.isEmpty() || api_secret.isEmpty()) {
    SW_LOG("Telemetry disabled");
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
  SW_LOG("Telemetry::handleNetworkReply: {}", response);
}

}  // namespace shapeworks