#include "Telemetry.h"

#include <Applications/Configuration.h>
#include <Logging.h>
#include <Utils/StudioUtils.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QOperatingSystemVersion>
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

  QString measurement_id{GA_MEASUREMENT_ID};
  QString api_secret{GA_API_SECRET};

  if (measurement_id.isEmpty() || api_secret.isEmpty()) {
    SW_LOG("Telemetry disabled, no measurement id or api secret");
    enabled_ = false;
    return;
  }

  if (!prefs_.get_telemetry_enabled()) {
    SW_LOG("Telemetry disabled by preferences");
    enabled_ = false;
    return;
  }

  active_event_ = create_event(name, params);
  send_event(active_event_);
}

void Telemetry::handle_network_reply(QNetworkReply* reply) {
  try {
    std::string response = QString(reply->readAll()).toStdString();
    // parse reply as json
    QJsonDocument json = QJsonDocument::fromJson(response.c_str());
    // get response from json
    QJsonObject json_obj = json.object();
    if (json_obj["response"].toString() == "ok") {
      //SW_DEBUG("Telemetry::handleNetworkReply: ok");

      auto events = prefs_.get_pending_telemetry_events();
      if (!events.empty()) {
        //SW_DEBUG("Telemetry::handleNetworkReply: sending pending events");
        auto event = events.front();
        events.pop_front();
        prefs_.set_pending_telemetry_events(events);
        send_event(event);
      }

      return;
    }
  } catch (std::exception& e) {
    //SW_DEBUG("Telemetry::handleNetworkReply: exception: {}", e.what());
    return;
  }

  enabled_ = false;
  // store event for later retry
  store_event(active_event_);
  active_event_ = "";
}

QString Telemetry::create_event(const QString& name, const QVariantMap& params) {
  auto device_id = prefs_.get_device_id();
  auto params_json = QJsonObject{
      {"device_id", device_id}, {"platform", StudioUtils::get_platform_string()}, {"version", SHAPEWORKS_VERSION}};

  const auto os = QOperatingSystemVersion::current();
  params_json["operating_system"] =
      QString{"%1 %2.%3"}.arg(os.name(), QString::number(os.majorVersion()), QString::number(os.minorVersion()));
  params_json["operating_system_language"] = QLocale::system().nativeLanguageName();

  for (auto it = params.constKeyValueBegin(); it != params.constKeyValueEnd(); it++) {
    params_json.insert(it->first, QJsonValue::fromVariant(it->second));
  }

  auto event = QJsonObject{{"client_id", device_id},
                           {"timestamp_micros", QDateTime::currentMSecsSinceEpoch() * 1000},
                           {"events", QJsonArray{QJsonObject{{"name", name}, {"params", params_json}}}}};

  return QJsonDocument(event).toJson();
}

void Telemetry::send_event(const QString& event) {
  if (!enabled_) {
    store_event(event);
    return;
  }

  QString api_secret{GA_API_SECRET};

  QUrlQuery query;
  query.addQueryItem("secret", api_secret);

  QString server = "https://tracker.shapeworks-cloud.org:5001";
  QUrl url(server + "/post_json");
  url.setQuery(query);
  QNetworkRequest network_request(url);

  active_event_ = event;
  network_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  network_.post(network_request, event.toUtf8());
}

void Telemetry::store_event(const QString& event) {
  auto events = prefs_.get_pending_telemetry_events();
  events.push_back(event);
  while (events.size() > 100) {  // only keep the last 100 events
    events.pop_front();
  }
  prefs_.set_pending_telemetry_events(events);
}

}  // namespace shapeworks