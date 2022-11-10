/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "ParticleImageDomainWithGradients.h"
#include "itkImage.h"
#include "itkGradientImageFilter.h"
#include "itkFixedArray.h"
#include "itkImageDuplicator.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "vnl/vnl_matrix_fixed.h"

namespace shapeworks
{
/** \class ParticleImageDomainWithGradN
 *
 * An image domain that extends ParticleImageDomainWithGradients with gradient of normals
 * information.
 *
 * \sa ParticleImageDomain
 * \sa ParticleClipRegionDomain
 * \sa ParticleDomain
 */
template <class T>
class ParticleImageDomainWithGradN : public ParticleImageDomainWithGradients<T>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithGradients<T> Superclass;
  using Pointer = std::shared_ptr<ParticleImageDomainWithGradN>;

    /** Point type of the domain (not necessarily of the image). */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::ImageType ImageType;

  typedef typename Superclass::GradNType GradNType;
  typedef typename Superclass::GradNType VnlMatrixType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I, double narrow_band)
  {
    Superclass::SetImage(I, narrow_band);

    const auto grad = this->GetVDBGradient();

    // Compute the gradient of normals component-wise
    for(int i=0; i<3; i++) {
      auto norm_i = openvdb::FloatGrid::create();
      norm_i->setTransform(this->transform());
      auto norm_i_accessor = norm_i->getAccessor();
      for(openvdb::VectorGrid::ValueOnCIter it = grad->cbeginValueOn(); it.test(); ++it) {
        const openvdb::Vec3f& v = *it;
        norm_i_accessor.setValue(it.getCoord(), v[i] / v.length());
      }

      m_VDBGradNorms[i] = openvdb::tools::gradient(*norm_i);
    }
  } // end setimage

  /** Sample the GradN at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradN returns a vnl
      matrix of size VDimension x VDimension. */
  inline GradNType SampleGradNAtPoint(const PointType &p, int idx) const override
  {
    const auto coord = this->ToVDBCoord(p);

    GradNType grad_n;
    for(int i=0; i<3; i++) {
      auto grad_ni = openvdb::tools::BoxSampler::sample(m_VDBGradNorms[i]->tree(), coord);
      grad_n.set(i, 0, grad_ni[0]);
      grad_n.set(i, 1, grad_ni[1]);
      grad_n.set(i, 2, grad_ni[2]);
    }
    return grad_n;
  }

  void DeletePartialDerivativeImages() override
  {
    for (unsigned int i = 0; i < DIMENSION; i++) {
      m_VDBGradNorms[i] = 0;
    }
  }

  /** Used when a domain is fixed. */
  void DeleteImages() override
  {
    Superclass::DeleteImages();
    DeletePartialDerivativeImages();
  }

protected:
  ParticleImageDomainWithGradN() {}
  virtual ~ParticleImageDomainWithGradN() {};

  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

  
private:
  typename openvdb::VectorGrid::Ptr m_VDBGradNorms[3];
};

} // end namespace itk
