#pragma once

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "ContourDomain.h"
#include "DomainType.h"

namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
double
ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>
::EstimateSigma(unsigned int idx,
                unsigned int dom,
                const shapeworks::ParticleDomain *domain,
                const PointType &pos,
                double initial_sigma,
                double precision,
                int &err,
                double &avgKappa) const
{
  //  avgKappa = this->ComputeKappa(m_MeanCurvatureCache->operator[](this->GetDomainNumber())->operator[](idx), dom);
  avgKappa = 0.0;
  const double min_sigma = 1.0e-4;
  const double epsilon = 1.0e-5;
  
  const double M = static_cast<double>(VDimension);
  const double MM = M * M * 2.0 + M;
  
  double error = 1.0e6;
  double sigma, prev_sigma;
  sigma = initial_sigma;
  
  while (error > precision)
    {
    double A = 0.0;
    double B = 0.0;
    double C = 0.0;
    double sigma2 = sigma * sigma;
    double sigma22 = sigma2 * 2.0;
    
    double mymc = m_MeanCurvatureCache->operator[](this->GetDomainNumber())->operator[](idx);

    for (unsigned int i = 0; i < m_CurrentNeighborhood.size(); i++)
      {
      if (m_CurrentNeighborhood[i].weight < epsilon) continue;

      double mc = m_MeanCurvatureCache->operator[](this->GetDomainNumber())->operator[](
              m_CurrentNeighborhood[i].pi_pair.Index);
      double Dij = (mymc + mc) * 0.5;
      double kappa = this->ComputeKappa(Dij, dom);

      avgKappa += kappa;
      
      double sqrdistance = m_CurrentNeighborhood[i].distance * m_CurrentNeighborhood[i].distance;
      sqrdistance = sqrdistance * kappa * kappa;

      double alpha = exp(-sqrdistance / sigma22) * m_CurrentNeighborhood[i].weight;
      A += alpha;
      B += sqrdistance * alpha;
      C += sqrdistance * sqrdistance * alpha;
      } // end for i

    avgKappa /= static_cast<double>(m_CurrentNeighborhood.size());

    prev_sigma = sigma;

    if (A < epsilon)
      {
      err = 1;
      avgKappa = 1.0;
      return sigma;
      }; // results are not meaningful
    
    // // First order convergence update.
    // sigma = sqrt(( 1.0 / DIM ) * ( B / A));
    
    // old math
    //  sigma -= (sigma2 * VDimension * A * A - A  * B) / (((2.0 * sigma * VDimension) * A * A -
    //                                          (1.0/(sigma2*sigma))*(A*C-B*B)) + epsilon);
    
    // New math -- results are not obviously different? 
    sigma -= (A * (B - A * sigma2 * M)) /
      ( (-MM * A *A * sigma) - 3.0 * A * B * (1.0 / (sigma + epsilon))
        - (A*C + B*B) * (1.0 / (sigma2 * sigma + epsilon)) + epsilon);
    
    error = 1.0 - fabs((sigma/prev_sigma));
    
    // Constrain sigma.
    if (sigma < min_sigma)
      {
      sigma = min_sigma;
      error = precision; // we are done if sigma has vanished
      }
    else
      {
      if (sigma < 0.0) sigma = min_sigma;
      }
    
    } // end while (error > precision)
  
  err = 0;
  return sigma;
}

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>
::BeforeEvaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system)
{
    m_MaxMoveFactor = 0.1;

  // Compute the neighborhood size and the optimal sigma.
  const double epsilon = 1.0e-6;
  
  if (system->GetDomain(d)->IsDomainFixed()) {
    return;
  }

  // Get the position for which we are computing the gradient.
  PointType pos = system->GetPosition(idx, d);
  
  // Retrieve the previous optimal sigma value for this point.  If the value is
  // tiny (i.e. unitialized) then use a fraction of the maximum allowed
  // neighborhood radius.
  m_CurrentSigma = this->GetSpatialSigmaCache()->operator[](d)->operator[](idx);
  double myKappa = this->ComputeKappa(m_MeanCurvatureCache->operator[](this->GetDomainNumber())->operator[](idx), d);

   // TEST DISTANCE TO PLANE IDEA
  //  myKappa *=  (fabs(pos[0]) * 1.0);
    // END TEST
  
  if (m_CurrentSigma < epsilon)
    {
    m_CurrentSigma = this->GetMinimumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
    }

  // Determine the extent of the neighborhood that will be used in the Parzen
  // windowing estimation.  The neighborhood extent is based on the optimal
  // sigma calculation and limited to a user supplied maximum radius (probably
  // the size of the domain).
  double neighborhood_radius = (m_CurrentSigma / myKappa) * 1.3 * this->GetNeighborhoodToSigmaRatio();

  if (neighborhood_radius > this->GetMaximumNeighborhoodRadius())
    {
    neighborhood_radius = this->GetMaximumNeighborhoodRadius();
    }
  
  
  // Get the neighborhood surrounding the point "pos".
  UpdateNeighborhood(pos, idx, d, neighborhood_radius, system);

  // Compute the weights based on angle between the neighbors and the center.
   //    this->ComputeAngularWeights(pos,m_CurrentNeighborhood,domain, m_CurrentWeights);
  
  // Estimate the best sigma for Parzen windowing.  In some cases, such as when
  // the neighborhood does not include enough points, the value will be bogus.
  // In these cases, an error != 0 is returned, and we try the estimation again
  // with an increased neighborhood radius.
  int err;
  m_CurrentSigma = EstimateSigma(idx, d,system->GetDomain(d), pos,
                                  m_CurrentSigma, epsilon, err, m_avgKappa);

  while (err != 0)
    {
    neighborhood_radius *= 2.0;

    // Constrain the neighborhood size.  If we have reached a maximum
    // possible neighborhood size, we'll just go with that.
    if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius())
      {
      m_CurrentSigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
      neighborhood_radius =  this->GetMaximumNeighborhoodRadius();
      break;
      }
    else
      {
      m_CurrentSigma = neighborhood_radius / this->GetNeighborhoodToSigmaRatio();
      }

    UpdateNeighborhood(pos, idx, d, neighborhood_radius, system);

    m_CurrentSigma = EstimateSigma(idx, d, system->GetDomain(d), pos,
                                   m_CurrentSigma, epsilon, err, m_avgKappa);
    } // done while err

  // Constrain sigma to a maximum reasonable size based on the user-supplied
  // limit to neighborhood size.
  if (m_CurrentSigma > this->GetMaximumNeighborhoodRadius())
    {
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


template <class TGradientNumericType, unsigned int VDimension>
typename ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>::VectorType
ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{
  const double epsilon = 1.0e-6;

   // Get the position for which we are computing the gradient.
  PointType pos = system->GetPosition(idx, d);

  // Compute the gradients
  double sigma2inv = 1.0 / (2.0 * m_CurrentSigma * m_CurrentSigma + epsilon);

  VectorType gradE;

  for (unsigned int n = 0; n < VDimension; n++) {
    gradE[n] = 0.0;
  }

  double mymc = m_MeanCurvatureCache->operator[](d)->operator[](idx);
  double A = 0.0;

  for (unsigned int i = 0; i < m_CurrentNeighborhood.size(); i++) {
    double mc = m_MeanCurvatureCache->operator[](d)->operator[](m_CurrentNeighborhood[i].pi_pair.Index);
    double Dij = (mymc + mc) * 0.5; // average my curvature with my neighbors
    double kappa = this->ComputeKappa(Dij, d);

    VectorType r;

    // Use the domain distance metric only if the two domains are the same
    // See https://github.com/SCIInstitute/ShapeWorks/issues/1215
    if (m_CurrentNeighborhood[i].dom == d) {
      system->GetDomain(d)->Distance(
              pos, idx,
              m_CurrentNeighborhood[i].pi_pair.Point, m_CurrentNeighborhood[i].pi_pair.Index,
              &r
      );
    } else {
      for (unsigned int n = 0; n < VDimension; n++) {
        // Note that the Neighborhood object has already filtered the
        // neighborhood for points whose normals differ by > 90 degrees.
        r[n] = (pos[n] - m_CurrentNeighborhood[i].pi_pair.Point[n]) * kappa;
      }
    }
    r *= kappa;

    double q = kappa * exp(-dot_product(r, r) * sigma2inv);
    A += q;

    for (unsigned int n = 0; n < VDimension; n++) {
      gradE[n] += m_CurrentNeighborhood[i].weight * r[n] * q;
    }
  }

  double p = 0.0;
  if (A > epsilon) {
    p = -1.0 / (A * m_CurrentSigma * m_CurrentSigma);
  }
  
  for (unsigned int n = 0; n <VDimension; n++)
    {    gradE[n] *= p;    }

  maxmove = (m_CurrentSigma / m_avgKappa) * m_MaxMoveFactor;

  // Contour domain cannot recover from swapped particles. This works around that by constraining moves to be no more
  // than 0.5 times the distance to closest neighbor.
  if (system->GetDomain(d)->GetDomainType() == shapeworks::DomainType::Contour && m_CurrentNeighborhood.size() > 0) {
    auto min_it = std::min_element(m_CurrentNeighborhood.begin(), m_CurrentNeighborhood.end(),
                                   [](const CrossDomainNeighborhood &n1, const CrossDomainNeighborhood &n2) {
                                     return n1.distance < n2.distance;
                                   });
    maxmove = std::min(maxmove, 0.5 * min_it->distance);
  }

  energy = (A * sigma2inv ) / m_avgKappa;

  gradE = gradE / m_avgKappa;
  return gradE;
}

}// end namespace


