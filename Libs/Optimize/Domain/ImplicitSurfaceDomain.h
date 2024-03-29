#pragma once

#include "ImageDomainWithCurvature.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_math.h"

namespace shapeworks {
/** \class ParticleImplicitSurfaceDomain
 *
 *  A 3D cartesian domain that constrains points so that they always lie an
 *  implicit surface. The implicit surface is defined as the zero isosurface of
 *  the given image.   Constraints are applied using a Newton-Raphson
 *  iteration, and this class assumes it has a distance transform
 *  as an image.
 */
template <class T>
class ImplicitSurfaceDomain : public ImageDomainWithCurvature<T> {
 public:
  /** Standard class typedefs */
  typedef ImageDomainWithCurvature<T> Superclass;
  typedef std::shared_ptr<ImplicitSurfaceDomain> Pointer;

  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  /** Set/Get the precision of the projection operation.  The resulting projection
      will be within the specified tolerance. */
  virtual void SetTolerance(const T _Tolerance) {
    if (this->m_Tolerance != _Tolerance) {
      this->m_Tolerance = _Tolerance;
      // this->Modified();
    }
  }
  virtual T GetTolerance() { return this->m_Tolerance; }

  shapeworks::DomainType GetDomainType() const override { return shapeworks::DomainType::Image; }

  /** Apply any constraints to the given point location.  This method
      constrains points to lie within the given domain and on a given implicit
      surface.  If the point is not already on the surface, it is projected
      back to the surface using a Newton-Raphson iteration.  IMPORTANT: This
      method returns the true/false value of its superclass, and does not
      indicate changes only due to projection.  This is done for speed: we
      typically will only want to know if a point tried to move outside of the
      bounding box domain, since movement off the surface will be very
      common.  Consider subclassing this method to add a check for significant
      differences in the input and output points. */
  virtual bool ApplyConstraints(PointType& p, int idx, bool dbg = false) const override {
    // First apply and constraints imposed by superclasses.  This will
    // guarantee the point starts in the correct image domain.
    bool flag = Superclass::ApplyConstraints(p);

    unsigned int k = 0;
    double mult = 1.0;

    const T epsilon = m_Tolerance * 0.001;
    T f = this->Sample(p);

    T gradmag = 1.0;
    while (fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
    //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
    {
      PointType p_old = p;
      // vnl_vector_fixed<T, DIMENSION> grad = -this->SampleGradientAtPoint(p);
      vnl_vector_fixed<T, DIMENSION> gradf = this->SampleGradientAtPoint(p, idx);
      vnl_vector_fixed<double, DIMENSION> grad;
      grad[0] = double(gradf[0]);
      grad[1] = double(gradf[1]);
      grad[2] = double(gradf[2]);

      gradmag = grad.magnitude();
      // vnl_vector_fixed<T, DIMENSION> vec = grad * (f / (gradmag + epsilon));
      vnl_vector_fixed<double, DIMENSION> vec = grad * (double(f) / (gradmag + double(epsilon)));

      for (unsigned int i = 0; i < DIMENSION; i++) {
        p[i] -= vec[i];
      }

      f = this->Sample(p);

      // Raise the tolerance if we have done too many iterations.
      k++;
      if (k > 10000) {
        mult *= 2.0;
        k = 0;
      }
    }  // end while
    return flag;
  };

  inline PointType UpdateParticlePosition(const PointType& point, int idx,
                                          vnl_vector_fixed<double, DIMENSION>& update) const override {
    PointType newpoint;

    for (unsigned int i = 0; i < 3; i++) {
      newpoint[i] = point[i] - update[i];
    }

    ApplyConstraints(newpoint, idx);

    return newpoint;
  }


  /** Get any valid point on the domain. This is used to place the first particle. */
  PointType GetZeroCrossingPoint() const override {
    PointType p;
    // TODO Hong
    // Return point that doesn't violate plane constraints.
    return p;
  }

  ImplicitSurfaceDomain() : m_Tolerance(1.0e-4) { }
  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ImplicitSurfaceDomain(){};

 private:
  T m_Tolerance;


};

}  // end namespace shapeworks
