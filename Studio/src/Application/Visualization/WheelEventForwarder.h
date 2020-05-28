#pragma once

#include <QtCore/QObject>

class QScrollBar;

class WheelEventForwarder : public QObject
{
  Q_OBJECT
public:
  explicit WheelEventForwarder(QScrollBar* target);
  ~WheelEventForwarder();

  bool eventFilter(QObject* obj, QEvent* event);

private:
  QScrollBar* target_;
};
