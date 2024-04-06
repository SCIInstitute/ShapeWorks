---
title: Libs/Optimize/Function/CorrespondenceFunction.h

---

# Libs/Optimize/Function/CorrespondenceFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md)** <br>Correspondence term.  |




## Source code

```cpp
#pragma once

#include <cmath>
#include <numeric>
#include <vector>

#include "Libs/Optimize/Matrix/ShapeGradientMatrix.h"
#include "Libs/Optimize/Matrix/ShapeMatrix.h"
#include "VectorFunction.h"

namespace shapeworks {

class CorrespondenceFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef CorrespondenceFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(CorrespondenceFunction, VectorFunction)


  typedef shapeworks::ShapeMatrix ShapeDataType;
  typedef shapeworks::ShapeGradientMatrix ShapeGradientType;

  typedef ShapeDataType::DataType DataType;

  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;

  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  void SetShapeData(ShapeDataType* s) { m_ShapeData = s; }
  ShapeDataType* GetShapeData() { return m_ShapeData.GetPointer(); }
  const ShapeDataType* GetShapeData() const { return m_ShapeData.GetPointer(); }

  void SetShapeGradient(ShapeGradientType* s) { m_ShapeGradient = s; }
  ShapeGradientType* GetShapeGradient() { return m_ShapeGradient.GetPointer(); }
  const ShapeGradientType* GetShapeGradient() const { return m_ShapeGradient.GetPointer(); }

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

  virtual void BeforeIteration() { this->ComputeUpdates(this->m_ParticleSystem); }

  virtual void AfterIteration() {
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

  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  void SetAttributesPerDomain(const std::vector<int>& i) { m_AttributesPerDomain = i; }

  void UseMeanEnergy() { m_UseMeanEnergy = true; }
  void UseEntropy() { m_UseMeanEnergy = false; }

  void SetXYZ(int i, bool val) {
    if (m_UseXYZ.size() != m_DomainsPerShape) m_UseXYZ.resize(m_DomainsPerShape);
    m_UseXYZ[i] = val;
  }
  void SetNormals(int i, bool val) {
    if (m_UseNormals.size() != m_DomainsPerShape) m_UseNormals.resize(m_DomainsPerShape);
    m_UseNormals[i] = val;
  }

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

  virtual VectorFunction::Pointer Clone() {
    auto copy = CorrespondenceFunction::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

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
    copy->m_InverseCovMatrix = this->m_InverseCovMatrix;

    copy->m_ShapeData = this->m_ShapeData;
    copy->m_ShapeGradient = this->m_ShapeGradient;

    return (VectorFunction::Pointer)copy;
  }

 protected:
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
    m_InverseCovMatrix = std::make_shared<Eigen::MatrixXd>(10, 10);
    m_points_mean = std::make_shared<vnl_matrix_type>(10, 10);
  }
  virtual ~CorrespondenceFunction() {}
  void operator=(const CorrespondenceFunction&);
  CorrespondenceFunction(const CorrespondenceFunction&);

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
  std::shared_ptr<Eigen::MatrixXd> m_InverseCovMatrix;
  int num_dims, num_samples;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-04-06 at 14:52:15 +0000
