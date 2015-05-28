/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleEntropyFunction.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleEntropyFunction_txx
#define __itkParticleEnsembleEntropyFunction_txx

#include "itkParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include <string>

namespace itk
{


template <unsigned int VDimension>
void
ParticleEnsembleEntropyFunction<VDimension>
::WriteModes(const std::string &prefix, int n) const
{
  typename ParticleGaussianModeWriter<VDimension>::Pointer writer =
    ParticleGaussianModeWriter<VDimension>::New();
  writer->SetShapeMatrix(m_ShapeMatrix);
  writer->SetFileName(prefix.c_str());
  writer->SetNumberOfModes(n);
  writer->Update();
}

template <unsigned int VDimension>
void
ParticleEnsembleEntropyFunction<VDimension>
::ComputeCovarianceMatrix()
{ 
  // NOTE: This code requires that indices be contiguous, i.e. it wont work if
  // you start deleting particles.
  const unsigned int num_samples = m_ShapeMatrix->cols();
  const unsigned int num_dims    = m_ShapeMatrix->rows();

  
  // Do we need to resize the covariance matrix?
  if (m_PointsUpdate.rows() != num_dims || m_PointsUpdate.cols() != num_samples)
    {
    m_PointsUpdate.set_size(num_dims, num_samples);
    }
  
  vnl_matrix_type points_minus_mean(num_dims, num_samples);
  vnl_vector_type means(num_dims);
  
  // Compute the covariance matrix.
  // (A is D' in Davies paper)
  // Compute the mean shape vector.
  for (unsigned int j = 0; j < num_dims; j++)
    {
    double total = 0.0;
    for (unsigned int i = 0; i < num_samples; i++)
      {
      total += m_ShapeMatrix->operator()(j, i);
      }
    means(j) = total/(double)num_samples;
    }

  
  for (unsigned int j = 0; j < num_dims; j++)
    {
    for (unsigned int i = 0; i < num_samples; i++)
      {
      points_minus_mean(j, i) = m_ShapeMatrix->operator()(j, i) - means(j);
      }
    }
  
  
  vnl_matrix_type A =  points_minus_mean.transpose()
      * points_minus_mean * (1.0/((double)(num_samples-1)));
  
  // Regularize A
  for (unsigned int i = 0; i < num_samples; i++)
    {
    A(i, i) = A(i, i) + m_MinimumVariance;
    }
  
  // 
  vnl_symmetric_eigensystem<double> symEigen(A);
  m_PointsUpdate = points_minus_mean * ((symEigen.pinverse()).transpose());
  m_MinimumEigenValue = symEigen.D(0, 0);
  
  // double energy = 0.0;
  m_CurrentEnergy = 0.0;
  for (unsigned int i = 1; i < num_samples; i++)
    {
    if (symEigen.D(i, i) < m_MinimumEigenValue)
      {
      m_MinimumEigenValue = symEigen.D(i, i);
      }
    //    energy += log(symEigen.D(i,i));
   m_CurrentEnergy += log(symEigen.D(i,i));
    }
  m_CurrentEnergy /= num_samples;
  //    energy = 0.5*log(symEigen.determinant());
  
  for (unsigned int i =0; i < num_samples; i++)
    {
    std::cout << i << ": "<< symEigen.D(i, i) - m_MinimumVariance << std::endl;
    }



  std::cout << "pca mode, variance, percent variance, sum percent\n";
  double totalVariance = 0;
  for ( int c = 0; c < num_samples; c++ )
  {
    totalVariance += symEigen.D(c, c) - m_MinimumVariance;
  }

  double sum_variance = 0;
  for ( int c = num_samples-1; c >= 0; c-- )
  {
    double variance = symEigen.D(c, c) - m_MinimumVariance;
    sum_variance += variance;
    std::cout << "mode " << num_samples-c-1 << " : ";
    std::cout << variance << ", ";
    std::cout << variance / totalVariance * 100.0 << "%, ";
    std::cout << sum_variance / totalVariance * 100.0 << "%";
    std::cout<< "\n";
  }



  std::cout << "ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = "
            << m_MinimumVariance <<  std::endl;
}

template <unsigned int VDimension>
typename ParticleEnsembleEntropyFunction<VDimension>::VectorType
ParticleEnsembleEntropyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
  // NOTE: This code requires that indices be contiguous, i.e. it won't work if
  // you start deleting particles.
  energy = m_CurrentEnergy;
  maxdt  = m_MinimumEigenValue;
  const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();
  const unsigned int PointsPerDomain = system->GetNumberOfParticles(d);
   
  VectorType gradE;
  unsigned int k = ((d % DomainsPerShape) * PointsPerDomain * VDimension)
      + (idx * VDimension);
  for (unsigned int i = 0; i< VDimension; i++)
    { 
    // gradE[i] = m_PointsUpdate((idx * VDimension) + i, d);
    gradE[i] = m_PointsUpdate(k + i, d / DomainsPerShape); 
    }


  return system->TransformVector(gradE,
                   system->GetInversePrefixTransform(d) * 
                                 system->GetInverseTransform(d));
}



} // end namespace
#endif
