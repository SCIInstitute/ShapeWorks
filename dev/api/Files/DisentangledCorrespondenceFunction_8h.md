---
title: Libs/Optimize/Function/DisentangledCorrespondenceFunction.h

---

# Libs/Optimize/Function/DisentangledCorrespondenceFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md)**  |




## Source code

```cpp
#pragma once
#include <vector>
#include "DisentangledCorrespondenceFunction.h"
#include "Libs/Optimize/Matrix/LegacyShapeMatrix.h"
#include "VectorFunction.h"

namespace shapeworks {

class DisentangledCorrespondenceFunction : public VectorFunction {
 public:
  constexpr static unsigned int VDimension = 3;
  typedef DisentangledCorrespondenceFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(DisentangledCorrespondenceFunction, VectorFunction);

  typedef LegacyShapeMatrix ShapeMatrixType;

  typedef typename ShapeMatrixType::DataType DataType;

  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;
  typedef std::shared_ptr<std::vector<vnl_matrix_type>> shared_vnl_matrix_array_type;

  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

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

  void WriteModes(const std::string&, int) const;

  void SetShapeMatrix(ShapeMatrixType* s) { m_ShapeMatrix = s; }
  ShapeMatrixType* GetShapeMatrix() { return m_ShapeMatrix.GetPointer(); }
  const ShapeMatrixType* GetShapeMatrix() const { return m_ShapeMatrix.GetPointer(); }

  virtual void BeforeIteration() {
    m_ShapeMatrix->BeforeIteration();

    if (m_Counter == 0) {
      this->ComputeCovarianceMatrices();
    }
  }

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

  bool GetHoldMinimumVariance() const { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b) { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i) { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const { return m_RecomputeCovarianceInterval; }

  virtual VectorFunction::Pointer Clone() {
    DisentangledCorrespondenceFunction::Pointer copy = DisentangledCorrespondenceFunction::New();

    copy->m_Shape_PointsUpdate = this->m_Shape_PointsUpdate;
    copy->m_Time_PointsUpdate = this->m_Time_PointsUpdate;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumEigenValue_shape_cohort = this->m_MinimumEigenValue_shape_cohort;
    copy->m_MinimumEigenValue_time_cohort = this->m_MinimumEigenValue_time_cohort;

    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;
    copy->m_ShapeMatrix = this->m_ShapeMatrix;


    copy->m_InverseCovMatrices_time_cohort = this->m_InverseCovMatrices_time_cohort;
    copy->m_InverseCovMatrices_shape_cohort = this->m_InverseCovMatrices_shape_cohort;

    copy->m_points_mean_time_cohort = this->m_points_mean_time_cohort;
    copy->m_points_mean_shape_cohort = this->m_points_mean_shape_cohort;

    return (VectorFunction::Pointer)copy;
  }

 protected:
  DisentangledCorrespondenceFunction() {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;  // log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;
    m_InverseCovMatrices_time_cohort = std::make_shared<std::vector<Eigen::MatrixXd>>();
    m_InverseCovMatrices_shape_cohort = std::make_shared<std::vector<Eigen::MatrixXd>>();
    m_points_mean_time_cohort = std::make_shared<std::vector<vnl_matrix_type>>();
    m_points_mean_shape_cohort = std::make_shared<std::vector<vnl_matrix_type>>();
    m_Time_PointsUpdate = std::make_shared<std::vector<vnl_matrix_type>>();
    m_Shape_PointsUpdate = std::make_shared<std::vector<vnl_matrix_type>>();
  }
  virtual ~DisentangledCorrespondenceFunction() {}
  void operator=(const DisentangledCorrespondenceFunction&);
  DisentangledCorrespondenceFunction(const DisentangledCorrespondenceFunction&);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  // Computes Covariance Matrices across time and shape domain and then generate gradient updates for them.
  virtual void ComputeCovarianceMatrices();

  // Initialize size and clear relevant variables from previous iteration, before gradient updates computation.
  void Initialize(){
    const unsigned int total_time_points = m_ShapeMatrix->GetDomainsPerShape();
    int total_subjects = m_ShapeMatrix->cols();
    m_points_mean_time_cohort->clear();
    m_points_mean_shape_cohort->clear();
    m_InverseCovMatrices_time_cohort->clear();
    m_InverseCovMatrices_shape_cohort->clear();
    m_Shape_PointsUpdate->clear();
    m_Time_PointsUpdate->clear();
    m_MinimumEigenValue_shape_cohort.resize(total_subjects, 0.0);
    m_MinimumEigenValue_time_cohort.resize(total_time_points, 0.0);
    for(int i = 0; i < total_time_points; ++i){
      Eigen::MatrixXd temp_cov_matrix;
      vnl_matrix_type temp_points_matrix;
      vnl_matrix_type temp_mean_matrix;
      m_InverseCovMatrices_time_cohort->push_back(temp_cov_matrix);
      m_points_mean_time_cohort->push_back(temp_mean_matrix);
      m_Time_PointsUpdate->push_back(temp_points_matrix);
    }
    for(int i = 0; i < total_subjects; ++i){
      Eigen::MatrixXd temp_cov_matrix;
      vnl_matrix_type temp_points_matrix;
      vnl_matrix_type temp_mean_matrix;
      m_InverseCovMatrices_shape_cohort->push_back(temp_cov_matrix);
      m_points_mean_shape_cohort->push_back(temp_mean_matrix);
      m_Shape_PointsUpdate->push_back(temp_points_matrix);
    }
  }

  double m_MinimumVariance;
  double m_MinimumEigenValue;
  std::vector<double> m_MinimumEigenValue_time_cohort;
  std::vector<double> m_MinimumEigenValue_shape_cohort;
  double m_CurrentEnergy;
  bool m_HoldMinimumVariance;
  double m_MinimumVarianceDecayConstant;
  int m_RecomputeCovarianceInterval;
  int m_Counter;
  bool m_UseMeanEnergy;

  // Inverse Covariance matrices across time and shape cohort
  std::shared_ptr<std::vector<Eigen::MatrixXd>> m_InverseCovMatrices_time_cohort; // T obj matrices each of dimensionality dM X N
  std::shared_ptr<std::vector<Eigen::MatrixXd>> m_InverseCovMatrices_shape_cohort; // N obj matrices each of dimensionality dM X T
  // mean vectors across time and shape cohort
  shared_vnl_matrix_array_type m_points_mean_time_cohort; // T mean vectors each of dimension dM
  shared_vnl_matrix_array_type m_points_mean_shape_cohort; // N mean vectors each of dimension dM
  // Matrices for Gradient Updates across time and shape cohort
  shared_vnl_matrix_array_type m_Time_PointsUpdate; // T update matrices each of dimensionality dM X N
  shared_vnl_matrix_array_type m_Shape_PointsUpdate; // N update matrices each of dimensionality dM X T
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-01-27 at 17:49:27 +0000
