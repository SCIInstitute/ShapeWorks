#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

namespace shapeworks {

class Project;

class NetworkAnalysisJob : public Job {
  Q_OBJECT
 public:
  NetworkAnalysisJob(std::shared_ptr<Project> project, std::string target_group, std::string target_feature);
  void run() override;
  QString name() override;

  Eigen::VectorXf get_tvalues();

 private:
  std::shared_ptr<Project> project_;
  ParticleShapeStatistics stats_;
  Eigen::VectorXf tvalues_;
  std::string target_group_;
  std::string target_feature_;
};
}  // namespace shapeworks
