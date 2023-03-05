#pragma once

#include "ParticleImplicitSurfaceDomain.h"
#include "itkParticleRegionNeighborhood.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk {
/** \class ParticleSurfaceNeighborhood
 *
 * ParticleSurfaceNeighborhood is a general purpose neighborhood object that
 * computes neighborhoods based on distance from a point.  It requires a domain
 * that provides bounds information and a distance metric.  This class uses a
 * PowerOfTwoPointTree to cache point and index values so that
 * FindNeighborhoodPoints is somewhat optimized.
 */
template <class TImage>
class ParticleSurfaceNeighborhood : public ParticleRegionNeighborhood {
 public:
  /** Standard class typedefs */
  typedef TImage ImageType;
  typedef ParticleSurfaceNeighborhood Self;
  typedef ParticleRegionNeighborhood Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;
  typedef typename ImageType::PixelType NumericType;
  typedef typename Superclass::PointTreeType PointTreeType;

  typedef vnl_vector_fixed<NumericType, TImage::ImageDimension> GradientVectorType;

  /**Expose the image dimension. */
  itkStaticConstMacro(Dimension, unsigned int, 3);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleSurfaceNeighborhood, ParticleRegionNeighborhood);

  /** Inherited typedefs from parent class. */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef shapeworks::ParticleImplicitSurfaceDomain<typename TImage::PixelType> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  /** Compile a list of points that are within a specified radius of a given
      point.  This implementation uses a PowerOfTwoTree to sort points
      according to location. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, std::vector<double>&,
                                                 double) const override;
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&,
                                                 double) const override;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetWeightingEnabled(bool is_enabled) { m_WeightingEnabled = is_enabled; }

  bool IsWeightingEnabled() const { return m_WeightingEnabled; }

  void SetForceEuclidean(bool is_enabled) { m_ForceEuclidean = is_enabled; }

  bool IsForceEuclidean() const { return m_ForceEuclidean; }

  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os, indent); }

 protected:
  ParticleSurfaceNeighborhood() : m_FlatCutoff(0.30) {}
  virtual ~ParticleSurfaceNeighborhood(){};

 private:
  ParticleSurfaceNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);               // purposely not implemented
  double m_FlatCutoff;
  bool m_WeightingEnabled{true};
  bool m_ForceEuclidean{false};
};

}  // end namespace itk

#include "itkParticleSurfaceNeighborhood.txx"
