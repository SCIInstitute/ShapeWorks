#pragma once

#include <Data/Preferences.h>

#include <QNetworkAccessManager>

//! User usage reporting (telemetry)
/*!
 * This class posts anonymous usage reporting statistics for usage tracking and software improvement purposes
 */

namespace shapeworks {

class Telemetry : public QObject {
  Q_OBJECT

 public:
  Telemetry(Preferences& prefs);

  void record_event(const QString& name, const QVariantMap& params);

 public Q_SLOTS:
  void handle_network_reply(QNetworkReply* reply);

 private:
  QNetworkAccessManager network_;

  Preferences& prefs_;
};

}  // namespace shapeworks
