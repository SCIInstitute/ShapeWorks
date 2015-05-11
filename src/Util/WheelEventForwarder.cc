#include <Util/WheelEventForwarder.h>
#include <QScrollBar>
#include <QtCore/QEvent>
#include <QtGui/QApplication>

WheelEventForwarder::WheelEventForwarder( QScrollBar* target ) :
  QObject(),
  target_( target )
{}

WheelEventForwarder::~WheelEventForwarder()
{
  this->target_ = NULL;
}

bool WheelEventForwarder::eventFilter( QObject* obj, QEvent* event )
{
  Q_UNUSED( obj );

  static bool recursion_protection = false;

  if ( recursion_protection ) { return false; }

  if ( !this->target_ ) { return false; }

  if ( event->type() == QEvent::Wheel )
  {
    recursion_protection = true;
    this->target_->event( event );
    recursion_protection = false;
  }

  // do not filter the event
  return false;
}
