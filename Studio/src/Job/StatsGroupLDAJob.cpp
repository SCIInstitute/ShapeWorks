#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Job/StatsGroupLDAJob.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/jkqtplotter.h>

namespace shapeworks {

//---------------------------------------------------------------------------
StatsGroupLDAJob::StatsGroupLDAJob() {}

//---------------------------------------------------------------------------
void StatsGroupLDAJob::set_stats(ParticleShapeStatistics stats) { stats_ = stats; }

//---------------------------------------------------------------------------
void StatsGroupLDAJob::run() {
  emit progress(0.1);
  stats_.PrincipalComponentProjections();
  auto pca_loadings = stats_.PCALoadings();
  emit progress(0.2);

  auto& group_ids = stats_.GroupID();

  int num_samples = pca_loadings.rows();

  Eigen::MatrixXd group_1_data;
  Eigen::MatrixXd group_2_data;

  int group_1_count = std::count(group_ids.begin(), group_ids.end(), 1);
  int group_2_count = num_samples - group_1_count;
  if (group_1_count == 0 || group_2_count == 0) {
    return;
  }

  group_1_data.resize(group_1_count, pca_loadings.cols());
  group_2_data.resize(group_2_count, pca_loadings.cols());

  int group_1_idx = 0;
  int group_2_idx = 0;
  for (int i = 0; i < num_samples; i++) {
    if (group_ids[i] == 1) {
      group_1_data.row(group_1_idx++) = pca_loadings.row(i);
    } else {
      group_2_data.row(group_2_idx++) = pca_loadings.row(i);
    }
  }

  py::module sw = py::module::import("shapeworks");
  py::object lda_loadings = sw.attr("stats").attr("lda_loadings");
  emit progress(0.5);

  using ResultType =
      std::tuple<Eigen::MatrixXd, Eigen::MatrixXd, Eigen::MatrixXd, Eigen::MatrixXd, Eigen::MatrixXd, Eigen::MatrixXd>;
  ResultType result = lda_loadings(group_1_data.transpose(), group_2_data.transpose()).cast<ResultType>();

  group1_x_ = std::get<0>(result);
  group2_x_ = std::get<1>(result);
  group1_pdf_ = std::get<2>(result);
  group2_pdf_ = std::get<3>(result);
  group1_map_ = std::get<4>(result);
  group2_map_ = std::get<5>(result);

  emit progress(1.0);
}

//---------------------------------------------------------------------------
QString StatsGroupLDAJob::name() { return "Group LDA"; }

//---------------------------------------------------------------------------
void StatsGroupLDAJob::plot(JKQTPlotter* plot, QString group_1_name, QString group_2_name) {
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();
  plot->clearGraphs();

  QString title = "LDA";

  {
    QVector<double> x, y;
    for (int i = 0; i < group1_x_.size(); i++) {
      x << group1_x_(i);
      y << group1_pdf_(i);
    }

    QString x_label = group_1_name + " PDF";
    QString y_label = "y_label 1";

    size_t column_x = ds->addCopiedColumn(x, x_label);
    size_t column_y = ds->addCopiedColumn(y, y_label);

    JKQTPXYLineGraph* graph = new JKQTPXYLineGraph(plot);
    graph->setColor(QColor(239, 133, 54));
    graph->setSymbolType(JKQTPNoSymbol);
    graph->setXColumn(column_x);
    graph->setYColumn(column_y);
    graph->setTitle(group_1_name + " PDF");
    plot->addGraph(graph);
  }

  {
    QVector<double> x, y;
    for (int i = 0; i < group2_x_.size(); i++) {
      x << group2_x_(i);
      y << group2_pdf_(i);
    }

    QString x_label = group_2_name + " PDF";
    QString y_label = "y_label 2";

    size_t column_x = ds->addCopiedColumn(x, x_label);
    size_t column_y = ds->addCopiedColumn(y, y_label);

    JKQTPXYLineGraph* graph = new JKQTPXYLineGraph(plot);
    graph->setColor(Qt::blue);
    graph->setSymbolType(JKQTPNoSymbol);
    graph->setXColumn(column_x);
    graph->setYColumn(column_y);
    graph->setTitle(group_2_name + " PDF");
    plot->addGraph(graph);
  }

  {
    QVector<double> x, y;
    for (int i = 0; i < group1_map_.size(); i++) {
      x << group1_map_(i);
      y << 0.01;
    }

    int column_x = ds->addCopiedColumn(x, "scatter_1_x");
    int column_y = ds->addCopiedColumn(y, "scatter_1_y");

    auto scatter = new JKQTPXYParametrizedScatterGraph(plot);
    scatter->setColor(QColor(239, 133, 54));
    scatter->setXColumn(column_x);
    scatter->setYColumn(column_y);
    scatter->setTitle(group_1_name + " Shape Mappings");
    plot->addGraph(scatter);
  }

  {
    QVector<double> x, y;
    for (int i = 0; i < group2_map_.size(); i++) {
      x << group2_map_(i);
      y << 0.01;
    }

    int column_x = ds->addCopiedColumn(x, "scatter_2_x");
    int column_y = ds->addCopiedColumn(y, "scatter_2_y");

    auto scatter = new JKQTPXYParametrizedScatterGraph(plot);
    scatter->setColor(Qt::blue);
    scatter->setXColumn(column_x);
    scatter->setYColumn(column_y);
    scatter->setTitle(group_2_name + " Shape Mappings");
    plot->addGraph(scatter);
  }

  plot->getPlotter()->setUseAntiAliasingForGraphs(true);
  plot->getPlotter()->setUseAntiAliasingForSystem(true);
  plot->getPlotter()->setUseAntiAliasingForText(true);
  plot->getPlotter()->setPlotLabelFontSize(18);
  plot->getPlotter()->setPlotLabel("\\textbf{" + title + "}");
  plot->getPlotter()->setDefaultTextSize(14);
  plot->getPlotter()->setShowKey(true);

  plot->getXAxis()->setAxisLabel("Shape mapping to linear discrimination of variation between population means");
  plot->getXAxis()->setLabelFontSize(8);
  plot->getYAxis()->setAxisLabel("Probability Density");
  plot->getYAxis()->setLabelFontSize(14);

  plot->clearAllMouseWheelActions();
  plot->setMousePositionShown(false);
  plot->setMinimumSize(250, 250);
  plot->zoomToFit();
}
}  // namespace shapeworks
