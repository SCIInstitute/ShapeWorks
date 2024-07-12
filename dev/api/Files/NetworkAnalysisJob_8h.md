---
title: Studio/Job/NetworkAnalysisJob.h

---

# Studio/Job/NetworkAnalysisJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::NetworkAnalysisJob](../Classes/classshapeworks_1_1NetworkAnalysisJob.md)**  |




## Source code

```cpp
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

  Eigen::VectorXd get_tvalues();
  Eigen::VectorXd get_spm_values();

 private:
  std::shared_ptr<Project> project_;
  ParticleShapeStatistics stats_;
  Eigen::VectorXd tvalues_;
  Eigen::VectorXd spm_values_;
  std::string target_group_;
  std::string target_feature_;

  double pvalue_of_interest_ = 0;
  double pvalue_threshold_ = 0;
  int num_iterations_ = 0;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-12 at 20:10:36 +0000
