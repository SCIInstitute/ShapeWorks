#include "ShapeGradientMatrix.h"

namespace shapeworks {

void ShapeGradientMatrix::SetValues(const ParticleSystemType* ps, int idx, int d) {
  const typename ParticleSystem::PointType posLocal = ps->GetPosition(idx, d);

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
  if (m_use_xyz[dom]) {
    k += idx * 3;
  }
  if (m_use_normals[dom]) {
    k += idx * 3;
  }
  k += idx * m_AttributesPerDomain[dom];

  int s = 0;
  if (m_use_xyz[dom]) {
    for (unsigned int i = 0; i < 3; i++) {
      for (unsigned int j = 0; j < 3; j++) {
        if (i == j) {
          this->operator()(i + k, j + 3 * (d / m_DomainsPerShape)) = 1.0 * m_AttributeScales[num + i];
        } else {
          this->operator()(i + k, j + 3 * (d / m_DomainsPerShape)) = 0.0;
        }
      }
    }
    k += 3;
    s += 3;
  }
  if (m_use_normals[dom]) {
    if (ps->GetDomainFlag(d)) {
      for (unsigned int c = s; c < s + 3; c++) {
        for (unsigned int vd = 0; vd < 3; vd++) {
          this->operator()(c - s + k, vd + 3 * (d / m_DomainsPerShape)) = 0.0 * m_AttributeScales[num + c];
        }
      }
      s += 3;
      k += 3;
    } else {
      vnl_vector_fixed<float, DIMENSION> gradient = ps->GetDomain(d)->SampleGradientAtPoint(posLocal, idx);
      vnl_vector_fixed<float, DIMENSION> normal = gradient.normalize();
      float grad_mag = gradient.magnitude();  // TODO This is always 1.0. Fix when correcting image gradient of
                                              // normals
      shapeworks::ParticleDomain::GradNType grad_n = ps->GetDomain(d)->SampleGradNAtPoint(posLocal, idx);

      shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat1;
      mat1.set_identity();
      vnl_matrix<float> nrml(3, 1);
      nrml.fill(0.0);
      nrml(0, 0) = normal[0];
      nrml(1, 0) = normal[1];
      nrml(2, 0) = normal[2];
      shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat2 = nrml * nrml.transpose();

      for (unsigned int x1 = 0; x1 < 3; x1++) {
        for (unsigned int x2 = 0; x2 < 3; x2++) {
          mat1(x1, x2) -= mat2(x1, x2);
          grad_n(x1, x2) /= grad_mag;
        }
      }

      // mat3 = H/|grad_f| * (I - n*n');
      shapeworks::ParticleImageDomainWithGradN<float>::VnlMatrixType mat3 = grad_n * mat1;
      ParticleSystem::VnlMatrixType tmp;
      tmp.set_size(3, 3);
      tmp.fill(0.0);
      for (unsigned int c = 0; c < 3; c++) {
        for (unsigned vd = 0; vd < 3; vd++) {
          tmp(c, vd) = mat3(c, vd);
        }
      }

      tmp = ps->TransformNormalDerivative(tmp, ps->GetTransform(d) * ps->GetPrefixTransform(d));

      for (unsigned int c = s; c < s + 3; c++) {
        for (unsigned int vd = 0; vd < 3; vd++) {
          this->operator()(c - s + k, vd + 3 * (d / m_DomainsPerShape)) = tmp(c - s, vd) * m_AttributeScales[num + c];
        }
      }
      s += 3;
      k += 3;
    }
  }

  if (m_AttributesPerDomain[dom] > 0) {
    if (ps->GetDomainFlag(d)) {
      for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++) {
        for (unsigned int vd = 0; vd < 3; vd++) {
          this->operator()(aa + k, vd + 3 * (d / m_DomainsPerShape)) = 0.0 * m_AttributeScales[num + aa + s];
        }
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
        for (int vd = 0; vd < 3; vd++) {
          this->operator()(aa + k, vd + 3 * (d / m_DomainsPerShape)) = dc[vd] * m_AttributeScales[num + aa + s];
        }
      }
    }
  }
}

}  // namespace shapeworks