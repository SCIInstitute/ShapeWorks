/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleImageDomainWithCurvature.h"

#include "vnl/vnl_inverse.h"

#include "TriMesh.h"
#include "TriMesh_algo.h"

#include "meshFIM.h"

namespace itk
{
/** \class ParticleImplicitSurfaceDomain
 *
 *  A 3D cartesian domain that constrains points so that they always lie an
 *  implicit surface. The implicit surface is defined as the zero isosurface of
 *  the given image.   Constraints are applied using a Newton-Raphson
 *  iteration, and this class assumes it has a distance transform
 *  as an image.
 */
template <class T>
class ParticleImplicitSurfaceDomain : public ParticleImageDomainWithCurvature<T>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithCurvature<T> Superclass;
  typedef SmartPointer<ParticleImplicitSurfaceDomain>  Pointer;

  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  /** Method for creation through the object factory. */
  itkNewMacro(ParticleImplicitSurfaceDomain);

  /** Set/Get the precision of the projection operation.  The resulting projection
      will be within the specified tolerance. */
  virtual void SetTolerance(const T _Tolerance) {
    if (this->m_Tolerance != _Tolerance)
    {
      this->m_Tolerance = _Tolerance;
      this->Modified();
    }
  }
  virtual T GetTolerance() {
    return this->m_Tolerance;
  }

  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Image;
  }

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
  virtual bool ApplyConstraints(PointType &p, bool dbg = false) const override;


  inline PointType UpdateParticlePosition(const PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const override {
    PointType newpoint;

    // Master merge conflict

    //vnl_vector_fixed<float, DIMENSION> negativeUpdate;
    //for (unsigned int i = 0; i < DIMENSION; i++) { negativeUpdate[i] = -update[i]; }
    //for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] + negativeUpdate[i]; }

    for (unsigned int i = 0; i < 3; i++) { newpoint[i] = point[i] - update[i]; }


    //for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] - update[i]; }

    // debuggg
    ApplyConstraints(newpoint);

    // debuggg
    /*
    if(!this->GetConstraints()->IsAnyViolated(point) && this->GetConstraints()->IsAnyViolated(newpoint)){
        std::cerr << "####### Violation within apply constraints #######" << std::endl;
    }
    */

    /*
    if(point[2] >= 0 && newpoint[2] < 0){
        std::cerr << "NewPoint " << newpoint << std::endl;
        std::cerr << "Point " << point << std::endl;
        std::cerr << "Update " << update << std::endl;
    }
    */
    return newpoint;
  }

  void SetMesh(TriMesh *mesh);
  void SetFeaMesh(const char *feaFile);
  void SetFeaGrad(const char *feaGradFile);
  void SetFids(const char *fidsFile);
  meshFIM* GetMesh()
  {
      return m_mesh;
  }
  meshFIM* GetMesh() const
  {
      return m_mesh;
  }

  /** Get any valid point on the domain. This is used to place the first particle. */
    PointType GetZeroCrossingPoint() const override {
      PointType p;
      // TODO Hong
      // Return point that doesn't violate plane constraints.
      return p;
    }

protected:
  ParticleImplicitSurfaceDomain() : m_Tolerance(1.0e-4)
  {
    m_mesh = NULL;
  }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ParticleImplicitSurfaceDomain() {};

private:
  T m_Tolerance;
  
  meshFIM *m_mesh;

};

} // end namespace itk

#include "itkParticleImplicitSurfaceDomain.txx"
