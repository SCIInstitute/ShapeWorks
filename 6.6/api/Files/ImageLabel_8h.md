---
title: Studio/Utils/ImageLabel.h

---

# Studio/Utils/ImageLabel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ImageLabel](../Classes/classshapeworks_1_1ImageLabel.md)** <br>An extention of QLabel especially for scaled images.  |




## Source code

```cpp
#pragma once

#include <QLabel>
#include <QResizeEvent>

namespace shapeworks {

class ImageLabel : public QLabel {
 public:
  ImageLabel(QWidget *parent) : QLabel(parent) {}
  void setPixmap(const QPixmap &pixmap) { setPixmap(pixmap, size()); }

 protected:
  void resizeEvent(QResizeEvent *reisze_event) override;

 private:
  void setPixmap(const QPixmap &pixmap, const QSize &size);

 private:
  QPixmap pixmap_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-02 at 21:06:13 +0000