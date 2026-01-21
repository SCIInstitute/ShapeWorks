#pragma once

#include <cmath>
#include <numeric>
#include <vector>

#include <vnl/vnl_diag_matrix.h>

#include "Libs/Optimize/Matrix/ShapeGradientMatrix.h"
#include "Libs/Optimize/Matrix/ShapeMatrix.h"
#include "VectorFunction.h"

namespace shapeworks {

//! Correspondence term
class CorrespondenceFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  constexpr static unsigned int Dimension = VDimension;

  typedef shapeworks::ShapeMatrix ShapeDataType;
  typedef shapeworks::ShapeGradientMatrix ShapeGradientType;

  typedef ShapeDataType::DataType DataType;

  /** Vector & Point types. */
  typedef VectorFunction::VectorType VectorType;
  typedef ParticleSystem::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;

  /// Factory method for creating instances
  static std::shared_ptr<CorrespondenceFunction> New() {
    return std::make_shared<CorrespondenceFunction>();
  }

  //! Access the shape matrix
  void SetShapeData(ShapeDataType* s) { m_ShapeData = s; }
  ShapeDataType* GetShapeData() { return m_ShapeData.GetPointer(); }
  const ShapeDataType* GetShapeData() const { return m_ShapeData.GetPointer(); }

  //! Access the shape gradient matrix
  void SetShapeGradient(ShapeGradientType* s) { m_ShapeGradient = s; }
  ShapeGradientType* GetShapeGradient() { return m_ShapeGradient.GetPointer(); }
  const ShapeGradientType* GetShapeGradient() const { return m_ShapeGradient.GetPointer(); }

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

  /** Called before each iteration of a solver. */
  virtual void before_iteration() { this->ComputeUpdates(this->particle_system_); }

  /** Called after each iteration of the solver. */
  virtual void after_iteration() {
    // Update the annealing parameter.
    if (m_HoldMinimumVariance != true && !m_UseMeanEnergy) {
      m_Counter++;
      if (m_Counter >= m_RecomputeCovarianceInterval) {
        m_Counter = 0;
        m_MinimumVariance *= m_MinimumVarianceDecayConstant;
      }
    }
  }
  void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period) {
    m_MinimumVarianceDecayConstant = exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance = initial_value;
    m_HoldMinimumVariance = false;
  }

  void SetMinimumVariance(double d) { m_MinimumVariance = d; }
  double GetMinimumVariance() const { return m_MinimumVariance; }

  bool GetHoldMinimumVariance() const { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b) { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i) { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const { return m_RecomputeCovarianceInterval; }

  void SetAttributeScales(const std::vector<double>& s) { m_AttributeScales = s; }

  /** Set/Get the number of domains per shape. */
  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  void SetAttributesPerDomain(const std::vector<int>& i) { m_AttributesPerDomain = i; }

  void UseMeanenergy() { m_UseMeanEnergy = true; }
  void UseEntropy() { m_UseMeanEnergy = false; }

  void SetXYZ(int i, bool val) {
    if (m_UseXYZ.size() != m_DomainsPerShape) m_UseXYZ.resize(m_DomainsPerShape);
    m_UseXYZ[i] = val;
  }
  void SetNormals(int i, bool val) {
    if (m_UseNormals.size() != m_DomainsPerShape) m_UseNormals.resize(m_DomainsPerShape);
    m_UseNormals[i] = val;
  }

  /**
   * @brief Precompute data for fixed domains to accelerate optimization.
   *
   * When optimizing with fixed domains (e.g., adding 1 new shape to 99 existing),
   * this method precomputes the covariance structure from the fixed shapes.
   * Subsequent calls to ComputeUpdates() will use this precomputed data,
   * avoiding expensive recomputation of the SVD and related matrices.
   *
   * @param ps The particle system containing both fixed and non-fixed domains
   */
  void PrecomputeForFixedDomains(const ParticleSystem* ps);

  /**
   * @brief Check if precomputed fixed domain data is available.
   */
  bool HasPrecomputedFixedDomains() const { return m_HasPrecomputedFixedDomains; }

  /**
   * @brief Clear precomputed fixed domain data (e.g., if fixed domains change).
   */
  void ClearPrecomputedFixedDomains();

  bool CheckForNans(vnl_matrix_type mat) {
    bool flag = false;
    for (int i = 0; i < mat.rows(); i++) {
      for (int j = 0; j < mat.cols(); j++) {
        if (std::isnan(mat(i, j))) {
          flag = true;
          break;
        }
      }
    }
    return flag;
  }

  std::shared_ptr<VectorFunction> clone() override {
    auto copy = CorrespondenceFunction::New();

    // from itkParticleVectorFunction
    copy->domain_number_ = this->domain_number_;
    copy->particle_system_ = this->particle_system_;

    // local
    copy->m_AttributeScales = this->m_AttributeScales;
    copy->m_Counter = this->m_Counter;
    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumEigenValue = this->m_MinimumEigenValue;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_PointsUpdate = this->m_PointsUpdate;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_AttributesPerDomain = this->m_AttributesPerDomain;
    copy->m_DomainsPerShape = this->m_DomainsPerShape;
    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;
    copy->m_points_mean = this->m_points_mean;
    copy->m_UseNormals = this->m_UseNormals;
    copy->m_UseXYZ = this->m_UseXYZ;

    copy->m_ShapeData = this->m_ShapeData;
    copy->m_ShapeGradient = this->m_ShapeGradient;

    return copy;
  }

  CorrespondenceFunction() {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;  // log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;
    m_UseNormals.clear();
    m_UseXYZ.clear();
    num_dims = 0;
    num_samples = 0;
    m_PointsUpdate = std::make_shared<vnl_matrix_type>(10, 10);
    m_points_mean = std::make_shared<vnl_matrix_type>(10, 10);
  }
  ~CorrespondenceFunction() override = default;

 protected:
  CorrespondenceFunction(const CorrespondenceFunction&) = delete;
  CorrespondenceFunction& operator=(const CorrespondenceFunction&) = delete;

  typename ShapeDataType::Pointer m_ShapeData;
  typename ShapeGradientType::Pointer m_ShapeGradient;

  virtual void ComputeUpdates(const ParticleSystem* c);
  std::shared_ptr<vnl_matrix_type> m_PointsUpdate;

  double m_MinimumVariance;
  double m_MinimumEigenValue;
  //  double m_MinimumVarianceBase;
  bool m_HoldMinimumVariance;
  int m_RecomputeCovarianceInterval;
  double m_MinimumVarianceDecayConstant;
  int m_Counter;
  std::vector<double> m_AttributeScales;  // size \sum_i n_i
  int m_DomainsPerShape;
  std::vector<int> m_AttributesPerDomain;  // n
  double m_CurrentEnergy;
  bool m_UseMeanEnergy;
  std::vector<bool> m_UseXYZ;
  std::vector<bool> m_UseNormals;
  std::shared_ptr<vnl_matrix_type> m_points_mean;
  int num_dims, num_samples;

  // ============================================================================
  // Fixed Shape Space / Fixed Domain Precomputation
  // ============================================================================
  // When most domains are fixed and only a few are being optimized,
  // we can precompute the shape space (mean, pinvMat, gradient basis) from the fixed domains
  // and reuse it, avoiding expensive recomputation each iteration.

  bool m_HasPrecomputedFixedDomains = false;

  // Number of fixed and total samples when precomputation was done
  int m_PrecomputedNumFixedSamples = 0;
  int m_PrecomputedNumDims = 0;

  // Precomputed from fixed domains:
  // μ_fixed: mean of fixed shapes (dM × 1)
  std::shared_ptr<vnl_matrix_type> m_FixedMean;

  // Y_fixed: centered fixed shape data (dM × N_fixed)
  std::shared_ptr<vnl_matrix_type> m_FixedY;

  // pinvMat_fixed: (Y_fixed^T Y_fixed + αI)^{-1}, size (N_fixed × N_fixed)
  std::shared_ptr<vnl_matrix_type> m_FixedPinvMat;

  // Precomputed gradient basis: Y_fixed × pinvMat_fixed (dM × N_fixed)
  // This allows computing gradients for new shapes as linear combinations
  std::shared_ptr<vnl_matrix_type> m_FixedGradientBasis;

  // SVD components from fixed shapes (for potential reuse)
  std::shared_ptr<vnl_matrix_type> m_FixedU;       // U matrix from SVD (N_fixed × N_fixed)
  std::shared_ptr<vnl_diag_matrix<double>> m_FixedW;  // Singular values

  // Which domains are fixed (indices into the particle system)
  std::vector<int> m_FixedDomainIndices;
};
}  // namespace shapeworks
