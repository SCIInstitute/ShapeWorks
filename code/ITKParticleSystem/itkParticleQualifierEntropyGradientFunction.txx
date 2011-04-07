/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleQualifierEntropyGradientFunction.txx,v $

  Date:      $Date: 2011/03/24 01:17:34 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleQualifierEntropyGradientFunction_txx

#define __itkParticleQualifierEntropyGradientFunction_txx



#include "itkParticleImageDomainWithGradients.h"

namespace itk {



template <class TGradientNumericType, unsigned int VDimension>

TGradientNumericType

ParticleQualifierEntropyGradientFunction<TGradientNumericType, VDimension>

::AngleCoefficient( const GradientVectorType& p_i_normal,  const GradientVectorType& p_j_normal) const

{

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

ParticleQualifierEntropyGradientFunction<TGradientNumericType, VDimension>

::ComputeAngularWeights(const PointType &pos,

                        const typename ParticleSystemType::PointVectorType &neighborhood,

                        const ParticleImageDomainWithGradients<TGradientNumericType, VDimension> *domain,

                        std::vector<double> &weights) const

{

  GradientVectorType posnormal = domain->SampleNormalVnl(pos, 1.0e-10);

  weights.resize(neighborhood.size());

  

  for (unsigned int i = 0; i < neighborhood.size(); i++)

    {

    weights[i] = this->AngleCoefficient(posnormal,

                                        domain->SampleNormalVnl(neighborhood[i].Point, 1.0e-10));

    if (weights[i] < 1.0e-5) weights[i] = 0.0;

    }

}



template <class TGradientNumericType, unsigned int VDimension>

double

ParticleQualifierEntropyGradientFunction<TGradientNumericType, VDimension>

::EstimateSigma(unsigned int idx, const typename ParticleSystemType::PointVectorType &neighborhood,

                const std::vector<double> &weights,

                const PointType &pos,  double initial_sigma,  double precision,

                int &err) const

{

  const double epsilon = 1.0e-5;

  

  double error = 1.0e6;

  double sigma, prev_sigma;

  sigma = initial_sigma;

  

  while (error > precision)

    {

    VectorType r_vec;

    //    double A = 0.0;

    //    double B = 0.0;

    //    double sigma3  = 3.0 / sigma;

    //    double sigma32 = 3.0 / (sigma * sigma);



    double C = 0.0;

    double D = 0.0;

    double PROB = 0.0;

    

    for (unsigned int i = 0; i < neighborhood.size(); i++)

      {

      for (unsigned int n = 0; n < VDimension; n++)

        {

        r_vec[n] = pos[n] - neighborhood[i].Point[n];

        }

      

      double r = r_vec.magnitude();

      double alpha = exp(-sigma * r);// * weights[i];

      //      double s3 = sigma3 -r;

      PROB += alpha;

      //      A += alpha * s3;

      //      B += alpha * (-sigma32 + (s3 * s3));

      C += alpha;

      D += alpha * r;      

      } // end for i



    if (C < 1.0e-6 || D < 1.0e-6)

      {

      err = 1;

      return sigma;

      }; // results are not meaningful



    prev_sigma = sigma;

    

    // Second order convergence update: Newton-Raphson

    //    sigma += A / ( B + epsilon);



    // First order convergence update: Fixed point iteration

    sigma = (3.0 * C) / (D+epsilon);

    

    error = 1.0 - fabs((sigma/prev_sigma));

    

    //    double PREVPROB = (PROB * prev_sigma * prev_sigma * prev_sigma) /

    //      (8.0 * 3.14159 * static_cast<double>(neighborhood.size()));   

    PROB = (PROB * sigma * sigma * sigma) /

      (8.0 * 3.14159 * static_cast<double>(neighborhood.size()));

    

    //    std::cout << "prev_sigma = " << prev_sigma << " sigma = " << sigma

    //              << " PREVPROB = " << PREVPROB << " PROB = " << PROB

    //              << " C = " << C << " D =  " << D << std::endl;

    } // end while (error > precision)

  err = 0;



  return sigma;

  

} // end estimate sigma



template <class TGradientNumericType, unsigned int VDimension>

typename ParticleQualifierEntropyGradientFunction<TGradientNumericType, VDimension>::VectorType

ParticleQualifierEntropyGradientFunction<TGradientNumericType, VDimension>

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



  //  if (sigma < epsilon)

  //    { sigma = m_MinimumNeighborhoodRadius / m_NeighborhoodToSigmaRatio;}

  

  // Determine the extent of the neighborhood that will be used in the Parzen

  // windowing estimation.

  double neighborhood_radius = (1.0 / sigma) * m_NeighborhoodToSigmaRatio;



  // Get the position for which we are computing the gradient.

  PointType pos = system->GetPosition(idx, d);

  

  // Get the neighborhood surrounding the point "pos".

  typename ParticleSystemType::PointVectorType neighborhood

    = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);

  

  // Compute the weights based on angle between the neighbors and the center.

  std::vector<double> weights;

  this->ComputeAngularWeights(pos,neighborhood,domain,weights);

  

  // Estimate the best sigma for Parzen windowing.  In some cases, such as when

  // the neighborhood may not include enough points, in these cases,

  // an error != 0 is returned, and we try the estimation again

  // with an increased neighborhood radius.

  int err;

  sigma =  this->EstimateSigma(idx, neighborhood,weights,pos, sigma, epsilon, err);

  

  while (err != 0)

    {

     neighborhood_radius *= 2.0;



     // Constrain the neighborhood size.

     if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius())

       {

       neighborhood_radius =  this->GetMaximumNeighborhoodRadius();

       break;

       }

     

     neighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);

     this->ComputeAngularWeights(pos,neighborhood,domain,weights);

     sigma = this->EstimateSigma(idx, neighborhood, weights, pos, sigma, epsilon, err);

    } // done while err

  

  // We are done with the sigma estimation step.  Cache the sigma value for

  // next time.

   m_SpatialSigmaCache->operator[](d)->operator[](idx) = sigma;



   // Compute the gradients.

   VectorType r;

   VectorType gradE;



   for (unsigned int n = 0; n < VDimension; n++)

     {

     gradE[n] = 0.0;

     }

   

   double PROB = 0.0;

   double mindist = 1.0e10;

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



     double dist = sqrt(dot_product(r,r));



     if (dist < mindist) mindist = dist;

     double q = sigma * exp(-dist * sigma);

     PROB += q;

     

     for (unsigned int n = 0; n < VDimension; n++)

       {

       gradE[n] += q * weights[i] * (r[n]/(dist + epsilon));

       }

     }

   

   double p = 0.0;

   if (PROB > epsilon)

     {

     p = -1.0 / PROB;

     }



   for (unsigned int n = 0; n <VDimension; n++)

     {

     gradE[n] *= p;

     }

   

   maxdt = mindist/2.0;



   return gradE;

}





}// end namespace

#endif

