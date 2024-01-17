---
title: Libs/Optimize/Matrix/LinearRegressionShapeMatrix.h

---

# Libs/Optimize/Matrix/LinearRegressionShapeMatrix.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LinearRegressionShapeMatrix](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md)**  |




## Source code

```cpp
#pragma once

#include "LegacyShapeMatrix.h"
#include "ParticleSystem.h"
#include "vnl/vnl_vector.h"

namespace shapeworks {
class LinearRegressionShapeMatrix : public LegacyShapeMatrix {
 public:
  typedef double DataType;
  typedef LinearRegressionShapeMatrix Self;
  typedef LegacyShapeMatrix Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(LinearRegressionShapeMatrix, LegacyShapeMatrix);

  void UpdateMeanMatrix() {
    // for each sample
    for (unsigned int i = 0; i < m_MeanMatrix.cols(); i++) {
      // compute the mean
      m_MeanMatrix.set_column(i, m_Intercept + m_Slope * m_Expl(i));
    }
  }

  inline vnl_vector<double> ComputeMean(double k) const { return m_Intercept + m_Slope * k; }

  void ResizeParameters(unsigned int n) {
    vnl_vector<double> tmpA = m_Intercept;  // copy existing  matrix
    vnl_vector<double> tmpB = m_Slope;      // copy existing  matrix

    // Create new
    m_Intercept.set_size(n);
    m_Slope.set_size(n);

    // Copy old data into new vector.
    for (unsigned int r = 0; r < tmpA.size(); r++) {
      m_Intercept(r) = tmpA(r);
      m_Slope(r) = tmpB(r);
    }
  }

  virtual void ResizeMeanMatrix(int rs, int cs) {
    vnl_matrix<double> tmp = m_MeanMatrix;  // copy existing  matrix

    // Create new column (shape)
    m_MeanMatrix.set_size(rs, cs);

    m_MeanMatrix.fill(0.0);

    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++) {
      for (unsigned int r = 0; r < tmp.rows(); r++) {
        m_MeanMatrix(r, c) = tmp(r, c);
      }
    }
  }

  void ResizeExplanatory(unsigned int n) {
    if (n > m_Expl.size()) {
      vnl_vector<double> tmp = m_Expl;  // copy existing  matrix

      // Create new
      m_Expl.set_size(n);
      m_Expl.fill(0.0);

      // Copy old data into new vector.
      for (unsigned int r = 0; r < tmp.size(); r++) {
        m_Expl(r) = tmp(r);
      }
    }
  }

  virtual void DomainAddEventCallback(Object*, const itk::EventObject& e) {
    const ParticleDomainAddEvent& event = dynamic_cast<const ParticleDomainAddEvent&>(e);
    unsigned int d = event.GetDomainIndex();

    if (d % this->m_DomainsPerShape == 0) {
      this->ResizeMatrix(this->rows(), this->cols() + 1);
      this->ResizeMeanMatrix(this->rows(), this->cols() + 1);
      this->ResizeExplanatory(this->cols());
    }
  }

  virtual void PositionAddEventCallback(Object* o, const itk::EventObject& e) {
    const ParticlePositionAddEvent& event = dynamic_cast<const ParticlePositionAddEvent&>(e);
    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);

    const unsigned int PointsPerDomain = ps->GetNumberOfParticles(d);

    // Make sure we have enough rows.
    if ((ps->GetNumberOfParticles(d) * 3 * this->m_DomainsPerShape) > this->rows()) {
      this->ResizeParameters(PointsPerDomain * 3 * this->m_DomainsPerShape);
      this->ResizeMatrix(PointsPerDomain * 3 * this->m_DomainsPerShape, this->cols());
      this->ResizeMeanMatrix(PointsPerDomain * 3 * this->m_DomainsPerShape, this->cols());
    }

    // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN
    // Add position info to the matrix
    unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * 3) + (idx * 3);
    for (unsigned int i = 0; i < 3; i++) {
      this->operator()(i + k, d / this->m_DomainsPerShape) = pos[i];
    }

    //   std::cout << "Row " << k << " Col " << d / this->m_DomainsPerShape << " = " << pos << std::endl;
  }

  virtual void PositionSetEventCallback(Object* o, const itk::EventObject& e) {
    const ParticlePositionSetEvent& event = dynamic_cast<const ParticlePositionSetEvent&>(e);

    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);
    const unsigned int PointsPerDomain = ps->GetNumberOfParticles(d);

    // Modify matrix info
    //    unsigned int k = 3 * idx;
    unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * 3) + (idx * 3);

    for (unsigned int i = 0; i < 3; i++) {
      this->operator()(i + k, d / this->m_DomainsPerShape) = pos[i] - m_MeanMatrix(i + k, d / this->m_DomainsPerShape);
    }
  }

  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

  void SetDomainsPerShape(int i) { this->m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return this->m_DomainsPerShape; }

  void SetExplanatory(std::vector<double> v) {
    ResizeExplanatory(v.size());
    for (unsigned int i = 0; i < v.size(); i++) {
      m_Expl[i] = v[i];
    }
  }
  void SetExplanatory(unsigned int i, double q) { m_Expl[i] = q; }
  const double& GetExplanatory(unsigned int i) const { return m_Expl[i]; }
  double& GetExplanatory(unsigned int i) { return m_Expl[i]; }

  const vnl_vector<double>& GetSlope() const { return m_Slope; }
  const vnl_vector<double>& GetIntercept() const { return m_Intercept; }

  void SetSlope(const std::vector<double>& v) {
    ResizeParameters(v.size());
    for (unsigned int i = 0; i < v.size(); i++) {
      m_Slope[i] = v[i];
    }
  }

  void SetIntercept(const std::vector<double>& v) {
    ResizeParameters(v.size());
    for (unsigned int i = 0; i < v.size(); i++) {
      m_Intercept[i] = v[i];
    }
  }

  void EstimateParameters() {
    //    std::cout << "Estimating params" << std::endl;
    //    std::cout << "Explanatory: " << m_Expl << std::endl;

    vnl_matrix<double> X = *this + m_MeanMatrix;

    // Number of samples
    double n = static_cast<double>(X.cols());

    vnl_vector<double> sumtx = m_Expl[0] * X.get_column(0);
    vnl_vector<double> sumx = X.get_column(0);
    double sumt = m_Expl[0];
    double sumt2 = m_Expl[0] * m_Expl[0];
    for (unsigned int k = 1; k < X.cols(); k++)  // k is the sample number
    {
      sumtx += m_Expl[k] * X.get_column(k);
      sumx += X.get_column(k);
      sumt += m_Expl[k];
      sumt2 += m_Expl[k] * m_Expl[k];
    }

    m_Slope = (n * sumtx - (sumx * sumt)) / (n * sumt2 - (sumt * sumt));

    vnl_vector<double> sumbt = m_Slope * m_Expl[0];
    for (unsigned int k = 1; k < X.cols(); k++) {
      sumbt += m_Slope * m_Expl[k];
    }

    m_Intercept = (sumx - sumbt) / n;
  }

  //
  void Initialize() {
    m_Intercept.fill(0.0);
    m_Slope.fill(0.0);
    m_MeanMatrix.fill(0.0);
  }

  virtual void BeforeIteration() {
    m_UpdateCounter++;
    if (m_UpdateCounter >= m_RegressionInterval) {
      m_UpdateCounter = 0;
      this->EstimateParameters();
      this->UpdateMeanMatrix();
    }
  }

  void SetRegressionInterval(int i) { m_RegressionInterval = i; }
  int GetRegressionInterval() const { return m_RegressionInterval; }

 protected:
  LinearRegressionShapeMatrix() {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
    m_UpdateCounter = 0;
    m_RegressionInterval = 1;
  }
  virtual ~LinearRegressionShapeMatrix(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  LinearRegressionShapeMatrix(const Self&);  // purposely not implemented
  void operator=(const Self&);                                // purposely not implemented

  int m_UpdateCounter;
  int m_RegressionInterval;

  // Parameters for the linear model
  vnl_vector<double> m_Intercept;
  vnl_vector<double> m_Slope;

  // The explanatory variable value for each sample (matrix column)
  vnl_vector<double> m_Expl;

  // A matrix to store the mean estimated for each explanatory variable (each sample)
  vnl_matrix<double> m_MeanMatrix;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-01-17 at 16:55:16 +0000
