

#include "CustomSlider.h"

#include <QColor>
#include <QSlider>
#include <QStyleOptionComplex>
#include <QStyleOptionSlider>
#include <QStylePainter>

#include "math.h"

CustomSlider::CustomSlider(QWidget* parent) : QSlider(parent) {
  this->setStyleSheet(
      "\
      QSlider {\
        min-height: 24px\
      }\
      QSlider::groove:horizontal {\
        border: 1px solid #262626;\
        height: 3px;\
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\
        margin: 0 5px;\
      }\
      \
      QSlider::handle:horizontal {\
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);\
        border: 1px solid #5c5c5c;\
        width: 12px;\
        margin: -8px -6px;\
        border-radius: 3px;\
      }\
    ");
};

void CustomSlider::paintEvent(QPaintEvent* ev) {
  QStylePainter p(this);
  QStyleOptionSlider opt;
  initStyleOption(&opt);

  QRect handle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  // draw tick marks
  // do this manually because they are very badly behaved with style sheets
  int interval = tickInterval();
  if (interval == 0) {
    interval = pageStep();
  }

  if (tickPosition() != NoTicks) {
    for (int i = minimum(); i <= maximum(); i += interval) {
      int x =
          std::round((double)((double)((double)(i - this->minimum()) / (double)(this->maximum() - this->minimum())) *
                                  (double)(this->width() - handle.width()) +
                              (double)(handle.width() / 2.0))) -
          1;
      int h = 4;
      p.setPen(QColor("#a5a294"));
      if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove) {
        int y = this->rect().top();
        p.drawLine(x, y, x, y + h);
      }
      if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow) {
        int y = this->rect().bottom();
        p.drawLine(x, y, x, y - h);
      }
    }
  }

  QSlider::paintEvent(ev);
}
