/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleImageDomainWithGradients.h"
#include "itkImage.h"
#include "itkGradientImageFilter.h"
#include "itkFixedArray.h"
#include "itkImageDuplicator.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk
{
/** \class ParticleImageDomainWithHessians
 *
 * An image domain that extends ParticleImageDomainWithHessianGradients with Hessian
 * information.  Hessian values are interpolated with the SampleHessians(point)
 * method.  Hessians may be smoothed by specifying a sigma for Gaussian
 * blurring of the image prior to initialization.
 *
 * \sa ParticleImageDomain
 * \sa ParticleClipRegionDomain
 * \sa ParticleDomain
 */
template <class T>
class ParticleImageDomainWithHessians : public ParticleImageDomainWithGradients<T>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithGradients<T> Superclass;
  typedef SmartPointer<ParticleImageDomainWithHessians>  Pointer;

    /** Point type of the domain (not necessarily of the image). */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::ImageType ImageType;

  typedef vnl_matrix_fixed<T, DIMENSION, DIMENSION> VnlMatrixType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I, double narrow_band)
  {
    Superclass::SetImage(I, narrow_band);

    const auto grad = this->GetVDBGradient();

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

  /** Sample the Hessian at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleHessiansVnl returns a vnl
      matrix of size VDimension x VDimension. */
  inline VnlMatrixType SampleHessianVnl(const PointType &p) const
  {
    const auto coord = this->ToVDBCoord(p);

    VnlMatrixType hess;
    for(int i=0; i<DIMENSION; i++) {
      auto grad_n = openvdb::tools::BoxSampler::sample(m_VDBGradNorms[i]->tree(), coord);
      grad_n.normalize();
      hess.set(i, 0, grad_n[0]);
      hess.set(i, 1, grad_n[1]);
      hess.set(i, 2, grad_n[2]);
    }
    return hess;
  }


  inline vnl_matrix_fixed<float, DIMENSION, DIMENSION> SampleHessianAtPoint(const PointType &p, int idx) const override {
    return SampleHessianVnl(p);
  }
  
  /** Set /Get the standard deviation for blurring the image prior to
      computation of the Hessian derivatives.  This value must be set prior to
      initializing this class with an input image pointer and cannot be changed
      once the class is initialized.. */
  virtual void SetSigma(const double _Sigma) {
    if (this->m_Sigma != _Sigma) {
      this->m_Sigma = _Sigma;
      this->Modified();
    }
  }
  virtual double GetSigma() const {
    return m_Sigma;
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
  ParticleImageDomainWithHessians() : m_Sigma(0.0) {}
  virtual ~ParticleImageDomainWithHessians() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

  
private:
  double m_Sigma;
  typename openvdb::VectorGrid::Ptr m_VDBGradNorms[3];
};

} // end namespace itk
