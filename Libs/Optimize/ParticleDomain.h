#pragma once

#include "Constraints.h"
#include "DomainType.h"
#include "itkDataObject.h"
#include "itkPoint.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

class ParticleDomain {
 public:
  typedef std::shared_ptr<ParticleDomain> Pointer;

  /** Point type used to store particle locations. */
  using PointType = itk::Point<double, 3>;
  using GradNType = vnl_matrix_fixed<float, 3, 3>;
  using VectorDoubleType = vnl_vector_fixed<double, 3>;
  using VectorFloatType = vnl_vector_fixed<float, 3>;

  /** Apply any constraints to the given point location.
      This should force the point to a position on the surface that satisfies all constraints. */
  virtual bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const = 0;

  /** Applies the update to the point and returns the new point position. */
  // todo update should be const?
  virtual PointType UpdateParticlePosition(const PointType &point, int idx, VectorDoubleType &update) const = 0;

  virtual void InvalidateParticlePosition(int idx) const {}

  /** Projects the vector to the surface tangent at the point. */
  virtual VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType &gradE, const PointType &pos,
                                                         int idx) const = 0;
  virtual VectorFloatType SampleGradientAtPoint(const PointType &point, int idx) const = 0;
  virtual VectorFloatType SampleNormalAtPoint(const PointType &point, int idx) const = 0;
  virtual GradNType SampleGradNAtPoint(const PointType &p, int idx) const = 0;

  /** Distance between locations is used for computing energy and neighborhoods. Optionally
      return the gradient of the distance */
  virtual double Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                          VectorDoubleType *out_grad = nullptr) const {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = a[i] - b[i];
      }
    }
    return a.EuclideanDistanceTo(b);
  }
  /** Squared Distance between locations is used for computing sigma. */
  virtual double SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const {
    return a.SquaredEuclideanDistanceTo(b);
  }
  /** Returns whether or not the two points are separated by the given distance */
  virtual bool IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                                double &distance) const {
    distance = this->Distance(a, idx_a, b, idx_b);
    return distance < test_dist;
  }

  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetCurvature(const PointType &p, int idx) const = 0;
  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetSurfaceMeanCurvature() const = 0;
  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetSurfaceStdDevCurvature() const = 0;

  /** Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the
   * PowerOfTwoPointTree. */
  virtual const PointType &GetLowerBound() const = 0;
  /** Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the
   * PowerOfTwoPointTree. */
  virtual const PointType &GetUpperBound() const = 0;

  /** Get any valid point on the domain. This is used to place the first particle. */
  virtual PointType GetZeroCrossingPoint() const = 0;
  /** Use for neighborhood radius. */
  virtual double GetSurfaceArea() const = 0;

  /** GetValidLocation returns a PointType location on the surface. Used for placing the first particle. */
  virtual PointType GetValidLocationNear(PointType p) const = 0;
  /** GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma */
  virtual double GetMaxDiameter() const = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;
  virtual void UpdateZeroCrossingPoint() = 0;

  bool IsDomainFixed() const { return m_FixedDomain; }
  virtual shapeworks::DomainType GetDomainType() const = 0;

  std::shared_ptr<shapeworks::Constraints> GetConstraints() const { return constraints; }

  // Use `random` to advance a particle and return a new position
  virtual PointType GetPositionAfterSplit(const PointType &pt, const VectorDoubleType &local_direction,
                                          const VectorDoubleType &global_direction, double epsilon) const {
    // todo this has been copied from itkParticleSystem::AdvancedAllParticleSplitting.
    //  Ideally, we should compute a direction that is "consistent" depending on the domain type and use the
    //  `UpdateParticlePosition` API to advance the particle. See ContourDomain for an example. Leaving this be for
    //  now because we'd have to retest all MeshDomain and ImageDomain use cases if this behaviour changes.
    PointType new_pt;
    for (unsigned int k = 0; k < 3; k++) {
      new_pt[k] = pt[k] + epsilon * local_direction[k] / 5.;
    }
    return new_pt;
  }

  void SetDomainID(int id) { this->m_DomainID = id; }

  void SetDomainName(std::string name) { this->m_DomainName = name; }

 protected:
  // is this a fixed domain or not?  We start as fixed and if an image or mesh is set, we set this to false
  bool m_FixedDomain{true};

  int m_DomainID{-1};
  std::string m_DomainName;

  ParticleDomain() { this->constraints = std::make_shared<shapeworks::Constraints>(); }
  virtual ~ParticleDomain() {}

  std::shared_ptr<shapeworks::Constraints> constraints;

 private:
  ParticleDomain(const ParticleDomain &);  // purposely not implemented
  void operator=(const ParticleDomain &);  // purposely not implemented
};

}  // namespace shapeworks
