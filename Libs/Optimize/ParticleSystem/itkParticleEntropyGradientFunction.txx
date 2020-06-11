/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEntropyGradientFunction.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEntropyGradientFunction_txx
#define __itkParticleEntropyGradientFunction_txx

#include "itkParticleImageDomainWithGradients.h"
namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
TGradientNumericType
ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
::AngleCoefficient( const GradientVectorType& p_i_normal,  const GradientVectorType& p_j_normal) const
{
  // define the float region cut-off in terms of the cosine
  //   const TGradientNumericType flat_cutoff = 0.156; // approx 90% of the region
  //float flat_cutoff = 0.11; // approx 90% of the region
  //  const  double flat_cutoff = 0.05;
  
  // get the cosine of the angle between the two particles' normals
  TGradientNumericType cosine = dot_product(p_i_normal,p_j_normal) /
    (p_i_normal.magnitude()*p_j_normal.magnitude() + 1.0e-6);
  
  // the flat region
  if ( cosine >= m_FlatCutoff ) return 1.0;
  
  // the feathered region
  return ( cos( (m_FlatCutoff - cosine) / (1.0+m_FlatCutoff) * (3.14159265358979/2.0) )) ; 
} 

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
::ComputeAngularWeights(const PointType &pos,
                        const typename ParticleSystemType::PointVectorType &neighborhood,
                        const ParticleImageDomainWithGradients<TGradientNumericType, VDimension> *domain,
                        std::vector<double> &weights) const
{
  GradientVectorType posnormal = domain->SampleNormalAtPoint(pos);
  weights.resize(neighborhood.size());
  
  for (unsigned int i = 0; i < neighborhood.size(); i++)
    {
    weights[i] = this->AngleCoefficient(posnormal,
                                        domain->SampleNormalAtPoint(neighborhood[i].Point));
    if (weights[i] < 1.0e-5) weights[i] = 0.0;
    }
}

template <class TGradientNumericType, unsigned int VDimension>
double
ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
::EstimateSigma(unsigned int idx, const typename ParticleSystemType::PointVectorType &neighborhood,
                const std::vector<double> &weights,
                const PointType &pos,  double initial_sigma,  double precision,
                int &err) const
{
  const double epsilon = 1.0e-5;
  const double min_sigma = 1.0e-4;
  
  const double M = static_cast<double>(VDimension);
  const double MM = M * M * 2.0 + M;
  
  double error = 1.0e6;
  double sigma, prev_sigma;
  sigma = initial_sigma;
  
  while (error > precision)
    {
    VectorType r_vec;
    double A = 0.0;
    double B = 0.0;
    double C = 0.0;
    double sigma2 = sigma * sigma;
    double sigma22 = sigma2 * 2.0;
    
    for (unsigned int i = 0; i < neighborhood.size(); i++)
      {
      if (weights[i] < epsilon) continue;
      
      //    if ( neighborhood[i].Index == idx) continue;
      for (unsigned int n = 0; n < VDimension; n++)
        {
        r_vec[n] = pos[n] - neighborhood[i].Point[n];
        }
      
      double r = r_vec.magnitude();
      double r2 = r*r;
      double alpha = exp(-r2 / sigma22) * weights[i];
      A += alpha;
      B += r2 * alpha;
      C += r2 * r2 * alpha;
      } // end for i

    prev_sigma = sigma;
    
    if (A < epsilon)
      {
      err = 1;
      return sigma;
      }; // results are not meaningful
    
    // First order convergence update.  This is a fixed point iteration.
    //sigma = sqrt(( 1.0 / DIM ) * ( B / A));
    
    // Second order convergence update (Newton-Raphson).  This is the first
    // derivative of the negative of the probability density estimation function squared over the
    // second derivative.
    
    // old math
    //    sigma -= (sigma2 * VDimension * A * A - A  * B) / (((2.0 * sigma * VDimension) * A * A -
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
  
} // end estimate sigma


template <class TGradientNumericType, unsigned int VDimension>
typename ParticleEntropyGradientFunction<TGradientNumericType, VDimension>::VectorType
ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
::Evaluate(unsigned int idx,unsigned int d, const ParticleSystemType * system,
           double &maxdt) const
{
  // Grab a pointer to the domain.  We need a Domain that has surface normal information.
  const ParticleImageDomainWithGradients<TGradientNumericType, VDimension> *
    domain = static_cast<const ParticleImageDomainWithGradients<
  TGradientNumericType, VDimension> *>(system->GetDomain(d));
  const double epsilon = 1.0e-6;
  
  // Retrieve the previous optimal sigma value for this point.  If the value is
  // tiny (i.e. unitialized) then use a fraction of the maximum allowed
  // neighborhood radius.
  double sigma = m_SpatialSigmaCache->operator[](d)->operator[](idx);
  if (sigma < epsilon)
    { sigma = m_MinimumNeighborhoodRadius / m_NeighborhoodToSigmaRatio;}
  
  // Determine the extent of the neighborhood that will be used in the Parzen
  // windowing estimation.  The neighborhood extent is based on the optimal
  // sigma calculation and limited to a user supplied maximum radius (probably
  // the size of the domain).
  double neighborhood_radius = sigma * m_NeighborhoodToSigmaRatio;
  if (neighborhood_radius > m_MaximumNeighborhoodRadius)
    { neighborhood_radius = m_MaximumNeighborhoodRadius; }
  
  // Get the position for which we are computing the gradient.
  PointType pos = system->GetPosition(idx, d);
  
  // Get the neighborhood surrounding the point "pos".
  typename ParticleSystemType::PointVectorType neighborhood
    = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
  
  // Compute the weights based on angle between the neighbors and the center.
  std::vector<double> weights;
  this->ComputeAngularWeights(pos,neighborhood,domain,weights);
  
  // Estimate the best sigma for Parzen windowing.  In some cases, such as when
  // the neighborhood does not include enough points, the value will be bogus.
  // In these cases, an error != 0 is returned, and we try the estimation again
  // with an increased neighborhood radius.
  int err;
  sigma =  this->EstimateSigma(idx, neighborhood,weights,pos, sigma, epsilon, err);
  
  while (err != 0)
    {
    neighborhood_radius *= 2.0;
    // Constrain the neighborhood size.  If we have reached a maximum
    // possible neighborhood size, we'll just go with that.
    if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius())
      {
      sigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
      neighborhood_radius =  this->GetMaximumNeighborhoodRadius();
      break;
      }
    else
      {
      sigma = neighborhood_radius / this->GetNeighborhoodToSigmaRatio();
      }
    
    neighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
    this->ComputeAngularWeights(pos,neighborhood,domain,weights);
    sigma = this->EstimateSigma(idx, neighborhood, weights, pos, sigma, epsilon, err);
    } // done while err
  
  // Constrain sigma to a maximum reasonable size based on the user-supplied
  // limit to neighborhood size.
  if (sigma > this->GetMaximumNeighborhoodRadius())
    {
    sigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
    neighborhood_radius = this->GetMaximumNeighborhoodRadius();
    neighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
    this->ComputeAngularWeights(pos,neighborhood,domain,weights);
    }

  //  std::cout << idx <<  "\t SIGMA = " << sigma << "\t NEIGHBORHOOD SIZE = " << neighborhood.size()
  //            << "\t NEIGHBORHOOD RADIUS= " << neighborhood_radius << std::endl;
  
  // We are done with the sigma estimation step.  Cache the sigma value for
   // next time.
   m_SpatialSigmaCache->operator[](d)->operator[](idx) = sigma;

  //----------------------------------------------
 
   // Compute the gradients.
   double sigma2inv = 1.0 / (2.0* sigma * sigma + epsilon);

   VectorType r;
   VectorType gradE;

   for (unsigned int n = 0; n < VDimension; n++)
     {
     gradE[n] = 0.0;
     }
   
   double A = 0.0;
   for (unsigned int i = 0; i < neighborhood.size(); i++)
     {
     //    if ( neighborhood[i].Index == idx) continue;
     if (weights[i] < epsilon) continue;
     
     for (unsigned int n = 0; n < VDimension; n++)
       {
       // Note that the Neighborhood object has already filtered the
       // neighborhood for points whose normals differ by > 90 degrees.
       r[n] = pos[n] - neighborhood[i].Point[n];
       }
     
     double q = exp( -dot_product(r, r) * sigma2inv);
     A += q;

     for (unsigned int n = 0; n < VDimension; n++)
       {
       gradE[n] += weights[i] * r[n] * q;
       }
     }
   
   double p = 0.0;
   if (A > epsilon)
     {
     p = -1.0 / (A * sigma * sigma);
     }


   
   // TEST
   //   vnl_vector_fixed<float, VDimension> tosurf = domain->SampleGradientVnl(pos);
   //   float tosurfmag = tosurf.magnitude() + 1.0e-5;

   // end test
   //   float f = domain->Sample(pos);

   for (unsigned int n = 0; n <VDimension; n++)
     {
     gradE[n] *= p;
     // TEST
     //     gradE[n] += f * (tosurf[n] / tosurfmag);
     // end test
     }
   //   maxdt = sigma * sigma;
   maxdt = 0.5;
   
   return gradE;
}


}// end namespace
#endif
