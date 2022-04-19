#include <Utils/ImageLabel.h>

namespace shapeworks {
//-----------------------------------------------------------------------------
void ImageLabel::resizeEvent(QResizeEvent *resize_event) {
  QLabel::resizeEvent(resize_event);
  setPixmap(pixmap_, resize_event->size());
}

//-----------------------------------------------------------------------------
void ImageLabel::setPixmap(const QPixmap &pixmap, const QSize &size) {
  pixmap_ = pixmap;
  QLabel::setPixmap(pixmap_.scaled(size, Qt::KeepAspectRatio));
}
}  // namespace shapeworks
