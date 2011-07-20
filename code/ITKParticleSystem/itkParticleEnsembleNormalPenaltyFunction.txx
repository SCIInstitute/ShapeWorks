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
#include "itkParticleEnsembleNormalPenaltyFunction.h"
namespace itk
{
  template <unsigned int VDimension>
  typename ParticleEnsembleNormalPenaltyFunction<VDimension>::VectorType
  ParticleEnsembleNormalPenaltyFunction<VDimension>
  ::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
             double &maxmove, double &energy) const
  {

    const double epsilon = 1.0e-8;
    const double N = (double)(system->GetNumberOfDomains() / m_DomainsPerShape);

    // Get the position for which we are computing the gradient
    PointType pos = system->GetTransformedPosition(idx, d);

    // get domain information with gradients
    const ParticleImageDomainWithGradients<float, VDimension> * domain
          = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

    // get normal for current position
    typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType posnormal = domain->SampleNormalVnl(pos);

    // find mean normal for sister particles across ensemble 
    vnl_vector<double> mean_normal(VDimension,0.0f);
    for (unsigned int i = d % m_DomainsPerShape; i < system->GetNumberOfDomains(); i += m_DomainsPerShape)
    {
      PointType neighpos = system->GetTransformedPosition(idx, i);

      domain = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(i));
      typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType 
               neighnormal = domain->SampleNormalVnl(neighpos);

      for (unsigned int n = 0; n < VDimension; n++)
      {
        mean_normal[n] += neighnormal[n];
      }
    }
    for (unsigned int n = 0; n < VDimension; n++)
    {
      mean_normal[n] = mean_normal[n] / N;
    }
    mean_normal.normalize();

    // compute gradient descent direction
    vnl_vector<double> gradE_norm(VDimension,0.0);
    for (unsigned int n = 0; n < VDimension; n++)
    {
      gradE_norm[n] = posnormal[n] - mean_normal[n];
    }

    // compute normal partial derivatives on the fly
    domain = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

    // get local copy of image    
    imgDuplicator->SetInputImage(domain->GetImage());
    imgDuplicator->Update();
    
    // get normal components in neighborhood around current point
    typename NormalComponentImageType::SizeType nSize; 
    typename NormalComponentImageType::IndexType nIndex;
    typename NormalComponentImageType::RegionType nRegion;

    // set up neighborhood iterator around current particle position in image
    nSize.Fill(1); nRegion.SetSize(nSize);
    imgDuplicator->GetOutput()->TransformPhysicalPointToIndex(pos,nIndex); nRegion.SetIndex(nIndex);
    NeighborhoodIteratorType it(radius,imgDuplicator->GetOutput(),nRegion);

    // get components of surface normals in the neighborhood
    ImageIteratorType itImg(normComp,normComp->GetRequestedRegion());
    typename NormalComponentImageType::PointType currPoint;
    typename ParticleImageDomainWithGradients<float, VDimension>::VnlVectorType currNormal;

    // create kernel - directional derivative operator
    typedef typename itk::DerivativeOperator<float, VDimension> DerivativeOperatorType;
    DerivativeOperatorType derivativeOperator;
    derivativeOperator.CreateToRadius(radius);

    vnl_matrix<double> normalPartialDerivatives(VDimension,VDimension, 0.0f);
    nSize.Fill(1); nRegion.SetSize(nSize);
    nIndex.Fill(1); nRegion.SetIndex(nIndex);

    for (unsigned int comp = 0; comp < VDimension; comp++)
    {
      itImg = itImg.Begin(); 
      for (unsigned int o = 0; o < it.Size(); ++o, ++itImg)
      {      
        imgDuplicator->GetOutput()->TransformIndexToPhysicalPoint(it.GetIndex(o),currPoint);
        currNormal = domain->SampleNormalVnl(currPoint);
        itImg.Set(currNormal[comp]);
      }    

      // compute derivative using inner product
      NeighborhoodIteratorType itComp(radius,normComp,nRegion);
      for (unsigned int dirValue = 0; dirValue < VDimension; dirValue++)
      {
        derivativeOperator.SetDirection(dirValue);
        derivativeOperator.CreateDirectional();    
        normalPartialDerivatives(dirValue,comp) = inprod(itComp,derivativeOperator);
      }
    }

    // update gradient and compute energy
    gradE_norm *= normalPartialDerivatives;
    energy = gradE_norm.magnitude();
    
    maxmove = domain->GetImage()->GetSpacing()[0];
    //  maxmove = energy * 0.5;

    //  Transform the gradient according to the transform of the given domain and return.
    return system->TransformVector(gradE_norm, system->GetInversePrefixTransform(d)
                                               * system->GetInverseTransform(d));
  }

} // end namespace

#endif

