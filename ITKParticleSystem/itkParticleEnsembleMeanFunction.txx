/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleMeanFunction.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleMeanFunction_txx
#define __itkParticleEnsembleMeanFunction_txx

#include "vnl/vnl_vector_fixed.h"
#include "itkParticleImageDomainWithGradients.h"

namespace itk
{

template <unsigned int VDimension>
void
ParticleEnsembleMeanFunction<VDimension>
::ComputeUpdates()
{
    const unsigned int num_samples = m_ShapeMatrix->cols();
    const unsigned int num_dims    = m_ShapeMatrix->rows();

    if (m_PointsUpdate.rows() != num_dims || m_PointsUpdate.cols() != num_samples)
    {
        m_PointsUpdate.set_size(num_dims, num_samples);
        m_PointsUpdate = m_PointsUpdate.fill(0.0);
    }

    vnl_vector_type means(num_dims, 0.0);

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
            m_PointsUpdate(j, i) = m_ShapeMatrix->operator()(j, i) - means(j);
        }
    }
//     std::cout << m_PointsUpdate.extract(num_dims, num_samples,0,0) << std::endl;
}

template <unsigned int VDimension>
typename ParticleEnsembleMeanFunction<VDimension>::VectorType
ParticleEnsembleMeanFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{

    VectorType gradE;
    for (unsigned int i = 0; i < VDimension; i++) gradE[i] = 0.0;

    int dom = d % m_DomainsPerShape;

    unsigned int k = 0;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    for (unsigned int i = 0; i< VDimension; i++)
        gradE[i] = m_PointsUpdate(k + i, d / m_DomainsPerShape);

    energy = gradE.squared_magnitude() * 0.5;
//    maxmove = domain->GetImage()->GetSpacing()[0];
      maxmove = energy * 0.5;

    //  Transform the gradient according to the transform of the given domain and
    //  return.
    return system->TransformVector(gradE, system->GetInversePrefixTransform(d)
                                   * system->GetInverseTransform(d));
}


//template <unsigned int VDimension>
//typename ParticleEnsembleMeanFunction<VDimension>::VectorType
//ParticleEnsembleMeanFunction<VDimension>
//::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
//           double &maxmove, double &energy) const
//{
//  const ParticleImageDomainWithGradients<float, VDimension> * domain
//    = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

//  //  const double epsilon = 1.0e-8;
//  const double N = (double)(system->GetNumberOfDomains() / m_DomainsPerShape);

//  // Get the position for which we are computing the gradient.
//  PointType pos = system->GetTransformedPosition(idx, d);

//  // Find the mean position for the ENSEMBLE neighborhood.
//  VectorType gradE;
//  for (unsigned int n = 0; n < VDimension; n++)
//    {
//    gradE[n] = pos[n];
//    }
//  for (unsigned int i = d % m_DomainsPerShape; i < system->GetNumberOfDomains();
//       i += m_DomainsPerShape)
//    {
//    if (i != d)
//      {
//      PointType neighpos = system->GetTransformedPosition(idx, i);

//      for (unsigned int n = 0; n < VDimension; n++)
//        {
//        gradE[n] += neighpos[n];
//        }
//      }
//    }
//  for (unsigned int n = 0; n < VDimension; n++)
//    {
//    gradE[n] = pos[n] - (gradE[n] / N);
//    }



//  energy = gradE.magnitude();
//  maxmove = domain->GetImage()->GetSpacing()[0];
//  //  maxmove = energy * 0.5;

//  //  Transform the gradient according to the transform of the given domain and
//  //  return.
//  return system->TransformVector(gradE, system->GetInversePrefixTransform(d)
//                                 * system->GetInverseTransform(d));
//}



} // end namespace
#endif
