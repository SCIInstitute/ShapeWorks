#pragma once

// Qt
#include <QSharedPointer>
#include <QWidget>

// Studio
#include <Job/CorrespondenceQualityJob.h>

class Ui_CorrespondenceQualityPanel;
class JKQTPlotter;

namespace shapeworks {

class Session;

//! Panel for the correspondence quality analysis
/*!
 * Runs CorrespondenceQualityJob and presents the result: a per-sample table that
 * can be sorted by mean, median or max distance, a box plot of the distribution,
 * and options to color the surface by the per-vertex distance and to reorder the
 * All Samples view worst-first so the challenging shapes come up front.
 */
class CorrespondenceQualityPanel : public QWidget {
  Q_OBJECT;

 public:
  CorrespondenceQualityPanel(QWidget* parent = 0);
  ~CorrespondenceQualityPanel();

  //! set the pointer to the session
  void set_session(QSharedPointer<Session> session);
  void reset();

  //! should the per-vertex distance be shown on the surface?
  bool get_display_distance() const;

  //! name of the surface scalar to display
  std::string get_display_feature_name() const;

 public Q_SLOTS:

  void run_clicked();
  void show_distance_clicked();
  void sort_changed();

  void handle_job_progress(double progress);
  void handle_job_complete();

 Q_SIGNALS:

  //! ask the analysis tool to switch to a samples view, which is where this panel's results show.
  //! \p all_samples forces the All Samples grid (sorting only applies there); otherwise the single
  //! sample view is left alone if that is where the user already is.
  void request_samples_view(bool all_samples);

 private:
  //! value a row is ranked by, honoring the metric and normalization options
  double get_sort_value(const CorrespondenceQualityRow& row) const;

  bool sorting_by_name() const;
  bool sort_descending() const;
  bool normalized() const;

  //! report row indices in the current sort order
  std::vector<int> get_sorted_rows() const;

  void update_run_button();
  void update_summary();
  void update_table();
  void update_graphs();

  //! push (or clear) the sample ordering used by the All Samples view
  void apply_sample_order();

  QSharedPointer<Session> session_;
  QSharedPointer<CorrespondenceQualityJob> job_;

  Ui_CorrespondenceQualityPanel* ui_;
};
}  // namespace shapeworks
