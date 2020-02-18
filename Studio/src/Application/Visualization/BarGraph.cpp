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
  this->recalculate_bars();
}

//---------------------------------------------------------------------------
void BarGraph::set_data(const std::vector<double>& values)
{
  double sum = std::accumulate(values.begin(), values.end(), 0);

  //this->min_val_ = *std::min_element(values.begin(), values.end());
  //this->max_val_ = *std::max_element(values.begin(), values.end());

  this->min_val_ = 0.1;
  this->max_val_ = 100;

  this->values_.clear();
  for (int i = 0; i < values.size(); i++) {
    this->values_.push_back(values[i] / sum * 100);
  }

  this->recalculate_bars();
  this->setMinimumSize((int)(this->margin_ * this->values_.size() * 2) + 45,
                       200 + this->margin_ * 5);
}

//---------------------------------------------------------------------------
void BarGraph::paint_bar_graph(QPainter &painter)
{

  if (this->font_height_ < 0) {
    QFontMetrics metrics(painter.font());
    QRect rect = metrics.tightBoundingRect("100");
    this->font_height_ = rect.height();
  }

  QString y_axis_label = "Explained Variance";

  if (this->y_axis_text_width_ < 0) {
    QFontMetrics metrics(painter.font());
    QRect rect = metrics.tightBoundingRect(y_axis_label);
    this->y_axis_text_width_ = rect.width();
    this->y_axis_text_rect_ = rect;
  }

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

  int graph_height = this->get_graph_height();

  // X Values
  QPoint last_acc_pos;
  for (size_t i = 0, s = values_.size(); i < s; ++i) {
    painter.drawRect(this->bars_[i]);
    // numbered eigen value on x axis

    if (i % 2 == 0 || this->bar_width_ > 20) {
      if (i < 99 || i % 4 == 0) {
        // after '9', there's not enough room to write each number, only write every other
        painter.drawText(45 + this->bar_width_ * (i + 0.5) + this->margin_ * (i + 1) - 3,
                         this->height() - 20, QString::number(i));
      }
    }

    int ypos = 5 + graph_height - static_cast<int>(this->get_height_for_value(
                                                     this->accumulation_[i]));

    QPoint start_pos(this->bars_[i].x(), ypos);
    QPoint end_pos(this->bars_[i].x() + this->bars_[i].width() + this->margin_, ypos);

    if (i != 0) {
      painter.drawLine(last_acc_pos, start_pos);
    }

    painter.drawLine(start_pos, end_pos);
    last_acc_pos = end_pos;
  }

  // X label
  painter.drawText(10 * this->margin_, height() - 5, "Mode");

  painter.save();
  painter.translate(10, this->height() / 2 + this->y_axis_text_width_ / 2);
  painter.rotate(-90);
  painter.drawText(0, 0, "Explained Variance");
  painter.restore();

  // Y Labels
  int num_steps = use_log_ ? (static_cast<int>(log10(max_val_)) -
                              static_cast<int>(log10(min_val_)) + 1) : 6;
  num_steps = std::max(1, num_steps);
  int start = static_cast<int>(use_log_ ? log10(min_val_) : 0);
  int separation = this->get_graph_height() / num_steps;

  int text_start = this->y_axis_text_rect_.height() + 5;

  for (int i = 0; i < num_steps; i++) {

    std::stringstream ss;
    double value;
    if (use_log_) {
      value = pow(10, (start + i));
    }
    else {
      value = static_cast<int>(start + (this->max_val_ / (num_steps - 1)) * i);
    }

    int ypos = 5 + graph_height - static_cast<int>(this->get_height_for_value(value));

    QRect label_rect(0, ypos - 100, 40, 200);
    QString label = QString::number(value);
    painter.drawText(label_rect, Qt::AlignRight | Qt::AlignVCenter, label);
    painter.drawLine(45, ypos, 50, ypos);
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
  this->recalculate_bars();
}

//---------------------------------------------------------------------------
void BarGraph::recalculate_bars()
{
  int num_bars = std::max(1, static_cast<int>(this->values_.size()));

  double width_to_work_with = (this->width() - (this->margin_ * this->values_.size())) - 45;
  int graph_height = this->get_graph_height();

  this->bar_width_ =
    std::max(this->margin_, (int)(width_to_work_with / (num_bars)));

  if (this->bars_.size() != this->values_.size()) {
    this->bars_.resize(this->values_.size());
  }

  double range =
    this->use_log_ ? (log10(this->max_val_) - log10(this->min_val_)) : (this->max_val_ -
                                                                        this->min_val_);

  int x = this->margin_ + 45;

  this->accumulation_.clear();
  double sum = 0;
  for (size_t i = 0, s = this->values_.size(); i < s; ++i) {
    double val = this->values_[i];
    sum = sum + val;
    this->accumulation_.push_back(sum);
    int barheight = static_cast<int>(this->get_height_for_value(val));
    this->bars_[i].setRect(x, 5 + graph_height - barheight, this->bar_width_, barheight);
    x += this->margin_ + this->bar_width_;
  }
}

//---------------------------------------------------------------------------
double BarGraph::get_chart_height()
{
  return this->height() * 1.0;
}

//---------------------------------------------------------------------------
double BarGraph::get_graph_height()
{
  return this->get_chart_height() - 45;
}

//---------------------------------------------------------------------------
double BarGraph::get_height_for_value(double value)
{
  double range =
    this->use_log_ ? (log10(this->max_val_) - log10(this->min_val_)) : (this->max_val_ -
                                                                        this->min_val_);

  double val =
    this->use_log_ ? (log10(value) - log10(this->min_val_)) : (value -
                                                               this->min_val_);
  if (val < 0) {
    val = 0;
  }
  double return_value = val * this->get_graph_height() / range;

  return return_value;
}
