#include "AnalysisUtils.h"

#include <Data/Session.h>
#include <Logging.h>
#include <jkqtplotter/graphs/jkqtpboxplot.h>
#include <jkqtplotter/graphs/jkqtpgeometric.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/graphs/jkqtpstatisticsadaptors.h>
#include <jkqtplotter/jkqtplotter.h>

#include <limits>

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleSystemEvaluation AnalysisUtils::get_local_particle_system(Session* session, int domain) {
  Eigen::MatrixXd matrix;
  int num_shapes = session->get_num_shapes();
  int num_total_particles = session->get_num_particles();
  if (num_shapes == 0 || num_total_particles == 0) {
    return ParticleSystemEvaluation{matrix};
  }

  auto shapes = session->get_shapes();
  int num_particles_domain = shapes[0]->get_particles().get_local_particles(domain).size() / 3;
  matrix.resize(num_particles_domain * 3, num_shapes);

  for (int i = 0; i < num_shapes; i++) {
    auto particles = shapes[i]->get_particles().get_local_particles(domain);
    for (int j = 0; j < num_particles_domain; j++) {
      matrix(j * 3 + 0, i) = particles[j * 3 + 0];
      matrix(j * 3 + 1, i) = particles[j * 3 + 1];
      matrix(j * 3 + 2, i) = particles[j * 3 + 2];
    }
  }

  return ParticleSystemEvaluation{matrix};
}

//---------------------------------------------------------------------------
void AnalysisUtils::create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label,
                                QString y_label) {
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();

  QVector<double> x, y;
  for (int i = 0; i < data.size(); i++) {
    x << i + 1;
    y << data[i];
  }
  size_t column_x = ds->addCopiedColumn(x, x_label);
  size_t column_y = ds->addCopiedColumn(y, y_label);

  plot->clearGraphs();
  JKQTPXYLineGraph* graph = new JKQTPXYLineGraph(plot);
  graph->setColor(Qt::blue);
  graph->setSymbolType(JKQTPNoSymbol);
  graph->setXColumn(column_x);
  graph->setYColumn(column_y);
  graph->setTitle(title);

  plot->getPlotter()->setUseAntiAliasingForGraphs(true);
  plot->getPlotter()->setUseAntiAliasingForSystem(true);
  plot->getPlotter()->setUseAntiAliasingForText(true);
  plot->getPlotter()->setPlotLabelFontSize(18);
  plot->getPlotter()->setPlotLabel("\\textbf{" + title + "}");
  plot->getPlotter()->setDefaultTextSize(14);
  plot->getPlotter()->setShowKey(false);

  plot->getXAxis()->setAxisLabel(x_label);
  plot->getXAxis()->setLabelFontSize(14);
  plot->getYAxis()->setAxisLabel(y_label);
  plot->getYAxis()->setLabelFontSize(14);

  plot->getPlotter()->setPlotBorderBottom(10);

  plot->clearAllMouseWheelActions();
  plot->setMousePositionShown(false);
  plot->addGraph(graph);
  plot->zoomToFit();
}

//---------------------------------------------------------------------------
void AnalysisUtils::create_box_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label,
                                    QColor color) {
  plot->clearGraphs();
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();
  size_t column_x = ds->addCopiedColumn(data, x_label);

  auto [graph, outliers] = jkqtpstatAddVBoxplotAndOutliers(plot->getPlotter(), ds->begin(column_x), ds->end(column_x),
                                                           0, 0.25, 0.75, 0.03, 0.97, "outliers");

  // set color to blue
  graph->setColor(color);
  outliers->setColor(color);

  plot->getPlotter()->setUseAntiAliasingForGraphs(true);
  plot->getPlotter()->setUseAntiAliasingForSystem(true);
  plot->getPlotter()->setUseAntiAliasingForText(true);
  plot->getPlotter()->setPlotLabelFontSize(18);
  plot->getPlotter()->setPlotLabel("\\textbf{" + title + "}");
  plot->getPlotter()->setDefaultTextSize(14);
  plot->getPlotter()->setShowKey(false);

  plot->clearAllMouseWheelActions();
  plot->setMousePositionShown(false);
  plot->setMinimumSize(250, 250);
  plot->zoomToFit();
  // setColor() above only mutates style state; without this the widget can keep showing the
  // render triggered by addGraph(), which still has the auto-assigned palette color
  plot->redrawPlot();
}

//---------------------------------------------------------------------------
void AnalysisUtils::create_ranked_plot(JKQTPlotter* plot, const std::vector<RankedSeries>& series,
                                       const std::vector<double>& reference_lines, QString title, QString x_label,
                                       QString y_label, bool log_y, KeyCorner key_corner) {
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();
  plot->clearGraphs();

  int num_points = 0;
  double min_value = std::numeric_limits<double>::max();
  for (const auto& s : series) {
    num_points = std::max<int>(num_points, s.values.size());
    for (int i = 0; i < s.values.size(); i++) {
      min_value = std::min(min_value, s.values[i]);
    }
  }
  if (num_points == 0) {
    plot->redrawPlot();
    return;
  }

  QVector<double> x;
  for (int i = 0; i < num_points; i++) {
    x << i + 1;
  }
  size_t column_x = ds->addCopiedColumn(x, x_label);

  // a log axis cannot show a zero or negative value, so only use it when the data allows
  const bool use_log = log_y && min_value > 0;
  plot->getYAxis()->setLogAxis(use_log);

  // reference lines go in first so the series are drawn over them
  for (double value : reference_lines) {
    if (use_log && value <= 0) {
      continue;
    }
    // anchored on the first sample so zoomToFit() does not stretch the axis back to zero
    auto* line = new JKQTPGeoInfiniteLine(plot->getPlotter(), 1, value, 1, 0, QColor(120, 120, 120), 1, Qt::DashLine);
    line->setTwoSided(true);
    plot->addGraph(line);
  }

  for (const auto& s : series) {
    QVector<double> y;
    for (int i = 0; i < s.values.size(); i++) {
      y << s.values[i];
    }
    size_t column_y = ds->addCopiedColumn(y, s.label);

    auto* graph = new JKQTPXYLineGraph(plot);
    graph->setColor(s.color);
    graph->setSymbolType(JKQTPNoSymbol);
    graph->setLineWidth(2);
    graph->setXColumn(column_x);
    graph->setYColumn(column_y);
    graph->setTitle(s.label);
    plot->addGraph(graph);
  }

  plot->getPlotter()->setUseAntiAliasingForGraphs(true);
  plot->getPlotter()->setUseAntiAliasingForSystem(true);
  plot->getPlotter()->setUseAntiAliasingForText(true);
  plot->getPlotter()->setPlotLabelFontSize(18);
  plot->getPlotter()->setPlotLabel("\\textbf{" + title + "}");
  plot->getPlotter()->setDefaultTextSize(14);
  plot->getPlotter()->setShowKey(series.size() > 1);
  plot->getPlotter()->setKeyFontSize(10);
  // an outside key gets clipped in a panel this narrow, so tuck it into whichever bottom corner the
  // caller says the data leaves free
  plot->getPlotter()->setKeyPosition(key_corner == KeyCorner::BottomRight ? JKQTPKeyInsideBottomRight
                                                                         : JKQTPKeyInsideBottomLeft);

  // setLabelFontSize() is the axis *title*; the tick numbers have their own, much smaller, default
  plot->getXAxis()->setAxisLabel(x_label);
  plot->getXAxis()->setLabelFontSize(14);
  plot->getXAxis()->setTickLabelFontSize(12);
  plot->getYAxis()->setAxisLabel(y_label);
  plot->getYAxis()->setLabelFontSize(14);
  plot->getYAxis()->setTickLabelFontSize(12);

  if (use_log) {
    // a log axis only labels whole decades, which over this range means just two numbers, so label
    // the minor ticks as well
    plot->getYAxis()->setMinorTickLabelsEnabled(true);
    plot->getYAxis()->setMinorTickLabelFullNumber(true);
    plot->getYAxis()->setMinorTickLabelFontSize(9);
    // the axis drops any minor label that would collide with its neighbour, so ask for the denser
    // set and let it keep whatever fits
    plot->getYAxis()->setMinorTicks(4);
  }

  plot->getPlotter()->setPlotBorderBottom(10);

  plot->clearAllMouseWheelActions();
  plot->setMousePositionShown(false);
  plot->zoomToFit();
  plot->redrawPlot();
}

//---------------------------------------------------------------------------

}  // namespace shapeworks
