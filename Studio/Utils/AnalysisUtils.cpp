#include "AnalysisUtils.h"

#include <Data/Session.h>
#include <Logging.h>
#include <jkqtplotter/graphs/jkqtpboxplot.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/graphs/jkqtpstatisticsadaptors.h>
#include <jkqtplotter/jkqtplotter.h>

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
}
//---------------------------------------------------------------------------

}  // namespace shapeworks
