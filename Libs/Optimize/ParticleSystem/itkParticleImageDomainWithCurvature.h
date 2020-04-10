/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleImageDomainWithHessians.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkDiscreteGaussianImageFilter.h"

namespace itk
{
/** \class ParticleImageDomainWithCurvature
 *
 * An image domain that extends ParticleImageDomainWithHessianGradients with Hessian
 * information.  Hessian values are interpolated with the SampleCurvature(point)
 * method.  Curvature may be smoothed by specifying a sigma for Gaussian
 * blurring of the image prior to initialization.
 *
 * \sa ParticleImageDomain
 * \sa ParticleClipRegionDomain
 * \sa ParticleDomain
 */
template <class T, unsigned int VDimension=3>
class ParticleImageDomainWithCurvature : public ParticleImageDomainWithHessians<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithHessians<T, VDimension> Superclass;
  typedef SmartPointer<ParticleImageDomainWithCurvature<T, VDimension>>  Pointer;

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    // Computes partial derivatives in parent class
    Superclass::SetImage(I);
    m_VDBCurvature = openvdb::tools::meanCurvature(*this->GetVDBImage());
  } // end setimage

  double GetCurvature(const PointType &p) const
  {
    const auto coord = this->ToVDBCoord(p);
    return openvdb::tools::BoxSampler::sample(m_VDBCurvature->tree(), coord);
  }

protected:
  ParticleImageDomainWithCurvature() {}
  virtual ~ParticleImageDomainWithCurvature() {}

private:
  openvdb::FloatGrid::Ptr m_VDBCurvature;
};

} // end namespace itk
