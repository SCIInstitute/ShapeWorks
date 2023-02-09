#pragma once

#include <QLabel>
#include <QResizeEvent>

namespace shapeworks {

//! An extention of QLabel especially for scaled images
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
