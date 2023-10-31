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
  enum class JobType { MSE_Plot, Predict };

  ShapeScalarJob(QSharedPointer<Session> session, QString target_feature, Eigen::MatrixXd target_particles,
                 JobType job_type);
  void run() override;
  QString name() override;

  QPixmap get_plot();

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

  JobType job_type_;
};
}  // namespace shapeworks
