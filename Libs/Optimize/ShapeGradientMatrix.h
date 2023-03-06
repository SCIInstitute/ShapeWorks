#pragma once

#include "ParticleImageDomainWithGradN.h"
#include "ParticleImageDomainWithGradients.h"
#include "ParticleImplicitSurfaceDomain.h"
#include "itkDataObject.h"
#include "itkParticleAttribute.h"
#include "itkParticleContainer.h"
#include "itkParticleSystem.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"

namespace shapeworks {
/** \class ShapeGradientMatrix
 *
 * \brief Each column describes a shape.  A shape may be composed of
 * m_DomainsPerShape domains (default 1). ALL DOMAINS ARE *NOT* ASSUMED TO HAVE THE
 * SAME NUMBER OF PARTICLES!
 *
 *
 * Each column represents a single shape.
 */
class ShapeGradientMatrix : public vnl_matrix<double>, public itk::ParticleAttribute {
 public:
  /** Standard class typedefs */
  typedef double DataType;
  typedef ShapeGradientMatrix Self;
  typedef ParticleAttribute Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  typedef itk::ParticleSystem ParticleSystemType;
  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  /** Run-time type information (and related methods). */
  itkTypeMacro(ShapeGradientMatrix, ParticleAttribute)

  virtual void BeforeIteration() {}
  virtual void AfterIteration() {}

  /** Set/Get the number of domains per shape.  This can only be safely done
    before shapes are initialized with points! */
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

  void SetValues(const ParticleSystemType* ps, int idx, int d);

  virtual void DomainAddEventCallback(Object*, const itk::EventObject& e) {
    const itk::ParticleDomainAddEvent& event = dynamic_cast<const itk::ParticleDomainAddEvent&>(e);
    unsigned int d = event.GetDomainIndex();

    if (d % m_DomainsPerShape == 0) this->ResizeMatrix(this->rows(), this->cols() + 3);  // 3 columns for every shape
  }

  virtual void PositionAddEventCallback(Object* o, const itk::EventObject& e) {
    // update the size of matrix based on xyz, normals and number of attributes being used
    const itk::ParticlePositionAddEvent& event = dynamic_cast<const itk::ParticlePositionAddEvent&>(e);
    const itk::ParticleSystem* ps = dynamic_cast<const itk::ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    int numRows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++) {
      if (m_use_xyz[i]) {
        numRows += 3 * ps->GetNumberOfParticles(i);
      }
      if (m_use_normals[i]) {
        numRows += 3 * ps->GetNumberOfParticles(i);
      }
      numRows += m_AttributesPerDomain[i] * ps->GetNumberOfParticles(i);
    }

    if (numRows > this->rows()) {
      this->ResizeMatrix(numRows, this->cols());
    }

    this->SetValues(ps, idx, d);
  }

  virtual void PositionSetEventCallback(Object* o, const itk::EventObject& e) {
    // update xyz, normals and number of attributes being used
    const itk::ParticlePositionSetEvent& event = dynamic_cast<const itk::ParticlePositionSetEvent&>(e);
    const itk::ParticleSystem* ps = dynamic_cast<const itk::ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    this->SetValues(ps, idx, d);
  }

  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

 protected:
  ShapeGradientMatrix() {
    m_DomainsPerShape = 1;

    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~ShapeGradientMatrix() {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

  int m_DomainsPerShape;

 private:
  ShapeGradientMatrix(const Self&);  // purposely not implemented
  void operator=(const Self&);       // purposely not implemented

  std::vector<bool> m_use_xyz;
  std::vector<bool> m_use_normals;
  std::vector<int> m_AttributesPerDomain;
  std::vector<double> m_AttributeScales;

};  // end class

}  // namespace itk
