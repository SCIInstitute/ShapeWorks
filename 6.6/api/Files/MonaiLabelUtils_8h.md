---
title: Studio/ShapeWorksMONAI/MonaiLabelUtils.h

---

# Studio/ShapeWorksMONAI/MonaiLabelUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[monailabel](../Namespaces/namespacemonailabel.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[monailabel::UrlValidator](../Classes/classmonailabel_1_1UrlValidator.md)**  |
| class | **[monailabel::MonaiLabelUtils](../Classes/classmonailabel_1_1MonaiLabelUtils.md)**  |




## Source code

```cpp
#pragma once

#include <QLineEdit>
#include <QString>
#include <QUrl>
#include <QWidget>
#include <QValidator>
#include <QSharedPointer>
#include <Data/Session.h>

namespace shapeworks {
    class Session;
}
namespace monailabel {
    
class UrlValidator : public QValidator {
 public:
  UrlValidator(QObject* parent = nullptr) : QValidator(parent) {}

  QValidator::State validate(QString& input, int& pos) const override {
    Q_UNUSED(pos);
    QUrl url(input);
    if (!url.isValid() || (url.scheme() != "http" && url.scheme() != "https")) {
      return QValidator::Invalid;
    }
    // Allow '0.0.0.0'
    if (url.host() == "0.0.0.0") {
      if (url.port() == -1) {
        return QValidator::Invalid;
      }
      return QValidator::Acceptable;
    }
    if (url.host().isEmpty()) {
      return QValidator::Invalid;
    }
    if (url.port() == -1) {
      return QValidator::Invalid;
    }
    return QValidator::Acceptable;
  }
};
class MonaiLabelUtils {
 public:
  static bool createDir(const QString& dirPath);
  static bool copySegmentation(const QString& sourcePath,
                               const QString& destinationPath);
  static bool deleteTempFile(const QString& filePath);
  static std::string getFeatureName(QSharedPointer<shapeworks::Session> session);
};

}  // namespace monailabel
```


-------------------------------

Updated on 2024-10-16 at 07:18:27 +0000
