/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleNormalPenaltyFunction.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleNormalPenaltyFunction_txx
#define __itkParticleEnsembleNormalPenaltyFunction_txx

#include "vnl/vnl_vector_fixed.h"
#include "itkParticleImageDomainWithGradients.h"

namespace itk
{

template <unsigned int VDimension>
typename ParticleEnsembleNormalPenaltyFunction<VDimension>::VectorType
ParticleEnsembleNormalPenaltyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{
  const ParticleImageDomainWithGradients<float, VDimension> * domain
    = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));
  
  //  const double epsilon = 1.0e-8;
  const double N = (double)(system->GetNumberOfDomains() / m_DomainsPerShape);

  // Get the position for which we are computing the gradient.
  PointType pos = system->GetTransformedPosition(idx, d);

  // get normal for current position
  typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType posnormal = domain->SampleNormalVnl(pos);

  // find mean normal for ensemble neighborhood
  vnl_vector<double> mean_normal(VDimension,0.0f);
  for (unsigned int n = 0; n < VDimension; n++)
  {
    mean_normal[n] = posnormal[n];
  }

  for (unsigned int i = d % m_DomainsPerShape; i < system->GetNumberOfDomains(); i += m_DomainsPerShape)
  {
    if (i != d)
    {
      domain = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(i));
      PointType neighpos = system->GetTransformedPosition(idx, i);
      typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType neighnormal = domain->SampleNormalVnl(neighpos);

      for (unsigned int n = 0; n < VDimension; n++)
      {
        mean_normal[n] += neighnormal[n];
      }
    }
  }

  for (unsigned int n = 0; n < VDimension; n++)
  {
    mean_normal[n] = mean_normal[n] / N;
  }

  mean_normal.normalize();

  //compute gradient
  vnl_matrix<double> normPartialDerivatives;
  normPartialDerivatives.set_size(VDimension,VDimension);
  normPartialDerivatives.fill(0.0f);

  domain = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

  for (unsigned int normComp = 0; normComp < VDimension; normComp++)
  {
    for (unsigned int dirValue = 0; dirValue < VDimension; dirValue++)
    {
      normPartialDerivatives(normComp,dirValue) = domain->SampleNormalPartialDerivative(normComp,dirValue,pos);
    }
  }

  vnl_vector<double> gradE_norm(VDimension,0.0);;
  
  for (unsigned int n = 0; n < VDimension; n++)
  {
    gradE_norm[n] = mean_normal[n] - posnormal[n];
  }

  gradE_norm *= normPartialDerivatives;

  energy = gradE_norm.magnitude();

  // compute gradient & energy
  maxmove = domain->GetImage()->GetSpacing()[0];
  //  maxmove = energy * 0.5;

  //  Transform the gradient according to the transform of the given domain and
  //  return.
  return system->TransformVector(gradE_norm, system->GetInversePrefixTransform(d)
                                 * system->GetInverseTransform(d));
}



} // end namespace

#endif

