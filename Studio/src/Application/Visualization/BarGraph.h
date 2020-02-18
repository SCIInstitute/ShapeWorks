#pragma once

#include <QWidget>
#include <utility>
#include <vector>

class BarGraph : public QWidget
{
  Q_OBJECT

public:
  BarGraph(QWidget* parent = 0);
  ~BarGraph();

  void set_data(const std::vector<double>& values);
  void set_log_scale(bool b);

private:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  void recalculate_bars();

  void paint_bar_graph(QPainter& painter);
  double get_chart_height();
  double get_graph_height();

  double get_height_for_value(double value);

  // members
  std::vector<double> values_;
  std::vector<double> accumulation_;
  std::vector<QRect> bars_;
  double max_val_ = 100.0, min_val_ = 0.0;
  int bar_width_ = 10, margin_ = 5;
  bool use_log_ = false;
  int font_height_ = -1;
  int y_axis_text_width_ = -1;
  QRect y_axis_text_rect_;
  QRect font_rect_;
};
