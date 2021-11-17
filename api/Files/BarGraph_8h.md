---
title: Studio/src/Visualization/BarGraph.h

---

# Studio/src/Visualization/BarGraph.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[BarGraph](../Classes/classBarGraph.md)**  |




## Source code

```cpp
#pragma once

#include <QWidget>
#include <utility>
#include <QTimer>
#include <vector>
#include <Eigen/Eigen>

class BarGraph : public QWidget {
Q_OBJECT

public:

  enum class ChartType {
    ExplainedVariance,
    Evaluation
  };

  BarGraph(QWidget* parent = 0);
  ~BarGraph();

  void set_chart_type(ChartType chart_type);

  void set_data(const std::vector<double>& values);
  void set_data(const Eigen::VectorXd values);
  void set_log_scale(bool b);

  void set_y_label(QString label);

public Q_SLOTS:
  void hover_timer_event();

private:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void recalculate_bars();

  void paint_bar_graph(QPainter& painter);
  double get_chart_height();
  double get_graph_height();

  double get_height_for_value(double value);

  QFont get_font_for_size(QFont font, const QString &text, int width, int height);

  // members
  QString y_label_;
  std::vector<double> values_;
  std::vector<double> accumulation_;
  std::vector<QRect> bars_;
  double max_val_ = 100.0, min_val_ = 0.0;
  int bar_width_ = 10, margin_ = 5;
  bool use_log_ = false;
  int font_height_ = -1;
  int font_width_ = -1;
  int y_axis_text_width_ = -1;
  QRect y_axis_text_rect_;
  QRect font_rect_;

  QPoint hover_position_;
  QPointF tooltip_position_;
  QTimer hover_timer_;
  QFont axis_font_;
  QFont tick_font_;

  ChartType chart_type_{ChartType::ExplainedVariance};
};
```


-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000
