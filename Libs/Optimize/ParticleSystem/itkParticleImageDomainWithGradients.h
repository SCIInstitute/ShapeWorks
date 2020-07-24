/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkParticleImageDomain.h"
#include "itkGradientImageFilter.h"
#include "itkFixedArray.h"

namespace itk
{
/** \class ParticleImageDomainWithGradients
 *
 * An image domain that extends ParticleImageDomainWithGradients with image
 * gradient information.  Gradient values are interpolated with the
 * SampleGradients(point) method.
 *
 * \sa ParticleImageDomain
 * \sa ParticleClipRegionDomain
 * \sa ParticleDomain
 */
template <class T>
class ParticleImageDomainWithGradients : public ParticleImageDomain<T>
{
public:
  typedef SmartPointer<ParticleImageDomainWithGradients<T>>  Pointer;

  /** Point type of the domain (not necessarily of the image). */
  typedef typename ParticleImageDomain<T>::PointType PointType;
  typedef typename ParticleImageDomain<T>::ImageType ImageType;

  typedef FixedArray<T, DIMENSION> VectorType;
  typedef vnl_vector_fixed<T, DIMENSION> VnlVectorType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I, double narrow_band) {
    ParticleImageDomain<T>::SetImage(I, narrow_band);
    m_VDBGradient = openvdb::tools::gradient(*this->GetVDBImage());
  }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl
      vector of length VDimension instead of an itk::CovariantVector
      (itk::FixedArray). */
  inline VectorType SampleGradient(const PointType &p) const
  {
    if(this->IsInsideBuffer(p)) {
      const auto coord = this->ToVDBCoord(p);
      const auto _v = openvdb::tools::BoxSampler::sample(m_VDBGradient->tree(), coord);
      const VectorType v(_v.asPointer()); // This copies 3 floats from a VDB vector to a vnl vector
      return v;
    } else {
      itkExceptionMacro("Gradient queried for a Point, " << p << ", outside the given image domain." );
    }
  }
  inline VnlVectorType SampleGradientVnl(const PointType &p) const
  { 
    return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); 
  }
  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &p) const
  {
    vnl_vector_fixed<float, DIMENSION> grad = this->SampleGradientVnl(p);
    return grad.normalize();
  }

  /** This method is called by an optimizer after a call to Evaluate and may be
      used to apply any constraints the resulting vector, such as a projection
      to the surface tangent plane. Returns true if the gradient was modified.*/
  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const override
  {
    double dotprod = 0.0;  
    VnlVectorType normal =  this->SampleNormalAtPoint(pos);
    for (unsigned int i = 0; i < DIMENSION; i++) {   dotprod  += normal[i] * gradE[i]; }
    vnl_vector_fixed<double, DIMENSION> result;
    for (unsigned int i = 0; i < DIMENSION; i++) { result[i] = gradE[i] - normal[i] * dotprod; }
    return result;
  }

  /** Used when a domain is fixed. */
  void DeleteImages() override
  {
    ParticleImageDomain<T>::DeleteImages();
    m_VDBGradient = 0;
  }
  
protected:
  ParticleImageDomainWithGradients() {}
  virtual ~ParticleImageDomainWithGradients() {}

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleImageDomain<T>::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBGradient->activeVoxelCount() << std::endl;
  }
  
private:
  openvdb::VectorGrid::Ptr m_VDBGradient;
};

} // end namespace itk
