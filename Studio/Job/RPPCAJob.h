#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

namespace shapeworks {

class RPPCAJob : public Job {
  Q_OBJECT
public:

  RPPCAJob(ParticleShapeStatistics stats);
  void run() override;
  QString name() override;
  Eigen::MatrixXd RPPCAEigenvectors() { return m_rppcaeigenvectors; }
  std::vector<double> RPPCAEigenvalues() { return m_rppcaeigenvalues; }
  Eigen::VectorXd RPPCAMean() { return m_rppcamean; }
  std::vector<double> RPPCAExpVar() { return m_rppcapercentVarByMode; }

 

private:

  ParticleShapeStatistics stats_;
  Eigen::MatrixXd m_rppcaeigenvectors;
  std::vector<double> m_rppcaeigenvalues;
  Eigen::VectorXd m_rppcamean;
  std::vector<double> m_rppcapercentVarByMode;

};
}
