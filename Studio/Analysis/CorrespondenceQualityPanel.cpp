// qt
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTimer>

// shapeworks
#include <Analysis/CorrespondenceQualityPanel.h>
#include <Data/Session.h>
#include <Data/Worker.h>
#include <Interface/Style.h>
#include <Logging.h>
#include <Utils/AnalysisUtils.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_CorrespondenceQualityPanel.h>

#include <algorithm>
#include <map>
#include <numeric>

namespace shapeworks {

namespace {
enum SortMetric { SORT_MEAN = 0, SORT_MEDIAN = 1, SORT_MAX = 2, SORT_LOCALIZED = 3, SORT_NAME = 4 };
}

//---------------------------------------------------------------------------
CorrespondenceQualityPanel::CorrespondenceQualityPanel(QWidget* parent)
    : QWidget(parent), ui_(new Ui_CorrespondenceQualityPanel) {
  ui_->setupUi(this);

  connect(ui_->open_button, &QPushButton::toggled, ui_->content, &QWidget::setVisible);
  connect(ui_->header, &QPushButton::clicked, ui_->open_button, &QPushButton::toggle);

  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->open_button->setChecked(false);
  ui_->progress->hide();

  ui_->show_distance->setEnabled(false);
  ui_->normalize_checkbox->setEnabled(false);
  ui_->sort_group->setEnabled(false);

  // rich text lays out to the full label width, so without a margin the summary table's right
  // border falls outside the visible area
  ui_->summary_label->setMargin(3);

  ui_->results_table->verticalHeader()->hide();
  ui_->results_table->horizontalHeader()->setStretchLastSection(true);

  update_run_button();

  connect(ui_->run_button, &QPushButton::clicked, this, &CorrespondenceQualityPanel::run_clicked);
  connect(ui_->show_distance, &QCheckBox::clicked, this, &CorrespondenceQualityPanel::show_distance_clicked);

  connect(ui_->sort_metric_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &CorrespondenceQualityPanel::options_changed);
  connect(ui_->sort_order_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &CorrespondenceQualityPanel::options_changed);
  connect(ui_->normalize_checkbox, &QCheckBox::clicked, this, &CorrespondenceQualityPanel::options_changed);
  connect(ui_->sort_samples_checkbox, &QCheckBox::clicked, this, &CorrespondenceQualityPanel::options_changed);

  update_summary();
  update_table();
  update_graphs();
}

//---------------------------------------------------------------------------
CorrespondenceQualityPanel::~CorrespondenceQualityPanel() {}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::set_session(QSharedPointer<Session> session) {
  session_ = session;
  reset();
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::reset() {
  job_.reset();
  ui_->show_distance->setEnabled(false);
  ui_->show_distance->setChecked(false);
  ui_->normalize_checkbox->setEnabled(false);
  ui_->sort_group->setEnabled(false);
  ui_->sort_samples_checkbox->setChecked(false);
  if (session_ && !session_->get_shape_display_order().empty()) {
    session_->set_shape_display_order({});
  }
  update_summary();
  update_table();
  update_graphs();
}

//---------------------------------------------------------------------------
bool CorrespondenceQualityPanel::get_display_distance() const {
  return ui_->show_distance->isChecked() && ui_->show_distance->isEnabled();
}

//---------------------------------------------------------------------------
std::string CorrespondenceQualityPanel::get_display_feature_name() const {
  return CorrespondenceQualityJob::FEATURE_NAME;
}

//---------------------------------------------------------------------------
bool CorrespondenceQualityPanel::sorting_by_name() const { return ui_->sort_metric_combo->currentIndex() == SORT_NAME; }

//---------------------------------------------------------------------------
bool CorrespondenceQualityPanel::sorting_by_ratio() const {
  return ui_->sort_metric_combo->currentIndex() == SORT_LOCALIZED;
}

//---------------------------------------------------------------------------
bool CorrespondenceQualityPanel::sort_descending() const { return ui_->sort_order_combo->currentIndex() == 0; }

//---------------------------------------------------------------------------
bool CorrespondenceQualityPanel::normalized() const { return ui_->normalize_checkbox->isChecked(); }

//---------------------------------------------------------------------------
double CorrespondenceQualityPanel::get_sort_value(const CorrespondenceQualityRow& row) const {
  const bool norm = normalized();
  switch (ui_->sort_metric_combo->currentIndex()) {
    case SORT_MEDIAN:
      return norm ? row.norm_median : row.median_dist;
    case SORT_MAX:
      return norm ? row.norm_max : row.max_dist;
    case SORT_LOCALIZED:
      // how concentrated the error is: a few swapped particles leave most of the surface intact,
      // so the mean stays low while the max spikes.  Scale free, so normalization does not apply.
      return row.mean_dist > 0 ? row.max_dist / row.mean_dist : 0.0;
    case SORT_MEAN:
    default:
      return norm ? row.norm_mean : row.mean_dist;
  }
}

//---------------------------------------------------------------------------
std::vector<int> CorrespondenceQualityPanel::get_sorted_rows() const {
  std::vector<int> order;
  if (!job_) {
    return order;
  }
  const auto& rows = job_->get_report().rows;
  order.resize(rows.size());
  std::iota(order.begin(), order.end(), 0);

  const bool by_name = sorting_by_name();
  const bool descending = sort_descending();

  std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
    if (by_name) {
      return descending ? rows[a].subject > rows[b].subject : rows[a].subject < rows[b].subject;
    }
    const double va = get_sort_value(rows[a]);
    const double vb = get_sort_value(rows[b]);
    return descending ? va > vb : va < vb;
  });
  return order;
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::run_clicked() {
  if (!session_) {
    return;
  }

  // ensure someone doesn't accidentally abort right after clicking RUN
  ui_->run_button->setEnabled(false);

  if (job_ && !job_->is_complete()) {
    ui_->run_button->setText("Aborting...");
    job_->abort();
    SW_LOG("Aborting {}", job_->name());
    return;
  }

  ui_->progress->show();
  handle_job_progress(0);

  auto method = ui_->method_combo->currentIndex() == 1 ? CorrespondenceEvaluation::DistanceMethod::PointToPoint
                                                       : CorrespondenceEvaluation::DistanceMethod::PointToCell;

  job_ = QSharedPointer<CorrespondenceQualityJob>::create(session_, method);
  connect(job_.data(), &CorrespondenceQualityJob::progress, this, &CorrespondenceQualityPanel::handle_job_progress);
  connect(job_.data(), &CorrespondenceQualityJob::finished, this, &CorrespondenceQualityPanel::handle_job_complete);
  auto worker = Worker::create_worker();
  worker->run_job(job_);

  // re-enable after 1 second to prevent accidental double-clicks
  QTimer::singleShot(1000, this, [&]() { update_run_button(); });
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::show_distance_clicked() {
  if (!session_) {
    return;
  }
  if (get_display_distance()) {
    // the distance field lives on the reconstructed surfaces of each sample
    session_->set_display_mode(DisplayMode::Reconstructed);
    Q_EMIT request_samples_view(false);
  }
  session_->trigger_reinsert_shapes();
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::options_changed() {
  // normalization applies to every number the panel shows, the summary included
  update_summary();
  update_table();
  update_graphs();
  apply_sample_order();
  if (ui_->sort_samples_checkbox->isChecked()) {
    Q_EMIT request_samples_view(true);  // the sample order is only visible in the All Samples grid
  }
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::handle_job_progress(double progress) {
  ui_->progress->setValue(static_cast<int>(progress * 100));
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::handle_job_complete() {
  update_run_button();
  ui_->progress->hide();

  if (job_->is_aborted() || job_->is_failed()) {
    job_.reset();
    reset();
    return;
  }

  // the glyphs are colored from the shape's point features, so hand them the sampled values
  auto shapes = session_->get_shapes();
  for (const auto& [shape_index, values] : job_->get_particle_values()) {
    if (shape_index >= 0 && shape_index < static_cast<int>(shapes.size())) {
      shapes[shape_index]->set_point_features(CorrespondenceQualityJob::FEATURE_NAME, values);
    }
  }

  ui_->show_distance->setEnabled(true);
  ui_->show_distance->setChecked(true);
  ui_->normalize_checkbox->setEnabled(true);
  ui_->sort_group->setEnabled(true);
  session_->set_display_mode(DisplayMode::Reconstructed);
  Q_EMIT request_samples_view(false);

  update_summary();
  update_table();
  update_graphs();
  apply_sample_order();

  session_->trigger_reinsert_shapes();
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::update_summary() {
  if (!job_) {
    ui_->summary_label->hide();
    return;
  }
  ui_->summary_label->show();

  const auto& report = job_->get_report();
  // the aggregates are always over the per-sample *mean* distance, whatever the sort metric is
  const auto& stats = normalized() ? report.agg_norm : report.agg_raw;
  const double scale = normalized() ? 100.0 : 1.0;
  const QString units = normalized() ? "% of bbox diagonal" : "world units";

  auto value = [&](double v) { return QString::number(v * scale, 'f', 4); };
  auto cell = [](const QString& text) { return "<td align='right'>" + text + "</td>"; };
  auto heading = [](const QString& text) { return "<th align='right'>" + text + "</th>"; };

  const QString template_name =
      report.template_subject.empty() ? QString("none") : QString::fromStdString(report.template_subject);

  QString text = "<table width='100%' cellspacing='0' cellpadding='4'>";
  text += "<tr><td>Samples evaluated</td>" + cell("<b>" + QString::number(report.num_evaluated) + "</b>") + "</tr>";
  text += "<tr><td>Template (excluded)</td>" + cell(template_name.toHtmlEscaped()) + "</tr>";
  text += "</table>";

  text += "<p style='margin-top:8px; margin-bottom:4px;'>Mean distance across samples (" + units + ")</p>";

  text += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>";
  text += "<tr>" + heading("Mean") + heading("Median") + heading("p95") + heading("Max") + "</tr>";
  text += "<tr>" + cell(value(stats.mean)) + cell(value(stats.median)) + cell(value(stats.p95)) +
          cell(value(stats.max)) + "</tr>";
  text += "</table>";

  ui_->summary_label->setText(text);
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::update_table() {
  auto table = ui_->results_table;
  table->clear();
  table->setRowCount(0);

  if (!job_) {
    table->hide();
    return;
  }
  table->show();

  const auto& rows = job_->get_report().rows;
  const bool norm = normalized();
  const bool multi_domain = session_ && session_->get_domains_per_shape() > 1;

  QStringList headers;
  headers << "Sample";
  if (multi_domain) {
    headers << "Domain";
  }
  const QString suffix = norm ? " %" : "";
  headers << ("Mean" + suffix) << ("Median" + suffix) << ("Max" + suffix);

  table->setColumnCount(headers.size());
  table->setHorizontalHeaderLabels(headers);
  table->setRowCount(rows.size());

  auto order = get_sorted_rows();
  const double scale = norm ? 100.0 : 1.0;

  for (int i = 0; i < static_cast<int>(order.size()); i++) {
    const auto& row = rows[order[i]];

    QString name = QString::fromStdString(row.subject);
    if (row.is_template) {
      name += " (template)";
    }

    int col = 0;
    auto name_item = new QTableWidgetItem(name);
    name_item->setToolTip(QString("bounding box diagonal: %1").arg(row.bbox_diag));
    table->setItem(i, col++, name_item);

    if (multi_domain) {
      table->setItem(i, col++, new QTableWidgetItem(QString::number(row.domain)));
    }

    const double values[3] = {norm ? row.norm_mean : row.mean_dist, norm ? row.norm_median : row.median_dist,
                              norm ? row.norm_max : row.max_dist};
    const double raw[3] = {row.mean_dist, row.median_dist, row.max_dist};
    for (int v = 0; v < 3; v++) {
      auto item = new QTableWidgetItem(QString::number(values[v] * scale, 'f', 4));
      if (norm) {
        item->setToolTip(QString("%1 in world units").arg(raw[v]));
      }
      table->setItem(i, col++, item);
    }
  }

  table->resizeColumnsToContents();
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::update_graphs() {
  if (!job_) {
    ui_->boxplot->hide();
    return;
  }
  ui_->boxplot->show();

  const auto& rows = job_->get_report().rows;
  const bool norm = normalized();
  const double scale = norm ? 100.0 : 1.0;

  // Rank the samples, then plot the chosen metric alongside the max.  Plotting the max is the
  // point: a handful of swapped particles leaves the mean almost untouched because the rest of the
  // surface is still fine, so a mean-only chart would rank that sample as healthy.  The max spikes
  // instead, and the gap between the two lines is how localized the damage is.
  std::vector<double> primary;
  std::vector<double> companion;
  for (int row_index : get_sorted_rows()) {
    const auto& row = rows[row_index];
    if (row.is_template) {  // near-identity reconstruction, would flatten the rest of the chart
      continue;
    }
    if (sorting_by_ratio()) {
      primary.push_back((norm ? row.norm_mean : row.mean_dist) * scale);
      companion.push_back((norm ? row.norm_max : row.max_dist) * scale);
    } else if (ui_->sort_metric_combo->currentIndex() == SORT_MAX) {
      primary.push_back((norm ? row.norm_max : row.max_dist) * scale);
      companion.push_back((norm ? row.norm_mean : row.mean_dist) * scale);
    } else {
      primary.push_back(get_sort_value(row) * scale);
      companion.push_back((norm ? row.norm_max : row.max_dist) * scale);
    }
  }

  if (primary.empty()) {  // e.g. a cohort of only the template
    ui_->boxplot->hide();
    return;
  }

  auto to_vector = [](const std::vector<double>& values) {
    Eigen::VectorXd out(values.size());
    for (int i = 0; i < static_cast<int>(values.size()); i++) {
      out(i) = values[i];
    }
    return out;
  };

  QString primary_label = ui_->sort_metric_combo->currentText();
  QString companion_label = "Max distance";
  if (sorting_by_name() || sorting_by_ratio()) {
    primary_label = "Mean distance";
  } else if (ui_->sort_metric_combo->currentIndex() == SORT_MAX) {
    companion_label = "Mean distance";
  }

  std::vector<AnalysisUtils::RankedSeries> series;
  series.push_back({to_vector(primary), primary_label, QColor(40, 80, 200)});
  series.push_back({to_vector(companion), companion_label, QColor(200, 60, 40)});

  // median and p95 of the per-sample mean, matching the summary above
  const auto& stats = norm ? job_->get_report().agg_norm : job_->get_report().agg_raw;
  std::vector<double> reference_lines{stats.median * scale, stats.p95 * scale};

  const QString y_label = norm ? "Distance (% of bbox diag)" : "Distance (world units)";
  const QString x_label = sorting_by_name() ? "Sample (name order)" : "Sample (table order)";

  // the two series differ by more than an order of magnitude, so a linear axis would flatten the
  // lower one against zero
  // the primary series is ranked, so the corner it falls away from is the empty one
  const auto key_corner = sort_descending() ? AnalysisUtils::KeyCorner::BottomLeft
                                            : AnalysisUtils::KeyCorner::BottomRight;

  AnalysisUtils::create_ranked_plot(ui_->boxplot, series, reference_lines, "Correspondence quality", x_label, y_label,
                                    true, key_corner);
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::apply_sample_order() {
  if (!session_) {
    return;
  }

  const bool had_order = !session_->get_shape_display_order().empty();

  if (!job_ || !ui_->sort_samples_checkbox->isChecked() || job_->get_report().rows.empty()) {
    if (had_order) {
      session_->set_shape_display_order({});
      session_->trigger_reinsert_shapes();
    }
    return;
  }

  const auto& rows = job_->get_report().rows;
  const auto& shape_indices = job_->get_row_shape_indices();

  // one value per shape: the worst of its domains, so a shape is as challenging as its
  // hardest anatomy
  std::map<int, double> value_by_shape;
  std::map<int, std::string> name_by_shape;
  for (int i = 0; i < static_cast<int>(rows.size()) && i < static_cast<int>(shape_indices.size()); i++) {
    const int shape_index = shape_indices[i];
    const double value = get_sort_value(rows[i]);
    auto it = value_by_shape.find(shape_index);
    if (it == value_by_shape.end()) {
      value_by_shape[shape_index] = value;
      name_by_shape[shape_index] = rows[i].subject;
    } else {
      it->second = std::max(it->second, value);
    }
  }

  std::vector<int> scored;
  std::vector<int> unscored;
  for (int i = 0; i < static_cast<int>(session_->get_shapes().size()); i++) {
    if (value_by_shape.count(i)) {
      scored.push_back(i);
    } else {
      unscored.push_back(i);
    }
  }

  const bool by_name = sorting_by_name();
  const bool descending = sort_descending();
  std::stable_sort(scored.begin(), scored.end(), [&](int a, int b) {
    if (by_name) {
      return descending ? name_by_shape[a] > name_by_shape[b] : name_by_shape[a] < name_by_shape[b];
    }
    return descending ? value_by_shape[a] > value_by_shape[b] : value_by_shape[a] < value_by_shape[b];
  });

  // shapes with no score (excluded, or missing meshes) keep their natural order at the end
  scored.insert(scored.end(), unscored.begin(), unscored.end());

  session_->set_shape_display_order(scored);
  session_->trigger_reinsert_shapes();
}

//---------------------------------------------------------------------------
void CorrespondenceQualityPanel::update_run_button() {
  if (job_ && !job_->is_complete()) {
    Style::apply_abort_button_style(ui_->run_button);
    ui_->run_button->setText("Abort");
  } else {
    Style::apply_normal_button_style(ui_->run_button);
    ui_->run_button->setText("Run");
  }
  ui_->run_button->setEnabled(true);
}
}  // namespace shapeworks
