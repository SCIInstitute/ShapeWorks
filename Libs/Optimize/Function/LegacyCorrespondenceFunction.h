#pragma once

#include <vector>

#include "LegacyCorrespondenceFunction.h"
#include "Libs/Optimize/Matrix/LegacyShapeMatrix.h"
#include "VectorFunction.h"

namespace shapeworks {

/**
 * \class LegacyCorrespondenceFunction (ParticleEnsembleEntropyFunction)
 *
 *
 */
class LegacyCorrespondenceFunction : public VectorFunction {
 public:
  constexpr static unsigned int VDimension = 3;
  constexpr static unsigned int Dimension = VDimension;

  typedef LegacyShapeMatrix ShapeMatrixType;

  typedef typename ShapeMatrixType::DataType DataType;

  /** Vector & Point types. */
  typedef VectorFunction::VectorType VectorType;
  typedef ParticleSystem::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;

  /// Factory method for creating instances
  static std::shared_ptr<LegacyCorrespondenceFunction> New() {
    return std::make_shared<LegacyCorrespondenceFunction>();
  }

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double&, double&) const;
  virtual VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const {
    double e;
    return this->Evaluate(a, b, c, d, e);
  }

  virtual double Energy(unsigned int a, unsigned int b, const ParticleSystem* c) const {
    double e, d;
    this->Evaluate(a, b, c, d, e);
    return e;
  }

  /** Write the first n modes to +- 3 std dev and the mean of the model
      described by the covariance matrix.  The string argument is a prefix to
      the file names. */
  void WriteModes(const std::string&, int) const;

  /**Access the shape matrix. */
  void SetShapeMatrix(ShapeMatrixType* s) { m_ShapeMatrix = s; }
  ShapeMatrixType* GetShapeMatrix() { return m_ShapeMatrix.GetPointer(); }
  const ShapeMatrixType* GetShapeMatrix() const { return m_ShapeMatrix.GetPointer(); }

  /** Called before each iteration of a solver. */
  virtual void BeforeIteration() {
    m_ShapeMatrix->BeforeIteration();

    if (m_Counter == 0) {
      this->ComputeCovarianceMatrix();
    }
  }

  /** Called after each iteration of the solver. */
  virtual void AfterIteration() {
    m_ShapeMatrix->AfterIteration();
    // Update the annealing parameter.
    if (m_HoldMinimumVariance != true && !m_UseMeanEnergy) {
      m_Counter++;
      if (m_Counter >= m_RecomputeCovarianceInterval) {
        m_Counter = 0;
        m_MinimumVariance *= m_MinimumVarianceDecayConstant;
      }
    }
  }

  /** */
  void SetMinimumVariance(double d) { m_MinimumVariance = d; }
  double GetMinimumVariance() const { return m_MinimumVariance; }

  void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period) {
    m_MinimumVarianceDecayConstant = exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance = initial_value;
    m_HoldMinimumVariance = false;
  }
  bool GetMinimumVarianceDecayConstant() const { return m_MinimumVarianceDecayConstant; }

  void PrintShapeMatrix() { m_ShapeMatrix->PrintMatrix(); }

  void UseMeanEnergy() { m_UseMeanEnergy = true; }
  void UseEntropy() { m_UseMeanEnergy = false; }

  /** */
  bool GetHoldMinimumVariance() const { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b) { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i) { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const { return m_RecomputeCovarianceInterval; }

  std::shared_ptr<VectorFunction> Clone() override {
    auto copy = LegacyCorrespondenceFunction::New();

    copy->m_PointsUpdate = this->m_PointsUpdate;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumEigenValue = this->m_MinimumEigenValue;
    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;
    copy->m_ShapeMatrix = this->m_ShapeMatrix;

    copy->m_InverseCovMatrix = this->m_InverseCovMatrix;
    copy->m_points_mean = this->m_points_mean;
    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;

    return copy;
  }

  LegacyCorrespondenceFunction() {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;  // log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;
    m_PointsUpdate = std::make_shared<vnl_matrix_type>(10, 10);
    m_InverseCovMatrix = std::make_shared<Eigen::MatrixXd>(10, 10);
    m_points_mean = std::make_shared<vnl_matrix_type>(10, 10);
  }
  ~LegacyCorrespondenceFunction() override = default;

 protected:
  LegacyCorrespondenceFunction(const LegacyCorrespondenceFunction&) = delete;
  LegacyCorrespondenceFunction& operator=(const LegacyCorrespondenceFunction&) = delete;
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  virtual void ComputeCovarianceMatrix();
  std::shared_ptr<vnl_matrix_type> m_PointsUpdate;
  double m_MinimumVariance;
  double m_MinimumEigenValue;
  double m_CurrentEnergy;
  bool m_HoldMinimumVariance;
  double m_MinimumVarianceDecayConstant;
  int m_RecomputeCovarianceInterval;
  int m_Counter;
  bool m_UseMeanEnergy;

  std::shared_ptr<vnl_matrix_type> m_points_mean;       // 3Nx3N - used for energy computation
  std::shared_ptr<Eigen::MatrixXd> m_InverseCovMatrix;  // 3NxM - used for energy computation
};

}  // namespace shapeworks
