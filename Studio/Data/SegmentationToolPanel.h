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

  //! set the pointer to the session
  void set_session(QSharedPointer<Session> session);
  void reset();

 public Q_SLOTS:

 Q_SIGNALS:

  void update_view();

 private:
  QSharedPointer<Session> session_;

   void update_paint_value();

  Ui_SegmentationToolPanel* ui_;
};
}  // namespace shapeworks
