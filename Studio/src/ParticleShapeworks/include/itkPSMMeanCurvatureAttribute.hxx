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
#ifndef __itkPSMMeanCurvatureAttribute_hxx
#define __itkPSMMeanCurvatureAttribute_hxx
#include "itkPSMMeanCurvatureAttribute.h"

#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

template <class TNumericType, unsigned int VDimension>
void
PSMMeanCurvatureAttribute<TNumericType, VDimension>::
ComputeCurvatureStatistics(const ParticleSystemType *system, unsigned int d)
{
  typedef PSMImageDomainWithCurvature<TNumericType, VDimension> DomainType;
  typedef typename DomainType::ImageType ImageType;

  // Loop through a zero crossing image, project all the zero crossing points
  // to the surface, and use those points to comput curvature stats.  
  typedef itk::ZeroCrossingImageFilter<ImageType, ImageType> ZeroCrossingImageFilterType;
  typename ZeroCrossingImageFilterType::Pointer zc = ZeroCrossingImageFilterType::New() ;
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
          domain->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
          
          // Project point to surface.
          // Make sure constraints are enabled
          //      bool c = domain->GetConstraintsEnabled();
          //      domain->EnableConstraints();
          domain->ApplyConstraints(pos);
          //      domain->SetConstraintsEnabled(c);
          
          // Compute curvature at point.
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
}
  
} // end namespace itk

#endif
