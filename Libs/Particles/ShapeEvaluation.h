#pragma once

#include <string>
#include <Eigen/Core>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_svd.h>
#include "ParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {

class ShapeEvaluation
{
public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullCompactness(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static Eigen::MatrixXd ComputeFullCompactnessWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);
  static Eigen::VectorXd ComputeFullCompactnessBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);

  static double ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullGeneralization(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static Eigen::MatrixXd ComputeFullGeneralizationWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);
  static Eigen::VectorXd ComputeFullGeneralizationBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);

  static double ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullSpecificity(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static Eigen::MatrixXd ComputeFullSpecificityWithinSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);
  static Eigen::VectorXd ComputeFullSpecificityBetweenSubspace(const Eigen::MatrixXd &particleSystem, const std::vector<int> &num_particles_ar, std::function<void(float)> progress_callback = nullptr);

  static void DoMultiLevelModeling(const Eigen::MatrixXd &shape_matrix, const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives, Eigen::MatrixXd& between_objectives);
  static void ComputeWithinTerms(Eigen::MatrixXd &super_matrix,const std::vector<int>& num_particles, std::vector<Eigen::MatrixXd>& within_objectives);
  static void ComputeBetweenTerms(Eigen::MatrixXd &super_matrix, const std::vector<int>& num_particles, Eigen::MatrixXd& between_objectives);
};
}
