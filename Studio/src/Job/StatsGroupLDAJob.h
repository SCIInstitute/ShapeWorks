#pragma once
#include <Job/Job.h>

#include <ParticleShapeStatistics.h>

namespace shapeworks {

class StatsGroupLDAJob : public Job {
  Q_OBJECT
public:

  StatsGroupLDAJob(ParticleShapeStatistics stats);
  void run() override;
  QString name() override;

  Eigen::VectorXf get_group_pvalues();

private:

  ParticleShapeStatistics stats_;
  Eigen::VectorXf group_pvalues_;

};
}
