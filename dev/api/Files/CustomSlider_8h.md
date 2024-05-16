---
title: Studio/Interface/CustomSlider.h

---

# Studio/Interface/CustomSlider.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[CustomSlider](../Classes/classCustomSlider.md)**  |




## Source code

```cpp
#pragma once

#include <QSlider>

// adapted from:
// https://stackoverflow.com/questions/69890284/qslider-in-qt-misbehaves-in-new-macos-monterey-v12-0-1-any-workaround/69890285#69890285
class CustomSlider : public QSlider {
 public:
  explicit CustomSlider(Qt::Orientation orientation, QWidget* parent = nullptr) : QSlider(orientation, parent){};
  explicit CustomSlider(QWidget* parent = nullptr);

 protected:
  virtual void paintEvent(QPaintEvent* ev);
};
```


-------------------------------

Updated on 2024-05-16 at 20:36:02 +0000
