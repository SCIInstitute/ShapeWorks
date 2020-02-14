#include "BarGraph.h"

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
  this->setMinimumSize((int)(this->margin_ * this->values_.size() * 2), 200 + this->margin_ * 5);
}

//---------------------------------------------------------------------------
void BarGraph::paint_bar_graph(QPainter &painter)
{
  painter.save();
  painter.translate(0, 10);
  painter.scale(0.95, 0.95);

  QPen p(Qt::black);
  p.setWidth(2);
  painter.setPen(p);
  painter.setBrush(Qt::blue);

  for (size_t i = 0, s = values_.size(); i < s; ++i) {
    painter.drawRect(bars_[i]);
    //numbered eigen value on x axis
    painter.drawText(bar_width_ * (i + 0.5) + margin_ * (i + 1), height() - 20, QString::number(i));
  }
  //X label
  painter.drawText(2 * margin_, height() - 5, "Eigenvalues");
  // Y Label
  int num_steps = use_log_ ? (static_cast<int>(log10(max_val_)) -
                              static_cast<int>(log10(min_val_)) + 1) : 5;
  num_steps = std::max(1, num_steps);
  int start = static_cast<int>(use_log_ ? log10(min_val_) : 0);
  int separation = (this->get_chart_height() - 45) / num_steps;
  for (int i = 0; i < num_steps; i++) {
    std::stringstream ss;
    if (use_log_) {
      ss << "_1e" << (start + i);
      painter.drawText(width() - 45,
                       this->get_chart_height() - 75 - separation * i, QString(ss.str().c_str()));
    }
    else {
      ss << "_" << static_cast<int>(start + (max_val_ / num_steps) * i);
      painter.drawText(width() - 45,
                       this->get_chart_height() - 45 - separation * i, QString(ss.str().c_str()));
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
  int sz = std::max(1, static_cast<int>(this->values_.size()));
  this->bar_width_ =
    std::max(this->margin_, (int)((this->width() - (this->margin_ * this->values_.size())) / sz));
  int h = this->get_chart_height() - 45;
  if (this->bars_.size() != this->values_.size()) {
    this->bars_.resize(this->values_.size());
  }
  int x = this->margin_;
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
