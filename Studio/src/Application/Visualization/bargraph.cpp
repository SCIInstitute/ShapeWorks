#include "bargraph.h"

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>
#include <sstream>
#include <cmath>
#include <iostream>

BarGraph::BarGraph(QWidget* parent) :
  QWidget(parent),
  max_val(100.),
  min_val(0.),
  barwidth(10),
  margin(5),
  use_log_(true)
{}

BarGraph::~BarGraph() {}

void BarGraph::setLogScale(bool b)
{
  use_log_ = b;
  recalcBasicValues();
}

void BarGraph::setData(std::vector<double> val)
{
  min_val = *std::min_element(val.begin(), val.end());
  max_val = *std::max_element(val.begin(), val.end());
  values.clear();
  for (size_t i = 0; i < val.size(); i++) {
    values.push_back(val[i]);
  }
  recalcBasicValues();
  setMinimumSize((int)(margin * values.size() * 2), 200 + margin * 5);
}

void BarGraph::paintBargraph(QPainter &painter)
{
  painter.save();
  painter.translate(0, 10);
  painter.scale(0.95, 0.95);

  QPen p(Qt::black);
  p.setWidth(2);
  painter.setPen(p);
  painter.setBrush(Qt::blue);

  for (size_t i = 0, s = values.size(); i < s; ++i) {
    painter.drawRect(bars[i]);
    //numbered eigen value on x axis
    painter.drawText(barwidth * (i + 0.5) + margin * (i + 1), height() - 20, QString::number(i));
  }
  //X label
  painter.drawText(2 * margin, height() - 5, "Eigenvalues");
  // Y Label
  int num_steps = use_log_ ? (static_cast<int>(log10(max_val)) -
                              static_cast<int>(log10(min_val)) + 1) : 5;
  num_steps = std::max(1, num_steps);
  int start = static_cast<int>(use_log_ ? log10(min_val) : 0);
  int separation = (this->get_chart_height() - 45) / num_steps;
  for (int i = 0; i < num_steps; i++) {
    std::stringstream ss;
    if (use_log_) {
      ss << "_1e" << (start + i);
      painter.drawText(width() - 45,
                       this->get_chart_height() - 75 - separation * i, QString(ss.str().c_str()));
    }
    else {
      ss << "_" << static_cast<int>(start + (max_val / num_steps) * i);
      painter.drawText(width() - 45,
                       this->get_chart_height() - 45 - separation * i, QString(ss.str().c_str()));
    }
  }
  painter.restore();
}

void BarGraph::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  paintBargraph(painter);
}

void BarGraph::resizeEvent(QResizeEvent* event)
{
  recalcBasicValues();
}

void BarGraph::recalcBasicValues()
{
  int sz = std::max(1, static_cast<int>(values.size()));
  barwidth = std::max(margin, (int)((width() - (margin * values.size())) / sz));
  int h = this->get_chart_height() - 45;
  if (bars.size() != values.size()) {
    bars.resize(values.size());
  }
  int x = margin;
  for (size_t i = 0, s = values.size(); i < s; ++i) {
    double val = use_log_ ? (log10(values[i]) - log10(min_val)) : (values[i] - min_val);
    double range = use_log_ ? (log10(max_val) - log10(min_val)) : (max_val - min_val);
    int barheight = static_cast<int>(val * h / (range));
    bars[i].setRect(x, 5 + h - barheight, barwidth, barheight);
    x += margin + barwidth;
  }
}

double BarGraph::get_chart_height()
{
  return this->height() * 1.0;
}
