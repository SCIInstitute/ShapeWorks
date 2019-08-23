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
#ifndef __itkPSMSurfaceNeighborhood_hxx
#define __itkPSMSurfaceNeighborhood_hxx
#include "itkPSMSurfaceNeighborhood.h"

namespace itk
{
template <class TImage>
typename PSMSurfaceNeighborhood<TImage>::PointVectorType
PSMSurfaceNeighborhood<TImage>
::FindNeighborhoodPointsWithWeights(const PointType &center,
                         std::vector<double> &weights, double radius) const
{
  const DomainType *domain = dynamic_cast<const DomainType *>(this->GetDomain());
  GradientVectorType posnormal = domain->SampleNormalVnl(center, 1.0e-10);
  //  double posnormalmag = posnormal.magnitude();
  weights.clear();

  // Compute bounding box of the given hypersphere.
  PointType l, u;
  for (unsigned int i = 0; i < Dimension; i++)
    {
    l[i] = center[i] - radius;
    u[i] = center[i] + radius;
    }

  // Grab the list of points in this bounding box.
  typename PointTreeType::PointIteratorListType pointlist
    = Superclass::m_Tree->FindPointsInRegion(l, u);

  // Allocate return vector.  Reserve ensures no extra copies occur.
  PointVectorType ret;
  ret.reserve(pointlist.size());
  weights.reserve(pointlist.size());

  std::vector<double> vec_dist;
  std::vector<double> vec_cos;
  
  // Add any point whose distance from center is less than radius to the return
  // list.
  //  double vmax = radius;
  for (typename PointTreeType::PointIteratorListType::const_iterator it = pointlist.begin();
       it != pointlist.end(); it++)
    {
    //    double dist = this->GetDomain()->Distance(center, (*it)->Point);
    double sum = 0.0;
    for (unsigned int i = 0; i < Dimension; i++)
      {
      double q = center[i] - (*it)->Point[i];
      sum += q*q;
      }
    sum = sqrt(sum);
    
    if ( sum < radius && sum > 0.0 )
      {
      GradientVectorType pn = domain->SampleNormalVnl((*it)->Point, 1.0e-10);
      double cosine   = dot_product(posnormal,pn); // normals already normalized
      // double cosine = proj / (posnormalmag * pn.magnitude() + 1.0e-6);

      // double dist   = 

      
      if ( cosine >= m_FlatCutoff)
        {
        // Determine distance to tangent plane by projecting the point onto the
        // normal.
        weights.push_back(1.0);

        }
      else
        {
        // Drop to zero influence over 90 degrees.
        weights.push_back(cos((m_FlatCutoff - cosine) / (1.0+m_FlatCutoff) * 1.5708));

        // More quickly drop to zero influence
        // weights.push_back( exp((cosine - m_FlatCutoff) / (1.0 + m_FlatCutoff) * 4.0) );

        //        vmax = dist;
        }

      ret.push_back( **it );
      }

    }

  return ret;
}

}

#endif
