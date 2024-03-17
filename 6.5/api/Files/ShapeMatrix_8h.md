---
title: Libs/Optimize/Matrix/ShapeMatrix.h

---

# Libs/Optimize/Matrix/ShapeMatrix.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md)** <br>Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES!  |




## Source code

```cpp
#pragma once

#include <cmath>

#include "Domain/MeshDomain.h"
#include "Libs/Optimize/Container/GenericContainer.h"
#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Domain/ImplicitSurfaceDomain.h"
#include "Observer.h"
#include "ParticleSystem.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"
namespace shapeworks {

class ShapeMatrix : public vnl_matrix<double>, public Observer {
 public:
  typedef double DataType;
  typedef ShapeMatrix Self;
  typedef Observer Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ShapeMatrix, Observer);

  virtual void BeforeIteration() {}
  virtual void AfterIteration() {}

  void SetDomainsPerShape(int i) { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const { return m_DomainsPerShape; }

  void SetAttributesPerDomain(const std::vector<int>& i) { m_AttributesPerDomain = i; }

  void SetAttributeScales(const std::vector<double>& s) { m_AttributeScales = s; }

  void SetXYZ(int i, bool val) {
    if (m_use_xyz.size() != m_DomainsPerShape) m_use_xyz.resize(m_DomainsPerShape);
    m_use_xyz[i] = val;
  }
  void SetNormals(int i, bool val) {
    if (m_use_normals.size() != m_DomainsPerShape) m_use_normals.resize(m_DomainsPerShape);
    m_use_normals[i] = val;
  }

  virtual void SetMatrix(const vnl_matrix<double>& m) { vnl_matrix<double>::operator=(m); }

  virtual void ResizeMatrix(int rs, int cs) {
    vnl_matrix<double> tmp(*this);  // copy existing  matrix

    // Create new column (shape)
    this->set_size(rs, cs);

    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++) {
      for (unsigned int r = 0; r < tmp.rows(); r++) this->operator()(r, c) = tmp(r, c);
    }
  }

  virtual void DomainAddEventCallback(Object*, const itk::EventObject& e) {
    const ParticleDomainAddEvent& event = dynamic_cast<const ParticleDomainAddEvent&>(e);
    unsigned int d = event.GetDomainIndex();

    if (d % m_DomainsPerShape == 0) this->ResizeMatrix(this->rows(), this->cols() + 1);
  }

  void SetValues(const ParticleSystem* ps, int idx, int d) {
    const int VDimension = 3;

    const typename ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);
    const typename ParticleSystem::PointType posLocal = ps->GetPosition(idx, d);
    unsigned int k = 0;
    int dom = d % m_DomainsPerShape;
    int num = 0;
    for (int i = 0; i < dom; i++) {
      if (m_use_xyz[i]) {
        k += VDimension * ps->GetNumberOfParticles(i);
        num += VDimension;
      }
      if (m_use_normals[i]) {
        k += VDimension * ps->GetNumberOfParticles(i);
        num += VDimension;
      }
      k += m_AttributesPerDomain[i] * ps->GetNumberOfParticles(i);
      num += m_AttributesPerDomain[i];
    }
    if (m_use_xyz[dom]) k += idx * VDimension;
    if (m_use_normals[dom]) k += idx * VDimension;
    k += idx * m_AttributesPerDomain[dom];

    int s = 0;
    if (m_use_xyz[dom]) {
      for (unsigned int i = 0; i < VDimension; i++) {
        this->operator()(i + k, d / m_DomainsPerShape) = pos[i] * m_AttributeScales[num + i + s];
      }
      k += VDimension;
      s += VDimension;
    }
    if (m_use_normals[dom]) {
      vnl_vector_fixed<float, DIMENSION> pN = ps->GetDomain(d)->SampleNormalAtPoint(posLocal, idx);
      ParticleSystem::VectorType tmp;
      tmp[0] = pN[0];
      tmp[1] = pN[1];
      tmp[2] = pN[2];
      tmp = ps->TransformVector(tmp, ps->GetTransform(d) * ps->GetPrefixTransform(d));
      pN[0] = tmp[0];
      pN[1] = tmp[1];
      pN[2] = tmp[2];
      pN = pN.normalize();  // contains scaling
      for (unsigned int i = 0; i < VDimension; i++) {
        this->operator()(i + k, d / m_DomainsPerShape) = pN[i] * m_AttributeScales[num + i + s];
      }
      k += VDimension;
      s += VDimension;
    }

    std::vector<float> feature_values;
    if (m_AttributesPerDomain[dom] > 0) {
      Point3 pt;
      pt[0] = posLocal[0];
      pt[1] = posLocal[1];
      pt[2] = posLocal[2];
      feature_values.clear();
      const MeshDomain* domain = static_cast<const MeshDomain*>(ps->GetDomain(d));

      auto mesh = domain->GetSWMesh();

      auto field_attributes = ps->GetFieldAttributes();

      for (int i = 0; i < field_attributes.size(); i++) {
        feature_values.push_back(mesh->interpolateFieldAtPoint(field_attributes[i], pt));
      }

      for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++) {
        this->operator()(aa + k, d / m_DomainsPerShape) = feature_values[aa] * m_AttributeScales[aa + num + s];
      }
    }
  }

  virtual void PositionAddEventCallback(Object* o, const itk::EventObject& e) {
    const int VDimension = 3;

    // update the size of matrix based on xyz, normals and number of attributes being used
    const ParticlePositionAddEvent& event = dynamic_cast<const ParticlePositionAddEvent&>(e);
    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    int numRows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++) {
      if (m_use_xyz[i]) numRows += VDimension * ps->GetNumberOfParticles(i);
      if (m_use_normals[i]) numRows += VDimension * ps->GetNumberOfParticles(i);
      numRows += m_AttributesPerDomain[i] * ps->GetNumberOfParticles(i);
    }

    if (numRows > this->rows()) this->ResizeMatrix(numRows, this->cols());

    this->SetValues(ps, idx, d);
  }

  virtual void PositionSetEventCallback(Object* o, const itk::EventObject& e) {
    // update xyz, normals and number of attributes being used
    const ParticlePositionSetEvent& event = dynamic_cast<const ParticlePositionSetEvent&>(e);
    const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    this->SetValues(ps, idx, d);
  }

  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

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

 protected:
  ShapeMatrix() {
    m_DomainsPerShape = 1;

    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~ShapeMatrix() {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

  int m_DomainsPerShape;

 private:
  ShapeMatrix(const Self&);     // purposely not implemented
  void operator=(const Self&);  // purposely not implemented

  std::vector<bool> m_use_xyz;
  std::vector<bool> m_use_normals;
  std::vector<int> m_AttributesPerDomain;
  std::vector<double> m_AttributeScales;

};  // end class

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-03-17 at 12:58:44 -0600
