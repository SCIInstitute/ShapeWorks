
#include "itkParticleRegionNeighborhood.h"

namespace itk {

void ParticleRegionNeighborhood::SetDomain(DomainType::Pointer d) {
  Superclass::SetDomain(d);
  m_Tree->ConstructTree(d->GetLowerBound(), d->GetUpperBound(), m_TreeLevels);
}

ParticleRegionNeighborhood::PointVectorType ParticleRegionNeighborhood::FindNeighborhoodPoints(const PointType& center,
                                                                                               int idx,
                                                                                               double radius) const {
  // Compute bounding box of the given hypersphere.
  PointType l, u;
  for (unsigned int i = 0; i < VDimension; i++) {
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
  for (typename PointTreeType::PointIteratorListType::const_iterator it = pointlist.begin(); it != pointlist.end();
       it++) {
    double distance = this->GetDomain()->Distance(center, idx, (*it)->Point, (*it)->Index);
    if (distance < radius && distance > 0) {
      ret.push_back(**it);
    }
  }

  return ret;
}

void ParticleRegionNeighborhood::AddPosition(const PointType& p, unsigned int idx, int) {
  // Cache this point and index into the tree.  AddPoint returns a pointer
  // to the cached values and the node in which the point resides.  This info
  // is saved for efficient moves and deletes.
  typename IteratorNodePair::IteratorType it;
  typename IteratorNodePair::NodePointerType node;
  it = m_Tree->AddPoint(p, idx, node);

  m_IteratorMap->operator[](idx) = IteratorNodePair(it, node);
}

void ParticleRegionNeighborhood::SetPosition(const PointType& p, unsigned int idx, int threadId) {
  // Check whether the given index has moved outside its current bin.  If it
  // has moved outside its current bin, delete and reinsert into the tree.
  IteratorNodePair pr = m_IteratorMap->operator[](idx);

  for (unsigned int i = 0; i < VDimension; i++) {
    if (p[i] < pr.NodePointer->GetLowerBound()[i] || p[i] > pr.NodePointer->GetUpperBound()[i]) {
      this->RemovePosition(idx, threadId);
      this->AddPosition(p, idx, threadId);
      return;
    }
  }

  // Otherwise, simply modify the given point value.
  pr.Iterator->Point = p;
}

void ParticleRegionNeighborhood::RemovePosition(unsigned int idx, int) {
  IteratorNodePair pr = m_IteratorMap->operator[](idx);
  pr.NodePointer->GetList().erase(pr.Iterator);
}

}  // namespace itk
