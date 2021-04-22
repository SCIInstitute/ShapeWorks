/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSurfaceNeighborhood.txx,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSurfaceNeighborhood_txx
#define __itkParticleSurfaceNeighborhood_txx


namespace itk
{
template <class TImage>
typename ParticleSurfaceNeighborhood<TImage>::PointVectorType
ParticleSurfaceNeighborhood<TImage>
::FindNeighborhoodPoints(const PointType &center, int idx,
                         std::vector<double> &weights,
                         std::vector<double> &distances,
                         double radius) const
{
  const auto *domain = this->GetDomain();

  GradientVectorType posnormal;
  if(m_WeightingEnabled) { // uninitialized otherwise, but we're trying to avoid looking up the normal if we can
    posnormal = domain->SampleNormalAtPoint(center, idx);
  }

  weights.clear();
  distances.clear();

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
  ret.reserve(pointlist.size()); // todo investigate performance here: does it actually help?
  weights.reserve(pointlist.size());

  // Add any point whose distance from center is less than radius to the return list
  for (auto it = pointlist.begin(); it != pointlist.end(); it++)
  {
    const auto& pt_b = (*it)->Point;
    const auto& idx_b = (*it)->Index;

    // we are not a neighbor of ourself.
    if(idx_b == idx) {
      continue;
    }

    double distance;
    bool is_within_distance;

    if(m_ForceEuclidean) {
      distance = center.EuclideanDistanceTo(pt_b);
      is_within_distance = distance < radius;
    } else {
      is_within_distance = domain->IsWithinDistance(center, idx, pt_b, idx_b, radius, distance);
    }

    if (is_within_distance)
    {
      ret.push_back( **it );
      distances.push_back(distance);

      //todo change the APIs so don't have to pass a std::vector<double> of 1s whenever weighting is disabled
      if(!m_WeightingEnabled) {
        weights.push_back(1.0);
        continue;
      }

      const GradientVectorType pn = domain->SampleNormalAtPoint((*it)->Point, (*it)->Index);
      const double cosine  = dot_product(posnormal,pn); // normals already normalized
      if (cosine >= m_FlatCutoff)
      {
        weights.push_back(1.0);
      }
      else
      {
        // Drop to zero influence over 90 degrees.
        weights.push_back(cos((m_FlatCutoff - cosine) / (1.0+m_FlatCutoff) * 1.5708));

        // More quickly drop to zero influence
        // weights.push_back( exp((cosine - m_FlatCutoff) / (1.0 + m_FlatCutoff) * 4.0) );
      }
    }
  }
  return ret;
}

template <class TImage>
typename ParticleSurfaceNeighborhood<TImage>::PointVectorType
ParticleSurfaceNeighborhood<TImage>
::FindNeighborhoodPoints(const PointType &center, int idx,
                         std::vector<double> &weights,
                         double radius) const
{
  std::vector<double> distances;
  return this->FindNeighborhoodPoints(center, idx, weights, distances, radius);
}

}

#endif
