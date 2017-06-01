/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleNonparametricEntropyFunction.txx,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleNonparametricEntropyFunction_txx
#define __itkParticleEnsembleNonparametricEntropyFunction_txx

#include "itkParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include <string>

namespace itk
{


template <unsigned int VDimension>
void
ParticleEnsembleNonparametricEntropyFunction<VDimension>
::WriteModes(const std::string &prefix, int n) const
{
  typename ParticleGaussianModeWriter<VDimension>::Pointer writer =
    ParticleGaussianModeWriter<VDimension>::New();
  writer->SetShapeMatrix(m_ShapeMatrix);
  writer->SetFileName(prefix.c_str());
  writer->SetNumberOfModes(n);
  writer->Update();
}

/// MODIFY TO COMPUTE THE PROJECTION MATRIX AND POSSIBLY STORE THE SIGMA (FOR
/// ESTIMATING 
template <unsigned int VDimension>
void
ParticleEnsembleNonparametricEntropyFunction<VDimension>
::ComputeProjectedShapeMatrix()
{
  // NOTE: This code requires that indices be contiguous, i.e. it wont work if
  // you start deleting particles.
  const int num_samples = m_ShapeMatrix->cols();
  
  const int num_dims    = m_ShapeMatrix->rows();
  
//   // Do we need to resize the projected shape matrix?
//   if (m_ProjectedShapeMatrix.rows() != num_dims || m_ProjectedShapeMatrix.cols() != num_samples)
//     {
//     m_ProjectedShapeMatrix.set_size(num_dims, num_samples);
//     }
  
  vnl_matrix_type points_minus_mean(num_dims, num_samples);
  vnl_vector_type means(num_dims);
  
  // Compute the covariance matrix.
  // (A is D' in Davies paper)
  // Compute the mean shape vector.
  for (int j = 0; j < num_dims; j++)
    {
    double total = 0.0;
    for (int i = 0; i < num_samples; i++)
      {
      total += m_ShapeMatrix->operator()(j, i);
      }
    means(j) = total/(double)num_samples;
    }

  
  for (int j = 0; j < num_dims; j++)
    {
    for (int i = 0; i < num_samples; i++)
      {
      points_minus_mean(j, i) = m_ShapeMatrix->operator()(j, i) - means(j);
      }
    }
  
  
  vnl_matrix_type A =  points_minus_mean.transpose()
      * points_minus_mean * (1.0/((double)(num_samples-1)));

  
  // Regularize A
  //  for (int i = 0; i < num_samples; i++)
  //    {
  //    A(i, i) = A(i, i) + m_MinimumVariance;
  //    }
  
  // 
  vnl_symmetric_eigensystem<double> symEigen(A);
  //  m_ProjectedShapeMatrix = points_minus_mean * ((symEigen.pinverse()).transpose());
  //  m_MinimumEigenValue = symEigen.D(0, 0);

  // Search for top percentage of variance.

  double energy = 0.0;
  for (int i = 1; i < num_samples; i++)
    {    energy += symEigen.D(i,i);    }


  
  // Create the projection matrix
  vnl_matrix_type projection_matrix;
  

  m_ProjectedShapeMatrix
  for (unsigned int i =0; i < num_samples; i++)
    {
    }


}


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
  sigma =  this->EstimateSigma(neighborhood,weights,pos, sigma, epsilon, err);
  
  while (err != 0)
    {
    neighborhood_radius *= 2.0;
    //    std::cout << "GROWING " << neighborhood_radius << std::endl;
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
    sigma = this->EstimateSigma(neighborhood, weights, pos, sigma, epsilon, err);
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

  //   std::cout << idx <<  "\t SIGMA = " << sigma << "\t NEIGHBORHOOD SIZE = " << neighborhood.size()
  //                 << "\t NEIGHBORHOOD RADIUS= " << neighborhood_radius << std::endl;;
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
   
   for (unsigned int n = 0; n <VDimension; n++)
     {
     gradE[n] *= p;
     }
   
   maxdt = sigma * sigma;
   
   return gradE;
}

} // end namespace
#endif
