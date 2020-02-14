#include <Visualization/BarGraph.h>

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>

#include <sstream>
#include <cmath>
#include <iostream>

//---------------------------------------------------------------------------
BarGraph::BarGraph(QWidget* parent) :
  QWidget(parent)
{}

//---------------------------------------------------------------------------
BarGraph::~BarGraph() {}

//---------------------------------------------------------------------------
void BarGraph::set_log_scale(bool b)
{
  this->use_log_ = b;
  this->recalculate_basic_values();
}

//---------------------------------------------------------------------------
void BarGraph::set_data(std::vector<double> vals)
{
  this->min_val_ = *std::min_element(vals.begin(), vals.end());
  this->max_val_ = *std::max_element(vals.begin(), vals.end());
  this->values_.clear();
  for (size_t i = 0; i < vals.size(); i++) {
    this->values_.push_back(vals[i]);
  }
  this->recalculate_basic_values();
  this->setMinimumSize((int)(this->margin_ * this->values_.size() * 2) + 45,
                       200 + this->margin_ * 5);
}

//---------------------------------------------------------------------------
void BarGraph::paint_bar_graph(QPainter &painter)
{
  QColor top_color(100, 100, 255);
  QColor bottom_color(20, 20, 150);

  painter.save();
  QPen p(Qt::black);
  p.setWidth(2);
  painter.setPen(p);
  painter.setBrush(Qt::blue);

  QLinearGradient grad(QPointF(0.0, 0.0), QPointF(0.0, 1.0));
  grad.setCoordinateMode(QGradient::ObjectBoundingMode);
  grad.setColorAt(0, top_color);
  grad.setColorAt(1, bottom_color);

  painter.setBrush(QBrush(grad));
  painter.setPen(QPen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

  // X Values
  for (size_t i = 0, s = values_.size(); i < s; ++i) {
    painter.drawRect(this->bars_[i]);
    // numbered eigen value on x axis

    if (i % 2 == 0) {
      if (i < 99 || i % 4 == 0) {
        // after '9', there's not enough room to write each number, only write every other
        painter.drawText(45 + bar_width_ * (i + 0.5) + margin_ * (i + 1) - 3,
                         height() - 20, QString::number(i));
      }
    }
  }

  // X label
  painter.drawText(2 * margin_, height() - 5, "Mode");

  // Y Labels
  int num_steps = use_log_ ? (static_cast<int>(log10(max_val_)) -
                              static_cast<int>(log10(min_val_)) + 1) : 5;
  num_steps = std::max(1, num_steps);
  int start = static_cast<int>(use_log_ ? log10(min_val_) : 0);
  int separation = (this->get_chart_height() - 45) / num_steps;

  for (int i = 0; i < num_steps; i++) {
    std::stringstream ss;
    if (use_log_) {
      ss << "_1e" << (start + i);
      painter.drawText(0, this->get_chart_height() - 75 - separation * i,
                       QString(ss.str().c_str()));
    }
    else {
      ss << "_" << static_cast<int>(start + (max_val_ / num_steps) * i);
      painter.drawText(0, this->get_chart_height() - 45 - separation * i,
                       QString(ss.str().c_str()));
    }
  }
  painter.restore();
}

//---------------------------------------------------------------------------
void BarGraph::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  this->paint_bar_graph(painter);
}

//---------------------------------------------------------------------------
void BarGraph::resizeEvent(QResizeEvent* event)
{
  this->recalculate_basic_values();
}

//---------------------------------------------------------------------------
void BarGraph::recalculate_basic_values()
{
  int num_bars = std::max(1, static_cast<int>(this->values_.size()));

  double width_to_work_with = (this->width() - (this->margin_ * this->values_.size())) - 45;

  this->bar_width_ =
    std::max(this->margin_, (int)(width_to_work_with / (num_bars)));
  int h = this->get_chart_height() - 45;
  if (this->bars_.size() != this->values_.size()) {
    this->bars_.resize(this->values_.size());
  }
  int x = this->margin_ + 45;
  for (size_t i = 0, s = this->values_.size(); i < s; ++i) {
    double val =
      this->use_log_ ? (log10(this->values_[i]) - log10(this->min_val_)) : (this->values_[i] -
                                                                            this->min_val_);
    double range =
      this->use_log_ ? (log10(this->max_val_) - log10(this->min_val_)) : (this->max_val_ -
                                                                          this->min_val_);
    int barheight = static_cast<int>(val * h / (range));
    this->bars_[i].setRect(x, 5 + h - barheight, this->bar_width_, barheight);
    x += this->margin_ + this->bar_width_;
  }
}

//---------------------------------------------------------------------------
double BarGraph::get_chart_height()
{
  return this->height() * 1.0;
}
