#pragma once

#include "ParticleNeighborhood.h"
#include "PowerOfTwoPointTree.h"

namespace shapeworks {
/** \class ParticleRegionNeighborhood
 *
 * ParticleRegionNeighborhood is a general purpose neighborhood object that
 * computes neighborhoods based on distance from a point.  It requires a domain
 * that provides bounds information and a distance metric.  This class uses a
 * PowerOfTwoPointTree to cache point and index values so that
 * FindNeighborhoodPoints is somewhat optimized.
 */

class ParticleRegionNeighborhood : public ParticleNeighborhood {
 public:
  constexpr static unsigned int VDimension = 3;

  /** Standard class typedefs */
  typedef ParticleRegionNeighborhood Self;
  typedef ParticleNeighborhood Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;
  using DomainType = shapeworks::ParticleDomain;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleRegionNeighborhood, ParticleNeighborhood);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Inherited typedefs from parent class. */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;

  typedef typename Superclass::PointVectorType PointVectorType;

  /** PowerOfTwoTree type used to sort points according to location. */
  typedef PowerOfTwoPointTree<VDimension> PointTreeType;

  /** Compile a list of points that are within a specified radius of a given
      point.  This implementation uses a PowerOfTwoTree to sort points
      according to location. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, double) const;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  /** Override SetDomain so that we can grab the region extent info and
      construct our tree. */
  void SetDomain(DomainType::Pointer p) override;

  /** Set/Get the number of levels in the sorting tree.  */
  itkSetMacro(TreeLevels, unsigned int);
  itkGetMacro(TreeLevels, unsigned int);

  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    os << indent << "m_TreeLevels = " << m_TreeLevels << std::endl;
    m_Tree->PrintSelf(os, indent);
    Superclass::PrintSelf(os, indent);
  }

  /**  For efficiency, itkNeighborhoods are not necessarily observers of
      itkParticleSystem, but have specific methods invoked for various events.
      AddPosition is called by itkParticleSystem when a particle location is
      added.  SetPosition is called when a particle location is set.
      RemovePosition is called when a particle location is removed.*/
  virtual void AddPosition(const PointType& p, unsigned int idx, int threadId = 0);
  virtual void SetPosition(const PointType& p, unsigned int idx, int threadId = 0);
  virtual void RemovePosition(unsigned int idx, int threadId = 0);

 protected:
  ParticleRegionNeighborhood() : m_TreeLevels(3) {
    m_Tree = PointTreeType::New();
    m_IteratorMap = IteratorMapType::New();
  }
  virtual ~ParticleRegionNeighborhood(){};

  /** Map for direct reference of PointIndexPairs and Tree nodes from the
      PointTree.  This is used for fast removal or modification of point
      values. */
  struct IteratorNodePair {
    typedef typename PointTreeType::NodePointerType NodePointerType;
    typedef typename PointTreeType::PointListType::iterator IteratorType;

    IteratorNodePair() {}
    ~IteratorNodePair() {}
    IteratorNodePair(const IteratorType& p, const NodePointerType& n) {
      Iterator = p;
      NodePointer = n;
    }

    IteratorNodePair(const IteratorNodePair& o) { this->operator=(o); }
    const IteratorNodePair& operator=(const IteratorNodePair& o) {
      Iterator = o.Iterator;
      NodePointer = o.NodePointer;
      return *this;
    }

    IteratorType Iterator;
    NodePointerType NodePointer;
  };

  typedef GenericContainer<IteratorNodePair> IteratorMapType;

 protected:
  typename PointTreeType::Pointer m_Tree;
  typename IteratorMapType::Pointer m_IteratorMap;
  unsigned int m_TreeLevels;

 private:
  ParticleRegionNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);              // purposely not implemented
};

}  // end namespace shapeworks

