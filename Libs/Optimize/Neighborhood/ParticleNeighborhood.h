#pragma once

#include <vector>

#include "Libs/Optimize/Container/GenericContainer.h"
#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "ParticlePointIndexPair.h"
#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace shapeworks {
/** \class ParticleNeighborhood
 *
 * A ParticleNeighborhood is responsible for computing neighborhoods of
 * particles.  Given a point position in a domain, and a neighborhood radius,
 * the ParticleNeighborhood returns a list of points that are neighbors of that
 * point.  The base class, ParticleNeighborhood, must be subclassed to provide
 * functionality; the base class will throw an exception when
 * FindNeighborhoodPoints is called.
 */

class ParticleSystem;
class ParticleNeighborhoodTwo {
 public:
  ParticleNeighborhoodTwo(ParticleSystem* ps, int domain_id = -1) : ps_(ps), domain_id_(domain_id) {}

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights,
                                                               std::vector<double>& distances, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               double radius);

  void SetWeightingEnabled(bool is_enabled) { weighting_enabled_ = is_enabled; }

  bool IsWeightingEnabled() const { return weighting_enabled_; }

  void SetForceEuclidean(bool is_enabled) { force_euclidean_ = is_enabled; }

  bool IsForceEuclidean() const { return force_euclidean_; }

  void SetDomain(ParticleDomain::Pointer domain) { domain_ = domain; };
  ParticleDomain::Pointer GetDomain() const { return domain_; };

  void set_domain_id(int id) { domain_id_ = id; }

 private:
  std::vector<ParticlePointIndexPair> get_points_in_sphere(const itk::Point<double, 3>& position, int id,
                                                           double radius);

  ParticleSystem* ps_;
  ParticleDomain::Pointer domain_;
  int domain_id_{-1};
  double flat_cutoff_{0.3};
  bool weighting_enabled_{true};
  bool force_euclidean_{false};
};

class ParticleNeighborhood : public itk::DataObject {
 public:
  constexpr static unsigned int VDimension = 3;
  /** Standard class typedefs */
  typedef ParticleNeighborhood Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleNeighborhood, DataObject);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef itk::Point<double, VDimension> PointType;

  /** Domain type.  The Domain object provides bounds and distance
      information. */
  using DomainType = shapeworks::ParticleDomain;

  /** Container type for points.  This matches the itkParticleSystem container
      type. */
  typedef GenericContainer<PointType> PointContainerType;

  /** Point list (vector) type.  This is the type of list returned by FindNeighborhoodPoints. */
  typedef std::vector<ParticlePointIndexPair> PointVectorType;

  /** Set/Get the point container.  These are the points parsed by the
      Neighborhood class when FindNeighborhoodPoints is called. */
  itkSetObjectMacro(PointContainer, PointContainerType);
  itkGetConstObjectMacro(PointContainer, PointContainerType);

  /** Compile a list of points that are within a specified radius of a given
      point.  The default implementation will throw an exception. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  /** This method finds neighborhood points as in the previous method, but also
      computes a vector of weights associated with each of those points. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  /** This method finds neighborhood points as in the previous method, but also
      computes a vector of distances associated with each of those points. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, std::vector<double>&,
                                                 double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual unsigned int FindNeighborhoodPoints(const PointType&, int idx, double, PointVectorType&) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
    return 0;
  }

  /** Set the Domain that this neighborhood will use.  The Domain object is
      important because it defines bounds and distance measures. */
  // itkSetObjectMacro(Domain, DomainType);
  // itkGetConstObjectMacro(Domain, DomainType);
  virtual void SetDomain(DomainType::Pointer domain) {
    m_Domain = domain;
    this->Modified();
  };
  DomainType::Pointer GetDomain() const { return m_Domain; };

  /**  For efficiency, itkNeighborhoods are not necessarily observers of
      itkParticleSystem, but have specific methods invoked for various events.
      AddPosition is called by itkParticleSystem when a particle location is
      added.  SetPosition is called when a particle location is set.
      RemovePosition is called when a particle location is removed.*/
  virtual void AddPosition(const PointType& p, unsigned int idx, int threadId = 0) {}
  virtual void SetPosition(const PointType& p, unsigned int idx, int threadId = 0) {}
  virtual void RemovePosition(unsigned int idx, int threadId = 0) {}

 protected:
  ParticleNeighborhood() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }
  virtual ~ParticleNeighborhood(){};

 private:
  ParticleNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);        // purposely not implemented

  typename PointContainerType::Pointer m_PointContainer;
  typename DomainType::Pointer m_Domain;
};

}  // end namespace shapeworks
