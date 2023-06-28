#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

class JKQTPlotter;

namespace shapeworks {

class StatsGroupLDAJob : public Job {
  Q_OBJECT
 public:
  StatsGroupLDAJob();

  void set_stats(ParticleShapeStatistics stats);

  void run() override;

  QString name() override;

  Eigen::VectorXf get_group_pvalues();

  void plot(JKQTPlotter* plot, QString group_1_name, QString group_2_name);

 private:
  ParticleShapeStatistics stats_;
  Eigen::MatrixXd group1_x_, group2_x_, group1_pdf_, group2_pdf_, group1_map_, group2_map_;
};
}  // namespace shapeworks
