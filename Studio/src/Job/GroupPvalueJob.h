#pragma once
#include <Job/Job.h>

#include <ParticleShapeStatistics.h>

namespace shapeworks {

class GroupPvalueJob : public Job {
  Q_OBJECT
public:

  GroupPvalueJob(ParticleShapeStatistics stats);
  void run() override;
  QString name() override;

  Eigen::VectorXf get_group_pvalues();

private:

  ParticleShapeStatistics stats_;
  Eigen::VectorXf group_pvalues_;

};
}
