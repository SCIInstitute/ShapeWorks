/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMShapeEntropyFunction_hxx
#define __itkPSMShapeEntropyFunction_hxx
#include "itkPSMShapeEntropyFunction.h"

#include "vnl/algo/vnl_symmetric_eigensystem.h"
//#include "itkParticleGaussianModeWriter.h"
#include <string>

namespace itk
{

//template <unsigned int VDimension>
//void
//PSMShapeEntropyFunction<VDimension>
//::WriteModes(const std::string &prefix, int n) const
//{
//  typename ParticleGaussianModeWriter<VDimension>::Pointer writer =
//  ParticleGaussianModeWriter<VDimension>::New();
// writer->SetShapeMatrix(m_ShapeMatrix);
// writer->SetFileName(prefix.c_str());
// writer->SetNumberOfModes(n);
// writer->Update();
//}

template <unsigned int VDimension>
void
PSMShapeEntropyFunction<VDimension>
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
  
  // Compute the covariance matrix and the mean shape vector.
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
  
  // Compute the eigenvalues and vectors and the point updates.
  vnl_symmetric_eigensystem<double> symEigen(A);
  m_PointsUpdate = points_minus_mean * ((symEigen.pinverse()).transpose());
  m_MinimumEigenValue = symEigen.D(0, 0);
  
  m_CurrentEnergy = 0.0;
  for (unsigned int i = 1; i < num_samples; i++)
    {
    if (symEigen.D(i, i) < m_MinimumEigenValue)
      {
      m_MinimumEigenValue = symEigen.D(i, i);
      }
   m_CurrentEnergy += log(symEigen.D(i,i));
    }
  // m_CurrentEnergy = 0.5*log(symEigen.determinant());
  m_CurrentEnergy /= num_samples;

  // Record the variance for each PCA shape "parameter" (mode).
  // Variance is the eigenvalue of the covariance matrix.  Reverse
  // order of values because we want them in decreasing order of
  // magnitude.
  if (m_ShapePCAVariances.size() < num_samples)
    { 
      m_ShapePCAVariances.resize(num_samples);
    }
  for (int i = 0; i < (int)num_samples; i++)
     {
       int tmp = num_samples - i - 1;
       m_ShapePCAVariances[i] = symEigen.D(tmp, tmp) - m_MinimumVariance;
     }
}
  
template <unsigned int VDimension>
typename PSMShapeEntropyFunction<VDimension>::VectorType
PSMShapeEntropyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
  // NOTE: This code requires that indices be contiguous, i.e. it
  // won't work if you start deleting particles.
  energy = m_CurrentEnergy;
  maxdt  = m_MinimumEigenValue;
  const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();
  const unsigned int PointsPerDomain = system->GetNumberOfParticles(d);
   
  VectorType gradE;
  unsigned int k = ((d % DomainsPerShape) * PointsPerDomain * VDimension)
      + (idx * VDimension);
  for (unsigned int i = 0; i< VDimension; i++)
    { 
    gradE[i] = m_PointsUpdate(k + i, d / DomainsPerShape); 
    }


  return system->TransformVector(gradE,
                   system->GetInversePrefixTransform(d) * 
                                 system->GetInverseTransform(d));
}

} // end namespace itk
#endif
