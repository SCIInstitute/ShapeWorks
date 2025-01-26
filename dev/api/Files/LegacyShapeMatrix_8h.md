---
title: Libs/Optimize/Matrix/LegacyShapeMatrix.h

---

# Libs/Optimize/Matrix/LegacyShapeMatrix.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md)**  |




## Source code

```cpp
#pragma once

#include <cmath>

#include "Libs/Optimize/Container/GenericContainer.h"
#include "Observer.h"
#include "ParticleSystem.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
class LegacyShapeMatrix : public vnl_matrix<double>, public Observer {
 public:
  typedef double DataType;
  typedef LegacyShapeMatrix Self;
  typedef Observer Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self)

      
      itkTypeMacro(LegacyShapeMatrix, Observer)

      
      virtual void DomainAddEventCallback(Object*, const itk::EventObject& e) {
    const ParticleDomainAddEvent& event = dynamic_cast<const ParticleDomainAddEvent&>(e);
    unsigned int d = event.GetDomainIndex();

    if (d % m_DomainsPerShape == 0) this->ResizeMatrix(this->rows(), this->cols() + 1);
  }

  virtual void ResizeMatrix(int rs, int cs) {
    vnl_matrix<double> tmp(*this);  // copy existing  matrix

    // Create new column (shape)
    this->set_size(rs, cs);

    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++) {
      for (unsigned int r = 0; r < tmp.rows(); r++) this->operator()(r, c) = tmp(r, c);
    }
  }

  virtual void PositionAddEventCallback(Object* o, const itk::EventObject& e) {
    const int VDimension = 3;
    const ParticlePositionAddEvent& event = dynamic_cast<const ParticlePositionAddEvent&>(e);
    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);

    int numRows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++) {
      numRows += VDimension * ps->GetNumberOfParticles(i);
    }

    if (numRows > this->rows()) {
      this->ResizeMatrix(numRows, this->cols());
    }

    unsigned int k = 0;
    int dom = d % m_DomainsPerShape;
    for (int i = 0; i < dom; i++) {
      k += VDimension * ps->GetNumberOfParticles(i);
    }
    k += idx * VDimension;

    for (unsigned int i = 0; i < VDimension; i++) {
      if (i + k >= this->rows()) {
        throw std::runtime_error(
            "PositionSetEventCallback: index out of bounds! Different number of particles per shape?");
      }
      this->operator()(i + k, d / m_DomainsPerShape) = pos[i];
    }
  }

  virtual void PositionSetEventCallback(Object* o, const itk::EventObject& e) {
    const int VDimension = 3;
    const ParticlePositionSetEvent& event = dynamic_cast<const ParticlePositionSetEvent&>(e);
    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);

    unsigned int k = 0;
    int dom = d % m_DomainsPerShape;
    for (int i = 0; i < dom; i++) {
      k += VDimension * ps->GetNumberOfParticles(i);
    }
    k += idx * VDimension;

    for (unsigned int i = 0; i < VDimension; i++) {
      if (i + k >= this->rows()) {
        throw std::runtime_error(
            "PositionSetEventCallback: index out of bounds! Different number of particles per shape?");
      }
      this->operator()(i + k, d / m_DomainsPerShape) = pos[i];
    }
  }

  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

  // For debugging purposes.
  void PrintMatrix() {
    std::cout << std::endl;
    for (unsigned int r = 0; r < this->rows(); r++) {
      for (unsigned int c = 0; c < this->cols(); c++) {
        std::cout << this->operator()(r, c) << "  ";
      }
      std::cout << std::endl;
    }
  }

  bool CheckForNans() {
    bool flag = false;
    for (unsigned int r = 0; r < this->rows(); r++) {
      for (unsigned int c = 0; c < this->cols(); c++) {
        if (std::isnan(this->operator()(r, c))) {
          flag = true;
          break;
        }
      }
    }
    return flag;
  }

  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  virtual void BeforeIteration() {}
  virtual void AfterIteration() {}

  virtual void SetMatrix(const vnl_matrix<double>& m) { vnl_matrix<double>::operator=(m); }

 protected:
  LegacyShapeMatrix() : m_DomainsPerShape(1) {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~LegacyShapeMatrix() {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

  int m_DomainsPerShape;

 private:
  LegacyShapeMatrix(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-01-26 at 18:42:36 +0000
