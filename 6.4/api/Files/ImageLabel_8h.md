---
title: Studio/src/Utils/ImageLabel.h

---

# Studio/src/Utils/ImageLabel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-06-13 at 08:58:05 +0000
