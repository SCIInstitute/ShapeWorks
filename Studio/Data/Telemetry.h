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
  QString create_event(const QString& name, const QVariantMap& params);

  void send_event(const QString& event);

  void store_event(const QString& event);

  bool enabled_ = true;

  QNetworkAccessManager network_;

  QString active_event_;

  Preferences& prefs_;
};

}  // namespace shapeworks
