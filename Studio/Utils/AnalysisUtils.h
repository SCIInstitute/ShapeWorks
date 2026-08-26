#pragma once

#include <Particles/ParticleSystemEvaluation.h>

#include <QColor>
#include <QString>

class JKQTPlotter;

namespace shapeworks {

class Session;

class AnalysisUtils {
 public:
  static ParticleSystemEvaluation get_local_particle_system(Session* session, int domain);

  static void create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label, QString y_label);

  static void create_box_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label,
                              QColor color = Qt::blue);

  //! which corner the key sits in, so it can be kept clear of the data
  enum class KeyCorner { BottomLeft, BottomRight };

  //! one line of a ranked plot
  struct RankedSeries {
    Eigen::VectorXd values;
    QString label;
    QColor color{Qt::blue};
  };

  //! Per-sample ranked plot: each series drawn as a line over the sample index, with optional
  //! horizontal reference lines.  A log y axis keeps series of very different magnitude readable,
  //! and is ignored when any value is non-positive.
  static void create_ranked_plot(JKQTPlotter* plot, const std::vector<RankedSeries>& series,
                                 const std::vector<double>& reference_lines, QString title, QString x_label,
                                 QString y_label, bool log_y, KeyCorner key_corner = KeyCorner::BottomLeft);
};

}  // namespace shapeworks
