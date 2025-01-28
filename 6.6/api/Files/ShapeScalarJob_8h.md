---
title: Studio/Job/ShapeScalarJob.h

---

# Studio/Job/ShapeScalarJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeScalarJob](../Classes/classshapeworks_1_1ShapeScalarJob.md)**  |




## Source code

```cpp
#pragma once
#include <Data/Session.h>
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

#include <QPixmap>

namespace shapeworks {

class Project;

class ShapeScalarJob : public Job {
  Q_OBJECT
 public:
  enum class JobType { Find_Components, MSE_Plot, Predict };
  enum class Direction { To_Shape, To_Scalar };

  ShapeScalarJob(QSharedPointer<Session> session, QString target_feature, Eigen::MatrixXd target_particles,
                 JobType job_type);
  void run() override;
  QString name() override;

  QPixmap get_plot();

  void set_number_of_components(int num_components) { num_components_ = num_components; }
  void set_number_of_folds(int num_folds) { num_folds_ = num_folds; }
  void set_max_number_of_components(int num) { max_components_ = num; }

  Eigen::VectorXd get_prediction() { return prediction_; };

  static Eigen::VectorXd predict_scalars(QSharedPointer<Session> session, QString target_feature,
                                         Eigen::MatrixXd target_particles);

  static Eigen::VectorXd predict_shape(QSharedPointer<Session> session, QString target_feature,
                                       Eigen::MatrixXd target_particles);

  static void clear_model() { needs_clear_ = true; };

  void set_direction(Direction direction) { direction_ = direction; }

 private:
  void prep_data();

  void run_fit();
  void run_prediction();

  static Eigen::VectorXd predict(QSharedPointer<Session> session, QString target_feature,
                                 Eigen::MatrixXd target_particles, Direction direction);

  QSharedPointer<Session> session_;

  ParticleShapeStatistics stats_;

  QString target_feature_;

  QPixmap plot_;

  Eigen::MatrixXd all_particles_;
  Eigen::MatrixXd all_scalars_;

  Eigen::MatrixXd target_values_;
  Eigen::VectorXd prediction_;

  bool num_components_ = 3;
  int num_folds_ = 5;
  int max_components_ = 20;

  Direction direction_{Direction::To_Scalar};
  JobType job_type_;

  static std::atomic<bool> needs_clear_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-01-28 at 22:52:41 +0000
