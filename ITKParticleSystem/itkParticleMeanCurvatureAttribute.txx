/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleMeanCurvatureAttribute.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

template <class TNumericType, unsigned int VDimension>
void
ParticleMeanCurvatureAttribute<TNumericType, VDimension>::
ComputeCurvatureStatistics(const ParticleSystemType *system, unsigned int d)
{
  typedef ParticleImageDomainWithCurvature<TNumericType, VDimension> DomainType;
  typedef typename DomainType::ImageType ImageType;

  // Loop through a zero crossing image, project all the zero crossing points
  // to the surface, and use those points to comput curvature stats.  
  typedef itk::ZeroCrossingImageFilter<ImageType, ImageType> ZeroCrossingImageFilterType ;
  typename ZeroCrossingImageFilterType::Pointer zc = ZeroCrossingImageFilterType::New() ;
  //itk::ZeroCrossingImageFilter<ImageType, ImageType>::Pointer zc
  //  = itk::ZeroCrossingImageFilter<ImageType, ImageType>::New();
  zc->SetInput( dynamic_cast<const DomainType *>(system->GetDomain(d))->GetImage() );
  zc->Update();
  
  itk::ImageRegionConstIteratorWithIndex<ImageType> it(zc->GetOutput(),
                                                       zc->GetOutput()->GetRequestedRegion());
  std::vector<double> datalist;
  m_MeanCurvatureList[d] = 0.0;
  m_CurvatureStandardDeviationList[d] = 0.0;
  const DomainType *domain = static_cast<const DomainType *>(system->GetDomain(d));
  
  for (; ! it.IsAtEnd(); ++it)
    {
    if (it.Get() == 1.0)
      {
      // Find closest pixel location to surface.
      PointType pos;
      //dynamic_cast<const DomainType
      //*>(system->GetDomain(d))->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
      domain->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
      
      // Project point to surface.
      // Make sure constraints are enabled
      //      bool c = domain->GetConstraintsEnabled();

      //      domain->EnableConstraints();
      domain->ApplyConstraints(pos);

      //      domain->SetConstraintsEnabled(c);
      
      // Compute curvature at point.
//      std::cout << "pos : " << pos[0] << ' ' << pos[1] << ' ' << pos[2] << std::endl;
      double mc = domain->GetCurvature(pos);      
      m_MeanCurvatureList[d] += mc;
      datalist.push_back(mc);
      }
    }
  double n = static_cast<double>(datalist.size());
  m_MeanCurvatureList[d] /= n;
  
  // Compute std deviation using point list
  for (unsigned int i = 0; i < datalist.size(); i++)
    {
    m_CurvatureStandardDeviationList[d] += (datalist[i] - m_MeanCurvatureList[d]) * (datalist[i] - m_MeanCurvatureList[d]);
    }
  m_CurvatureStandardDeviationList[d] = sqrt(m_CurvatureStandardDeviationList[d] / (n-1));
  
  std::cout << "Mean curvature magnitude = " << m_MeanCurvatureList[d] << std::endl;
  std::cout << "Std deviation = " << m_CurvatureStandardDeviationList[d] << std::endl;
}

} // end namespace itk
