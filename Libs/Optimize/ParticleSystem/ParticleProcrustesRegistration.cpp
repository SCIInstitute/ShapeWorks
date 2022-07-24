#include "ParticleProcrustesRegistration.h"

#include "Procrustes3D.h"

namespace shapeworks {

  //---------------------------------------------------------------------------
void ParticleProcrustesRegistration::RunRegistration(int d) {
  // DOES NOT Assume all domains have the same number of particles.
  const int totalDomains = m_ParticleSystem->GetNumberOfDomains();
  const int numPoints = m_ParticleSystem->GetNumberOfParticles(d);
  const int numShapes = totalDomains / m_DomainsPerShape;

  // Do not run procrustesfor this domain if number of points less than 10
  if (numPoints < 10) {
    return;
  }

  Procrustes3D::ShapeListType shapelist;
  Procrustes3D::ShapeType shapevector;
  Procrustes3D::PointType point;

  // Read input shapes from file list

  for (int i = d % m_DomainsPerShape; i < totalDomains; i += m_DomainsPerShape) {
    shapevector.clear();
    for (int j = 0; j < numPoints; j++) {
      point(0) = m_ParticleSystem->GetPrefixTransformedPosition(j, i)[0];
      point(1) = m_ParticleSystem->GetPrefixTransformedPosition(j, i)[1];
      point(2) = m_ParticleSystem->GetPrefixTransformedPosition(j, i)[2];

      shapevector.push_back(point);
    }
    shapelist.push_back(shapevector);
  }

  // Run alignment
  Procrustes3D::SimilarityTransformListType transforms;
  Procrustes3D procrustes(m_Scaling, m_RotationTranslation, m_TranslationOnly);
  procrustes.AlignShapes(transforms, shapelist);

  // Construct transform matrices for each particle system.

  int k = d % m_DomainsPerShape;

  Procrustes3D::TransformMatrixListType matrices;
  procrustes.ConstructTransformMatrices(transforms, matrices);

  for (int i = 0; i < numShapes; i++, k += m_DomainsPerShape) {
    m_ParticleSystem->SetTransform(k, matrices[i]);
  }
}

//---------------------------------------------------------------------------
void ParticleProcrustesRegistration::RunRegistration() {
  for (int i = 0; i < m_DomainsPerShape; i++) {
    this->RunRegistration(i);
  }
}
}  // namespace shapeworks
