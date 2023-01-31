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
  QNetworkAccessManager network_;

  Preferences& prefs_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-01-31 at 20:06:22 +0000
