---
title: Studio/Data/Telemetry.h

---

# Studio/Data/Telemetry.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Telemetry](../Classes/classshapeworks_1_1Telemetry.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>

#include <QNetworkAccessManager>


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
```


-------------------------------

Updated on 2024-03-13 at 22:27:54 +0000
