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

 private:
  void prep_data();

  void run_mse();
  void run_prediction();

  QSharedPointer<Session> session_;

  ParticleShapeStatistics stats_;

  QString target_feature_;

  QPixmap plot_;

  Eigen::MatrixXd all_particles_;
  Eigen::MatrixXd all_scalars_;

  Eigen::MatrixXd target_particles_;
  Eigen::VectorXd prediction_;

  bool num_components_ = 3;
  int num_folds_ = 5;
  int max_components_ = 20;

  JobType job_type_;
};
}  // namespace shapeworks
