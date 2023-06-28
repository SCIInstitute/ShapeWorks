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

  void set_pvalue_of_interest(double pvalue_of_interest);
  void set_pvalue_threshold(double pvalue_threshold);
  void set_num_iterations(int num_iterations);

  Eigen::VectorXf get_tvalues();
  Eigen::VectorXf get_spm_values();

 private:
  std::shared_ptr<Project> project_;
  ParticleShapeStatistics stats_;
  Eigen::VectorXf tvalues_;
  Eigen::VectorXf spm_values_;
  std::string target_group_;
  std::string target_feature_;

  double pvalue_of_interest_ = 0;
  double pvalue_threshold_ = 0;
  int num_iterations_ = 0;
};
}  // namespace shapeworks
