#pragma once

#include "ParticleImageDomainWithGradN.h"
#include "ParticleImageDomainWithGradients.h"
#include "ParticleImplicitSurfaceDomain.h"
#include "TriMesh.h"
#include "itkDataObject.h"
#include "itkParticleAttribute.h"
#include "itkParticleContainer.h"
#include "itkParticleSystem.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"

namespace itk {
/** \class ParticleGeneralShapeGradientMatrix
 *
 * \brief Each column describes a shape.  A shape may be composed of
 * m_DomainsPerShape domains (default 1). ALL DOMAINS ARE *NOT* ASSUMED TO HAVE THE
 * SAME NUMBER OF PARTICLES!
 *
 *
 * Each column represents a single shape.
 */
class ParticleGeneralShapeGradientMatrix : public vnl_matrix<double>, public ParticleAttribute {
 public:
  /** Standard class typedefs */
  typedef double DataType;
  typedef ParticleGeneralShapeGradientMatrix Self;
  typedef ParticleAttribute Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  typedef ParticleSystem ParticleSystemType;
  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleGeneralShapeGradientMatrix, ParticleAttribute)

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

  void SetValues(const ParticleSystemType* ps, int idx, int d) {
    const typename itk::ParticleSystem::PointType posLocal = ps->GetPosition(idx, d);

    unsigned int k = 0;
    int dom = d % m_DomainsPerShape;
    int num = 0;
    for (int i = 0; i < dom; i++) {
      if (m_use_xyz[i]) {
        k += 3 * ps->GetNumberOfParticles(i);
        num += 3;
      }
      if (m_use_normals[i]) {
        k += 3 * ps->GetNumberOfParticles(i);
        num += 3;
      }
      k += m_AttributesPerDomain[i] * ps->GetNumberOfParticles(i);
      num += m_AttributesPerDomain[i];
    }
    if (m_use_xyz[dom]) k += idx * 3;
    if (m_use_normals[dom]) k += idx * 3;
    k += idx * m_AttributesPerDomain[dom];

    int s = 0;
    if (m_use_xyz[dom]) {
      for (unsigned int i = 0; i < 3; i++) {
        for (unsigned int j = 0; j < 3; j++) {
          if (i == j)
            this->operator()(i + k, j + 3 * (d / m_DomainsPerShape)) = 1.0 * m_AttributeScales[num + i];
          else
            this->operator()(i + k, j + 3 * (d / m_DomainsPerShape)) = 0.0;
        }
      }
      k += 3;
      s += 3;
    }
    if (m_use_normals[dom]) {
      if (ps->GetDomainFlag(d)) {
        for (unsigned int c = s; c < s + 3; c++) {
          for (unsigned int vd = 0; vd < 3; vd++)
            this->operator()(c - s + k, vd + 3 * (d / m_DomainsPerShape)) = 0.0 * m_AttributeScales[num + c];
        }
        s += 3;
        k += 3;
      } else {
        vnl_vector_fixed<float, DIMENSION> gradient = ps->GetDomain(d)->SampleGradientAtPoint(posLocal, idx);
        vnl_vector_fixed<float, DIMENSION> normal = gradient.normalize();
        float grad_mag = gradient.magnitude();  // TODO This is always 1.0. Fix when correcting image gradient of
                                                // normals
        typename shapeworks::ParticleDomain::GradNType grad_n = ps->GetDomain(d)->SampleGradNAtPoint(posLocal, idx);

        typename shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat1;
        mat1.set_identity();
        vnl_matrix<float> nrml(3, 1);
        nrml.fill(0.0);
        nrml(0, 0) = normal[0];
        nrml(1, 0) = normal[1];
        nrml(2, 0) = normal[2];
        typename shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat2 = nrml * nrml.transpose();

        for (unsigned int x1 = 0; x1 < 3; x1++) {
          for (unsigned int x2 = 0; x2 < 3; x2++) {
            mat1(x1, x2) -= mat2(x1, x2);
            grad_n(x1, x2) /= grad_mag;
          }
        }

        // mat3 = H/|grad_f| * (I - n*n');
        typename shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat3 = grad_n * mat1;
        typename itk::ParticleSystem::VnlMatrixType tmp;
        tmp.set_size(3, 3);
        tmp.fill(0.0);
        for (unsigned int c = 0; c < 3; c++) {
          for (unsigned vd = 0; vd < 3; vd++) tmp(c, vd) = mat3(c, vd);
        }

        tmp = ps->TransformNormalDerivative(tmp, ps->GetTransform(d) * ps->GetPrefixTransform(d));

        for (unsigned int c = s; c < s + 3; c++) {
          for (unsigned int vd = 0; vd < 3; vd++)
            this->operator()(c - s + k, vd + 3 * (d / m_DomainsPerShape)) = tmp(c - s, vd) * m_AttributeScales[num + c];
        }
        s += 3;
        k += 3;
      }
    }

    if (m_AttributesPerDomain[dom] > 0) {
      if (ps->GetDomainFlag(d)) {
        for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++) {
          for (unsigned int vd = 0; vd < 3; vd++)
            this->operator()(aa + k, vd + 3 * (d / m_DomainsPerShape)) = 0.0 * m_AttributeScales[num + aa + s];
        }
      } else {
        // TODO, figure out what is going on here
        point pt;
        pt.clear();
        pt[0] = posLocal[0];
        pt[1] = posLocal[1];
        pt[2] = posLocal[2];

        for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++) {
          point dc;
          dc.clear();
          const shapeworks::ParticleImplicitSurfaceDomain<float>* domain =
              static_cast<const shapeworks::ParticleImplicitSurfaceDomain<float>*>(ps->GetDomain(d));
          meshFIM* ptr = domain->GetMesh();
          dc = ptr->GetFeatureDerivative(pt, aa);
          for (int vd = 0; vd < 3; vd++)
            this->operator()(aa + k, vd + 3 * (d / m_DomainsPerShape)) = dc[vd] * m_AttributeScales[num + aa + s];
        }
      }
    }
  }

  virtual void DomainAddEventCallback(Object*, const EventObject& e) {
    const itk::ParticleDomainAddEvent& event = dynamic_cast<const itk::ParticleDomainAddEvent&>(e);
    unsigned int d = event.GetDomainIndex();

    if (d % m_DomainsPerShape == 0) this->ResizeMatrix(this->rows(), this->cols() + 3);  // 3 columns for every shape
  }

  virtual void PositionAddEventCallback(Object* o, const EventObject& e) {
    // update the size of matrix based on xyz, normals and number of attributes being used
    const itk::ParticlePositionAddEvent& event = dynamic_cast<const itk::ParticlePositionAddEvent&>(e);
    const itk::ParticleSystem* ps = dynamic_cast<const itk::ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    int numRows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++) {
      if (m_use_xyz[i]) numRows += 3 * ps->GetNumberOfParticles(i);
      if (m_use_normals[i]) numRows += 3 * ps->GetNumberOfParticles(i);
      numRows += m_AttributesPerDomain[i] * ps->GetNumberOfParticles(i);
    }

    if (numRows > this->rows()) this->ResizeMatrix(numRows, this->cols());

    this->SetValues(ps, idx, d);
  }

  virtual void PositionSetEventCallback(Object* o, const EventObject& e) {
    // update xyz, normals and number of attributes being used
    const itk::ParticlePositionSetEvent& event = dynamic_cast<const itk::ParticlePositionSetEvent&>(e);
    const itk::ParticleSystem* ps = dynamic_cast<const itk::ParticleSystem*>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();

    this->SetValues(ps, idx, d);
  }

  virtual void PositionRemoveEventCallback(Object*, const EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

 protected:
  ParticleGeneralShapeGradientMatrix() {
    m_DomainsPerShape = 1;

    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~ParticleGeneralShapeGradientMatrix() {}

  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os, indent); }

  int m_DomainsPerShape;

 private:
  ParticleGeneralShapeGradientMatrix(const Self&);  // purposely not implemented
  void operator=(const Self&);                      // purposely not implemented

  std::vector<bool> m_use_xyz;
  std::vector<bool> m_use_normals;
  std::vector<int> m_AttributesPerDomain;
  std::vector<double> m_AttributeScales;

};  // end class

}  // namespace itk
