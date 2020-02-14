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

  void recalculate_basic_values();

  void paint_bar_graph(QPainter& painter);
  double get_chart_height();

  // members
  std::vector<double> values_;
  std::vector<QRect> bars_;
  double max_val_ = 100.0, min_val_ = 0.0;
  int bar_width_ = 10, margin_ = 5;
  bool use_log_ = true;
};
