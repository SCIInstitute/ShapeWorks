#ifndef WHEELEVENTFORWARDER_H
#define WHEELEVENTFORWARDER_H

#include <QtCore/QObject>

class QScrollBar;

class WheelEventForwarder : public QObject
{
  Q_OBJECT
public:
  explicit WheelEventForwarder( QScrollBar* target );
  ~WheelEventForwarder();

  bool eventFilter( QObject* obj, QEvent* event );

private:
  QScrollBar* target_;
};

#endif // WHEELEVENTFORWARDER_H
