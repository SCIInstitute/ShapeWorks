#include <Visualization/BarGraph.h>

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>
#include <QToolTip>

#include <sstream>
#include <cmath>
#include <iostream>

//---------------------------------------------------------------------------
BarGraph::BarGraph(QWidget* parent) :
  QWidget(parent)
{
  this->setMouseTracking(true);
  this->hover_timer_.setSingleShot(true);
  this->connect(&this->hover_timer_, &QTimer::timeout, this, &BarGraph::hover_timer_event);
}

//---------------------------------------------------------------------------
BarGraph::~BarGraph()
{}

//---------------------------------------------------------------------------
void BarGraph::set_chart_type(ChartType chart_type)
{
  this->chart_type_ = chart_type;
}

//---------------------------------------------------------------------------
void BarGraph::set_log_scale(bool b)
{
  this->use_log_ = b;
  this->recalculate_bars();
}

//---------------------------------------------------------------------------
void BarGraph::hover_timer_event()
{
  double x = this->hover_position_.x();

  double bar = std::floor((x - 45) / (this->bar_width_ + 5));

  if (bar >= 0 && bar < this->values_.size()) {
    double explained_variance = this->values_[bar];
    double accumulated_variance = this->accumulation_[bar];

    QString message("Mode: " + QString::number(bar + 1)
                    + "\nExplained Variance: " + QString::number(explained_variance, 'f', 1)
                    + "\nCumulative Variance: " + QString::number(accumulated_variance, 'f', 1));

    QPoint point(this->tooltip_position_.x(), this->tooltip_position_.y());
    QToolTip::showText(point, message);
  }
}

//---------------------------------------------------------------------------
void BarGraph::set_data(const std::vector<double>& values)
{
  double sum = std::accumulate(values.begin(), values.end(), 0.0);

  this->min_val_ = 0;
  this->max_val_ = 100;

  this->values_.clear();
  for (int i = 0; i < values.size(); i++) {
    this->values_.push_back(values[i] / sum * 100.0);
  }

  this->recalculate_bars();
  this->setMinimumSize((int) (this->margin_ * this->values_.size() * 2) + 45,
                       200 + this->margin_ * 5);
}

//---------------------------------------------------------------------------
void BarGraph::set_data(const Eigen::VectorXd values)
{
  this->values_.clear();
  for (int i = 0; i < values.size(); i++) {
    this->values_.push_back(values(i));
  }

  this->min_val_ = *min_element(this->values_.begin(), this->values_.end());
  this->max_val_ = *max_element(this->values_.begin(), this->values_.end());

  //this->min_val_ = 0;
  //this->max_val_ = 1;

  this->recalculate_bars();
  this->setMinimumSize((int) (this->margin_ * this->values_.size() * 2) + 45,
                       200 + this->margin_ * 5);
}

//---------------------------------------------------------------------------
void BarGraph::paint_bar_graph(QPainter& painter)
{

  if (this->font_height_ < 0) {

//    QFont font = this->get_font_for_size(painter.font(), this->y_label_,
    QFont font = this->get_font_for_size(painter.font(), "Explained Variance",
                                         this->height() * 0.75,
                                         this->width());
    axis_font_ = font;
    painter.setFont(font);

    //tick_font_ = this->get_font_for_size(painter.font(), this->y_label_,
    tick_font_ = this->get_font_for_size(painter.font(), "Explained Variance",
                                         this->height() * 0.5,
                                         this->width());

    QFontMetrics metrics(painter.font());
    QRect rect = metrics.tightBoundingRect("100");
    this->font_height_ = rect.height();
    this->font_width_ = rect.width();
    this->recalculate_bars();
  }

  painter.setFont(axis_font_);

  QString y_axis_label = this->y_label_;

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


  QString x_axis_label = "Mode";
  if (this->chart_type_ != ChartType::ExplainedVariance) {
    x_axis_label = "Number of Modes";
  }

  // X label
  painter.drawText(this->font_height_ + this->font_width_ + this->margin_ * 20,
                   height(), x_axis_label);

  // Y label
  painter.save();
  painter.translate(this->font_height_, this->height() / 2 + this->y_axis_text_width_ / 2);
  painter.rotate(-90);
  painter.drawText(0, 0, this->y_label_);
  painter.restore();

  painter.setFont(tick_font_);

  // X Values
  QPoint last_acc_pos;
  for (size_t i = 0, s = values_.size(); i < s; ++i) {

    if (this->chart_type_ == ChartType::ExplainedVariance) {
      painter.drawRect(this->bars_[i]);
    }
    // numbered eigen value on x axis


    if (i % 2 == 0 || this->bar_width_ > 20) {
      if (i < 99 || i % 4 == 0) {
        // after '9', there's not enough room to write each number, only write every other

        QRect label_rect(this->bars_[i].x()-100, this->height() - (this->font_height_ * 2) - margin_,
                         this->bars_[i].width()+200, this->font_height_);
        painter.drawText(label_rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(i + 1));

      }
    }

    int ypos = 5 + graph_height - static_cast<int>(this->get_height_for_value(
      this->accumulation_[i]));
    if (this->chart_type_ != ChartType::ExplainedVariance) {
      ypos = 5 + graph_height - static_cast<int>(this->get_height_for_value(
                                                   this->values_[i]));
    }

    QPoint start_pos(this->bars_[i].x(), ypos);
    QPoint end_pos(this->bars_[i].x() + this->bars_[i].width() + this->margin_, ypos);

    if (i != 0) {
      painter.drawLine(last_acc_pos, start_pos);
    }

    painter.drawLine(start_pos, end_pos);
    last_acc_pos = end_pos;
  }


  // Y Labels
  int num_steps = use_log_ ? (static_cast<int>(log10(max_val_)) -
                              static_cast<int>(log10(min_val_)) + 1) : 6;
  num_steps = std::max(1, num_steps);
  int start = static_cast<int>(use_log_ ? log10(min_val_) : 0);

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

    QRect label_rect(this->font_height_ - margin_, ypos - 100, this->font_width_, 200);
    QString label = QString::number(value);
    painter.drawText(label_rect, Qt::AlignRight | Qt::AlignVCenter, label);
    int xpos = this->font_height_ + this->font_width_;
    painter.drawLine(xpos, ypos, xpos + 5, ypos);
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
  this->font_height_ = -1;
}

//---------------------------------------------------------------------------
void BarGraph::mouseMoveEvent(QMouseEvent* event)
{
  QWidget::mouseMoveEvent(event);

  // Reset hover timer and tooltip
  this->hover_timer_.stop();
  QToolTip::hideText();

  this->tooltip_position_ = event->screenPos();
  this->hover_position_ = event->pos();

  this->hover_timer_.start(250);
}

//---------------------------------------------------------------------------
void BarGraph::recalculate_bars()
{
  int num_bars = std::max(1, static_cast<int>(this->values_.size()));

  double width_to_work_with = (this->width() - (this->margin_ * this->values_.size())) - 45;
  int graph_height = this->get_graph_height();

  this->bar_width_ =
    std::max(this->margin_, (int) (width_to_work_with / (num_bars)));

  if (this->bars_.size() != this->values_.size()) {
    this->bars_.resize(this->values_.size());
  }

  int x = this->margin_ + this->font_height_ + this->font_width_;

  this->accumulation_.clear();
  double sum = 0;
  for (size_t i = 0, s = this->values_.size(); i < s; ++i) {
    double val = this->values_[i];
    sum = sum + val;
    this->accumulation_.push_back(sum);
    int bar_height = static_cast<int>(this->get_height_for_value(val));
    this->bars_[i].setRect(x, 5 + graph_height - bar_height, this->bar_width_, bar_height);
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
  this->min_val_ = this->use_log_ ? 0.1 : 0.0;

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

//---------------------------------------------------------------------------
void BarGraph::set_y_label(QString label)
{
  this->y_label_ = label;
}

//---------------------------------------------------------------------------
QFont BarGraph::get_font_for_size(QFont font, const QString& text, int width, int height)
{
  font.setPointSize(92);
  QFontMetrics font_metrics(font);
  while (font_metrics.boundingRect(text).width() > width) {
    int new_size = font.pointSize() - 1;
    font.setPointSize(new_size);
    font_metrics = QFontMetrics(font);
  }

  if (height != -1) {
    font_metrics = QFontMetrics(font);
    while (font_metrics.height() > height) {
      int new_size = font.pointSize() - 1;
      font.setPointSize(new_size);
      font_metrics = QFontMetrics(font);
    }
  }

  return font;
}
