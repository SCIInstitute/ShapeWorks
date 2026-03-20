#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

class JKQTPlotter;

namespace shapeworks {

class StatsGroupDWDJob : public Job {
  Q_OBJECT
 public:
  StatsGroupDWDJob();

  void set_stats(ParticleShapeStatistics stats);

  void run() override;

  QString name() override;

  void plot(JKQTPlotter* plot, QString group_1_name, QString group_2_name);

  bool succeeded() const { return succeeded_; }

 private:
  bool succeeded_ = false;
  ParticleShapeStatistics stats_;
  Eigen::MatrixXd group1_x_, group2_x_, group1_pdf_, group2_pdf_, group1_map_, group2_map_;
};
}  // namespace shapeworks
