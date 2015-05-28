/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleRegionNeighborhood.txx,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleRegionNeighborhood_txx
#define __itkParticleRegionNeighborhood_txx

namespace itk
{
template<unsigned int VDimension>
void ParticleRegionNeighborhood<VDimension>::SetDomain(DomainType *d)
{
  Superclass::SetDomain(d);
  m_Tree->ConstructTree(d->GetLowerBound(), d->GetUpperBound(), m_TreeLevels);
}

// template <unsigned int VDimension>
// unsigned int
// ParticleRegionNeighborhood<VDimension>
// ::FindNeighborhoodPoints(const PointType &center, double radius,
//                          PointVectorType &ret) const
// {
// // Compute bounding box of the given hypersphere.
//   PointType l, u;
//   for (unsigned int i = 0; i < VDimension; i++)
//     {
//     l[i] = center[i] - radius;
//     u[i] = center[i] + radius;
//     }

//   // Grab the list of points in this bounding box.
//   typename PointTreeType::PointIteratorListType pointlist = m_Tree->FindPointsInRegion(l, u);

//   // Allocate return vector.  Reserve ensures no extra copies occur.
//   //  PointVectorType ret;
//   ret.reserve(pointlist.size());

//   // Add any point whose distance from center is less than radius to the return
//   // list.
//   unsigned int k = 0;
//   for (typename PointTreeType::PointIteratorListType::const_iterator it = pointlist.begin();
//        it != pointlist.end(); it++)
//     {
//     //    double dist = this->GetDomain()->Distance(center, (*it)->Point);
//     double sum = 0.0;
//     for (unsigned int i = 0; i < VDimension; i++)
//       {
//       double q = center[i] - (*it)->Point[i];
//       sum += q*q;
//       }
//     sum = sqrt(sum);
//     if (sum< radius && sum >0 )
//       {
//       ret[k] = ( **it );
//       k++;
//       }
//     }
//   ret.resize(k); 
//   return k;
// }

template <unsigned int VDimension>
typename ParticleRegionNeighborhood<VDimension>::PointVectorType
ParticleRegionNeighborhood<VDimension>
::FindNeighborhoodPoints(const PointType &center, double radius) const
{
  // Compute bounding box of the given hypersphere.
  PointType l, u;
  for (unsigned int i = 0; i < VDimension; i++)
    {
    l[i] = center[i] - radius;
    u[i] = center[i] + radius;
    }

  // Grab the list of points in this bounding box.
  typename PointTreeType::PointIteratorListType pointlist = m_Tree->FindPointsInRegion(l, u);

  // Allocate return vector.  Reserve ensures no extra copies occur.
  PointVectorType ret;
  ret.reserve(pointlist.size());
  
  // Add any point whose distance from center is less than radius to the return
  // list.
  for (typename PointTreeType::PointIteratorListType::const_iterator it = pointlist.begin();
       it != pointlist.end(); it++)
    {
    //    double dist = this->GetDomain()->Distance(center, (*it)->Point);
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {
      double q = center[i] - (*it)->Point[i];
      sum += q*q;
      }
    sum = sqrt(sum);
    
    if ( sum < radius && sum >0 )
      {
      ret.push_back( **it );
      }
    }
   
  return ret;
}

template <unsigned int VDimension>
void ParticleRegionNeighborhood<VDimension>
::AddPosition(const PointType &p, unsigned int idx, int)
{
  // Cache this point and index into the tree.  AddPoint returns a pointer
  // to the cached values and the node in which the point resides.  This info
  // is saved for efficient moves and deletes.
  typename IteratorNodePair::IteratorType it;
  typename IteratorNodePair::NodePointerType node;
  it = m_Tree->AddPoint(p, idx, node);
  
  m_IteratorMap->operator[](idx) =  IteratorNodePair(it, node);
}

template <unsigned int VDimension>
void ParticleRegionNeighborhood<VDimension>
::SetPosition(const PointType &p, unsigned int idx, int threadId)
{
  // Check whether the given index has moved outside its current bin.  If it
  // has moved outside its current bin, delete and reinsert into the tree.
  IteratorNodePair pr = m_IteratorMap->operator[](idx);

  for (unsigned int i = 0; i < VDimension; i++)
    {
    if (p[i] < pr.NodePointer->GetLowerBound()[i] || p[i] > pr.NodePointer->GetUpperBound()[i])
      {
      this->RemovePosition(idx, threadId);
      this->AddPosition(p, idx, threadId);
      return;
      }
    }

  // Otherwise, simply modify the given point value.
  pr.Iterator->Point = p;
}

template <unsigned int VDimension>
void ParticleRegionNeighborhood<VDimension>
::RemovePosition(unsigned int idx, int)
{
  IteratorNodePair pr = m_IteratorMap->operator[](idx);
  m_IteratorMap->Erase(idx);
  pr.NodePointer->GetList().erase(pr.Iterator); 
}



}

#endif
