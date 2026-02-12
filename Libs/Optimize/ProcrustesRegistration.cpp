#include "ProcrustesRegistration.h"

#include <Logging.h>

#include "Procrustes3D.h"

namespace shapeworks {

//---------------------------------------------------------------------------
Procrustes3D::ShapeType ProcrustesRegistration::ExtractShape(int domain_index, int num_points) {
  Procrustes3D::ShapeType shape;
  Procrustes3D::PointType point;
  for (int j = 0; j < num_points; j++) {
    point(0) = m_ParticleSystem->GetPrefixTransformedPosition(j, domain_index)[0];
    point(1) = m_ParticleSystem->GetPrefixTransformedPosition(j, domain_index)[1];
    point(2) = m_ParticleSystem->GetPrefixTransformedPosition(j, domain_index)[2];
    shape.push_back(point);
  }
  return shape;
}

//---------------------------------------------------------------------------
void ProcrustesRegistration::RunGPARegistration(int domainStart, int numShapes, int numPoints) {
  Procrustes3D::ShapeListType shapelist;
  for (int k = domainStart; k < domainStart + numShapes * m_DomainsPerShape; k += m_DomainsPerShape) {
    shapelist.push_back(ExtractShape(k, numPoints));
  }

  Procrustes3D::SimilarityTransformListType transforms;
  Procrustes3D procrustes(m_Scaling, m_RotationTranslation);
  procrustes.AlignShapes(transforms, shapelist);

  Procrustes3D::TransformMatrixListType matrices;
  procrustes.ConstructTransformMatrices(transforms, matrices);

  int k = domainStart;
  for (int i = 0; i < numShapes; i++, k += m_DomainsPerShape) {
    m_ParticleSystem->SetTransform(k, matrices[i]);
  }
}

//---------------------------------------------------------------------------
void ProcrustesRegistration::RunFixedDomainRegistration(int domainStart, int numShapes, int numPoints,
                                                        const std::vector<bool>& is_fixed) {
  const int domainType = domainStart;

  // Ensure cache vector is sized for all domain types
  if (static_cast<int>(m_FixedDomainCaches.size()) <= domainType) {
    m_FixedDomainCaches.resize(domainType + 1);
  }

  auto& cache = m_FixedDomainCaches[domainType];

  // Build/rebuild cache if needed (first call or particle count changed after split)
  if (!cache.valid || cache.num_points != numPoints) {
    Procrustes3D::ShapeListType fixed_shapelist;
    std::vector<int> fixed_domain_indices;

    for (int i = 0, k = domainStart; i < numShapes; i++, k += m_DomainsPerShape) {
      if (!is_fixed[i]) continue;
      fixed_shapelist.push_back(ExtractShape(k, numPoints));
      fixed_domain_indices.push_back(k);
    }

    // Run GPA on fixed shapes only
    Procrustes3D::SimilarityTransformListType fixed_transforms;
    Procrustes3D procrustes(m_Scaling, m_RotationTranslation);
    procrustes.AlignShapes(fixed_transforms, fixed_shapelist);

    // Set transforms for fixed shapes
    Procrustes3D::TransformMatrixListType fixed_matrices;
    procrustes.ConstructTransformMatrices(fixed_transforms, fixed_matrices);

    for (size_t i = 0; i < fixed_domain_indices.size(); i++) {
      m_ParticleSystem->SetTransform(fixed_domain_indices[i], fixed_matrices[i]);
    }

    // Compute and cache the mean of the aligned fixed shapes
    // (fixed_shapelist has been modified in-place by AlignShapes to be in Procrustes space)
    procrustes.ComputeMeanShape(cache.mean, fixed_shapelist);
    cache.num_points = numPoints;
    cache.valid = true;

    SW_LOG("Procrustes: cached fixed domain mean for domain type {} ({} fixed shapes, {} points)", domainType,
           fixed_domain_indices.size(), numPoints);
  }

  // Align each non-fixed shape to the cached fixed mean using OPA
  Procrustes3D procrustes(m_Scaling, m_RotationTranslation);

  for (int i = 0, k = domainStart; i < numShapes; i++, k += m_DomainsPerShape) {
    if (is_fixed[i]) {
      continue;
    }

    Procrustes3D::ShapeType source_shape = ExtractShape(k, numPoints);

    // Align source to the cached fixed mean (defensive copy of mean)
    SimilarityTransform3D transform;
    Procrustes3D::ShapeType target_mean = cache.mean;
    procrustes.AlignSourceToTarget(transform, target_mean, source_shape);

    // Convert to 4x4 matrix and set transform
    Procrustes3D::TransformMatrixType matrix;
    procrustes.ConstructTransformMatrix(transform, matrix);
    m_ParticleSystem->SetTransform(k, matrix);
  }
}

//---------------------------------------------------------------------------
void ProcrustesRegistration::RunRegistration(int d) {
  // DOES NOT Assume all domains have the same number of particles.
  const int totalDomains = m_ParticleSystem->GetNumberOfDomains();
  const int numPoints = m_ParticleSystem->GetNumberOfParticles(d);
  const int numShapes = totalDomains / m_DomainsPerShape;

  // Do not run procrustes for this domain if number of points less than 10
  if (numPoints < 10) {
    return;
  }

  const int domainStart = d % m_DomainsPerShape;
  bool has_fixed_domains = false;
  std::vector<bool> is_fixed = m_ParticleSystem->GetFixedShapes(has_fixed_domains);

  if (has_fixed_domains) {
    RunFixedDomainRegistration(domainStart, numShapes, numPoints, is_fixed);
  } else {
    RunGPARegistration(domainStart, numShapes, numPoints);
  }
}

//---------------------------------------------------------------------------
void ProcrustesRegistration::RunRegistration() {
  for (int i = 0; i < m_DomainsPerShape; i++) {
    RunRegistration(i);
  }
}
}  // namespace shapeworks
