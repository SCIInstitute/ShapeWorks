#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>
#include <Eigen/Eigen>
namespace shapeworks {

class RRPCAJob : public Job {
  Q_OBJECT
public:

  RPPCAJob(ParticleShapeStatistics stats);
  void run() override;
  QString name() override;

 

private:

  ParticleShapeStatistics stats_;
  

};
}
