---
title: Studio/Data/SegmentationToolPanel.h

---

# Studio/Data/SegmentationToolPanel.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::SegmentationToolPanel](../Classes/classshapeworks_1_1SegmentationToolPanel.md)**  |




## Source code

```cpp
#pragma once

#include <Shape.h>

#include <QSharedPointer>
#include <QWidget>

class Ui_SegmentationToolPanel;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class SegmentationToolPanel : public QWidget {
  Q_OBJECT;

 public:
  SegmentationToolPanel(QWidget* parent = 0);
  ~SegmentationToolPanel();

  void set_session(QSharedPointer<Session> session);
  void reset();

 public Q_SLOTS:

 Q_SIGNALS:

  void update_view();

 private:
  QSharedPointer<Session> session_;

  void update_paint_value();
  void recompute_surface();

  Ui_SegmentationToolPanel* ui_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-03 at 17:42:45 +0000
