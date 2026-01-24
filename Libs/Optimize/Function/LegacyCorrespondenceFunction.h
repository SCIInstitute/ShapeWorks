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
  virtual VectorType evaluate(unsigned int, unsigned int, const ParticleSystem*, double&, double&) const;
  virtual VectorType evaluate(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const {
    double e;
    return this->evaluate(a, b, c, d, e);
  }

  virtual double energy(unsigned int a, unsigned int b, const ParticleSystem* c) const {
    double e, d;
    this->evaluate(a, b, c, d, e);
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
  virtual void before_iteration() {
    m_ShapeMatrix->before_iteration();

    if (m_Counter == 0) {
      this->ComputeCovarianceMatrix();
    }
  }

  /** Called after each iteration of the solver. */
  virtual void after_iteration() override {
    m_ShapeMatrix->after_iteration();
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

  void UseMeanenergy() { m_UseMeanEnergy = true; }
  void UseEntropy() { m_UseMeanEnergy = false; }

  /** */
  bool GetHoldMinimumVariance() const { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b) { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i) { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const { return m_RecomputeCovarianceInterval; }

  /**
   * @brief Precompute data for fixed shape space optimization.
   * When most shapes are fixed (e.g., adding 1 new shape to 200 existing),
   * we can precompute the covariance structure from fixed shapes and only
   * update for the new shape, avoiding expensive SVD recomputation.
   */
  void PrecomputeForFixedDomains(const ParticleSystem* ps);

  /**
   * @brief Check if precomputed fixed domain data is available.
   */
  bool HasPrecomputedFixedDomains() const { return m_HasPrecomputedFixedDomains; }

  /**
   * @brief Clear precomputed fixed domain data.
   */
  void ClearPrecomputedFixedDomains();

  /**
   * @brief Request lazy precomputation on first iteration.
   * The actual precomputation will happen on the first call to ComputeCovarianceMatrix()
   * when the shape matrix is guaranteed to be populated.
   */
  void RequestPrecomputeForFixedDomains() { m_NeedsPrecomputation = true; }

  std::shared_ptr<VectorFunction> clone() override {
    auto copy = LegacyCorrespondenceFunction::New();

    copy->m_PointsUpdate = this->m_PointsUpdate;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumEigenValue = this->m_MinimumEigenValue;
    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;

    copy->domain_number_ = this->domain_number_;
    copy->particle_system_ = this->particle_system_;
    copy->m_ShapeMatrix = this->m_ShapeMatrix;

    copy->m_points_mean = this->m_points_mean;
    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;

    // Copy fixed shape space members
    copy->m_NeedsPrecomputation = this->m_NeedsPrecomputation;
    copy->m_HasPrecomputedFixedDomains = this->m_HasPrecomputedFixedDomains;
    copy->m_PrecomputedNumFixedSamples = this->m_PrecomputedNumFixedSamples;
    copy->m_PrecomputedNumDims = this->m_PrecomputedNumDims;
    copy->m_FixedY = this->m_FixedY;
    copy->m_FixedMean = this->m_FixedMean;
    copy->m_FixedPinvMat = this->m_FixedPinvMat;
    copy->m_FixedGradientBasis = this->m_FixedGradientBasis;
    copy->m_FixedW = this->m_FixedW;
    copy->m_FixedDomainIndices = this->m_FixedDomainIndices;

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

  std::shared_ptr<vnl_matrix_type> m_points_mean;

  // Fixed shape space optimization members
  bool m_NeedsPrecomputation = false;
  bool m_HasPrecomputedFixedDomains = false;
  int m_PrecomputedNumFixedSamples = 0;
  int m_PrecomputedNumDims = 0;
  std::shared_ptr<vnl_matrix_type> m_FixedY;           // Centered fixed shape data (dM × N_fixed)
  std::shared_ptr<vnl_matrix_type> m_FixedMean;        // Mean of fixed shapes (dM × 1)
  std::shared_ptr<vnl_matrix_type> m_FixedPinvMat;     // Regularized pseudoinverse (N_fixed × N_fixed)
  std::shared_ptr<vnl_matrix_type> m_FixedGradientBasis;  // Y_fixed × pinvMat (dM × N_fixed)
  std::shared_ptr<vnl_diag_matrix<double>> m_FixedW;   // Singular values
  std::vector<int> m_FixedDomainIndices;               // Indices of fixed shapes
};

}  // namespace shapeworks
