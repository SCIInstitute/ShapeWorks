
#include "SamplingFunction.h"

#include "Libs/Common/Logging.h"
#include "Libs/Optimize/Domain/DomainType.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

//---------------------------------------------------------------------------
double SamplingFunction::EstimateSigma(unsigned int idx, unsigned int dom, const shapeworks::ParticleDomain* domain,
                                       const PointType& pos, double initial_sigma, double precision, int& err,
                                       double& avgKappa) const {
  avgKappa = 0.0;
  const double min_sigma = 1.0e-4;
  const double epsilon = 1.0e-5;

  const double M = static_cast<double>(VDimension);
  const double MM = M * M * 2.0 + M;

  double error = 1.0e6;
  double sigma, prev_sigma;
  sigma = initial_sigma;

  while (error > precision) {
    double A = 0.0;
    double B = 0.0;
    double C = 0.0;
    double sigma2 = sigma * sigma;
    double sigma22 = sigma2 * 2.0;

    for (unsigned int i = 0; i < m_CurrentNeighborhood.size(); i++) {
      if (m_CurrentNeighborhood[i].weight < epsilon) continue;

      double kappa = 1.0;
      avgKappa += kappa;

      double sqrdistance = m_CurrentNeighborhood[i].distance * m_CurrentNeighborhood[i].distance;

      double alpha = exp(-sqrdistance / sigma22) * m_CurrentNeighborhood[i].weight;
      A += alpha;
      B += sqrdistance * alpha;
      C += sqrdistance * sqrdistance * alpha;
    }  // end for i

    // Alan : Even though the actual average kappa is always 1.0, this avgKappa can vary
    // depending on the number of times the while loop is executed.  This seems to be a bug, but
    // I am leaving it for now as it changes the results if I fix it.
    avgKappa /= static_cast<double>(m_CurrentNeighborhood.size());

    prev_sigma = sigma;

    if (A < epsilon) {
      err = 1;
      avgKappa = 1.0;
      return sigma;
    };  // results are not meaningful

    sigma -= (A * (B - A * sigma2 * M)) / ((-MM * A * A * sigma) - 3.0 * A * B * (1.0 / (sigma + epsilon)) -
                                           (A * C + B * B) * (1.0 / (sigma2 * sigma + epsilon)) + epsilon);

    error = 1.0 - fabs((sigma / prev_sigma));

    // Constrain sigma.
    if (sigma < min_sigma) {
      sigma = min_sigma;
      error = precision;  // we are done if sigma has vanished
    } else {
      if (sigma < 0.0) {
        sigma = min_sigma;
      }
    }
  }  // end while (error > precision)

  err = 0;
  return sigma;
}

//---------------------------------------------------------------------------
VectorFunction::Pointer SamplingFunction::Clone() {
  SamplingFunction::Pointer copy = SamplingFunction::New();
  copy->SetParticleSystem(this->GetParticleSystem());
  copy->m_Counter = this->m_Counter;
  copy->m_avgKappa = this->m_avgKappa;
  copy->m_IsSharedBoundaryEnabled = this->m_IsSharedBoundaryEnabled;
  copy->m_SharedBoundaryWeight = this->m_SharedBoundaryWeight;
  copy->m_CurrentSigma = this->m_CurrentSigma;
  copy->m_CurrentNeighborhood = this->m_CurrentNeighborhood;

  copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
  copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
  copy->m_FlatCutoff = this->m_FlatCutoff;
  copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

  copy->m_SpatialSigmaCache = this->m_SpatialSigmaCache;

  copy->m_DomainNumber = this->m_DomainNumber;
  copy->m_ParticleSystem = this->m_ParticleSystem;

  return (VectorFunction::Pointer)copy;
}

//---------------------------------------------------------------------------
void SamplingFunction::BeforeEvaluate(unsigned int idx, unsigned int d, const ParticleSystem* system) {
  m_MaxMoveFactor = 0.1;

  // Compute the neighborhood size and the optimal sigma.
  const double epsilon = 1.0e-6;

  if (system->GetDomain(d)->IsDomainFixed()) {
    return;
  }

  // Get the position for which we are computing the gradient.
  PointType pos = system->GetPosition(idx, d);

  // Retrieve the previous optimal sigma value for this point.  If the value is
  // tiny (i.e. initialized) then use a fraction of the maximum allowed
  // neighborhood radius.
  m_CurrentSigma = this->GetSpatialSigmaCache()->operator[](d)->operator[](idx);

  if (m_CurrentSigma < epsilon) {
    m_CurrentSigma = this->GetMinimumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
  }

  // Determine the extent of the neighborhood that will be used in the Parzen
  // windowing estimation.  The neighborhood extent is based on the optimal
  // sigma calculation and limited to a user supplied maximum radius (probably
  // the size of the domain).
  double neighborhood_radius = m_CurrentSigma * 1.3 * this->GetNeighborhoodToSigmaRatio();

  if (neighborhood_radius > this->GetMaximumNeighborhoodRadius()) {
    neighborhood_radius = this->GetMaximumNeighborhoodRadius();
  }

  // Get the neighborhood surrounding the point "pos".
  UpdateNeighborhood(pos, idx, d, neighborhood_radius, system);

  // Estimate the best sigma for Parzen windowing.  In some cases, such as when
  // the neighborhood does not include enough points, the value will be bogus.
  // In these cases, an error != 0 is returned, and we try the estimation again
  // with an increased neighborhood radius.
  int err;
  m_CurrentSigma = EstimateSigma(idx, d, system->GetDomain(d), pos, m_CurrentSigma, epsilon, err, m_avgKappa);

  while (err != 0) {
    neighborhood_radius *= 2.0;

    // Constrain the neighborhood size.  If we have reached a maximum
    // possible neighborhood size, we'll just go with that.
    if (neighborhood_radius > this->GetMaximumNeighborhoodRadius()) {
      m_CurrentSigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
      neighborhood_radius = this->GetMaximumNeighborhoodRadius();
      break;
    } else {
      m_CurrentSigma = neighborhood_radius / this->GetNeighborhoodToSigmaRatio();
    }

    UpdateNeighborhood(pos, idx, d, neighborhood_radius, system);

    m_CurrentSigma = EstimateSigma(idx, d, system->GetDomain(d), pos, m_CurrentSigma, epsilon, err, m_avgKappa);
  }  // done while err

  // Constrain sigma to a maximum reasonable size based on the user-supplied
  // limit to neighborhood size.
  if (m_CurrentSigma > this->GetMaximumNeighborhoodRadius()) {
    m_CurrentSigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
    neighborhood_radius = this->GetMaximumNeighborhoodRadius();
    UpdateNeighborhood(pos, idx, d, neighborhood_radius, system);
  }

  // Make sure sigma doesn't change too quickly!
  m_CurrentSigma = (this->GetSpatialSigmaCache()->operator[](d)->operator[](idx) + m_CurrentSigma) / 2.0;

  // We are done with the sigma estimation step.  Cache the sigma value for
  // next time.
  this->GetSpatialSigmaCache()->operator[](d)->operator[](idx) = m_CurrentSigma;
}

//---------------------------------------------------------------------------
SamplingFunction::VectorType SamplingFunction::Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system,
                                                        double& maxmove, double& energy) const {
  const double epsilon = 1.0e-6;

  // Get the position for which we are computing the gradient.
  PointType pos = system->GetPosition(idx, d);

  // Compute the gradients
  double sigma2inv = 1.0 / (2.0 * m_CurrentSigma * m_CurrentSigma + epsilon);

  VectorType gradE;

  for (unsigned int n = 0; n < VDimension; n++) {
    gradE[n] = 0.0;
  }

  double A = 0.0;

  for (unsigned int i = 0; i < m_CurrentNeighborhood.size(); i++) {
    VectorType r;

    // Use the domain distance metric only if the two domains are the same
    // See https://github.com/SCIInstitute/ShapeWorks/issues/1215
    if (m_CurrentNeighborhood[i].dom == d) {
      system->GetDomain(d)->Distance(pos, idx, m_CurrentNeighborhood[i].pi_pair.Point,
                                     m_CurrentNeighborhood[i].pi_pair.Index, &r);
    } else {
      for (unsigned int n = 0; n < VDimension; n++) {
        // Note that the Neighborhood object has already filtered the
        // neighborhood for points whose normals differ by > 90 degrees.
        r[n] = (pos[n] - m_CurrentNeighborhood[i].pi_pair.Point[n]);
      }
    }

    double q = exp(-dot_product(r, r) * sigma2inv);
    A += q;

    for (unsigned int n = 0; n < VDimension; n++) {
      gradE[n] += m_CurrentNeighborhood[i].weight * r[n] * q;
    }
  }

  double p = 0.0;
  if (A > epsilon) {
    p = -1.0 / (A * m_CurrentSigma * m_CurrentSigma);
  }

  for (unsigned int n = 0; n < VDimension; n++) {
    gradE[n] *= p;
  }

  maxmove = (m_CurrentSigma / m_avgKappa) * m_MaxMoveFactor;

  // Contour domain cannot recover from swapped particles. This works around that by constraining moves to be no more
  // than 0.5 times the distance to closest neighbor.
  if (system->GetDomain(d)->GetDomainType() == shapeworks::DomainType::Contour && m_CurrentNeighborhood.size() > 0) {
    auto min_it = std::min_element(
        m_CurrentNeighborhood.begin(), m_CurrentNeighborhood.end(),
        [](const CrossDomainNeighborhood& n1, const CrossDomainNeighborhood& n2) { return n1.distance < n2.distance; });
    maxmove = std::min(maxmove, 0.5 * min_it->distance);
  }

  energy = (A * sigma2inv) / m_avgKappa;

  gradE = gradE / m_avgKappa;
  return gradE;
}

//---------------------------------------------------------------------------
void SamplingFunction::UpdateNeighborhood(const SamplingFunction::PointType& pos, int idx, int d, double radius,
                                          const ParticleSystem* system) {
  const auto domains_per_shape = system->GetDomainsPerShape();
  const auto domain_base = d / domains_per_shape;

  m_CurrentNeighborhood.clear();
  for (int offset = 0; offset < domains_per_shape; offset++) {
    const auto domain_t = domain_base * domains_per_shape + offset;
    auto neighborhood = system->GetNeighborhood(domain_t);
    using ImageType = itk::Image<float, Dimension>;

    if (!m_IsSharedBoundaryEnabled && domain_t != d) {
      continue;
    }
    const bool this_is_contour = system->GetDomain(d)->GetDomainType() == shapeworks::DomainType::Contour;
    if (this_is_contour && domain_t != d) {
      continue;
    }
    const bool other_is_contour = system->GetDomain(domain_t)->GetDomainType() == shapeworks::DomainType::Contour;
    if (!other_is_contour && domain_t != d) {
      continue;
    }

    // Sampling term is only computed if:
    // * Both domains are the same
    // * This is not a contour, but the other domain is a contour

    std::vector<double> weights;
    std::vector<double> distances;
    std::vector<ParticlePointIndexPair> res;
    if (domain_t == d) {
      // same domain
      res = neighborhood->find_neighborhood_points(pos, idx, weights, distances, radius);
    } else {
      // cross domain

      bool weighting_state = neighborhood->is_weighting_enabled();
      // Disable cosine-falloff weighting for cross-domain sampling term. Contours don't have normals.
      neighborhood->set_weighting_enabled(false);
      neighborhood->set_force_euclidean(true);

      res = neighborhood->find_neighborhood_points(pos, -1, weights, distances, radius);

      neighborhood->set_force_euclidean(false);
      neighborhood->set_weighting_enabled(weighting_state);
    }

    assert(weights.size() == distances.size() && res.size() == weights.size());

    // todo should avoid this copy. requires changing way too many APIs
    for (int i = 0; i < res.size(); i++) {
      const double weight = domain_t == d ? weights[i] : m_SharedBoundaryWeight;
      m_CurrentNeighborhood.emplace_back(res[i], weight, distances[i], domain_t);
    }
  }
}

}  // namespace shapeworks
