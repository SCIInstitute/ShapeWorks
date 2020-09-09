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

    // Load the i-th hessian from an itk Image
    const auto LoadVDBHessian = [&](const int i, const typename ImageType::Pointer hess) {
      m_VDBHessians[i] = openvdb::FloatGrid::create(0.0);
      m_VDBHessians[i]->setTransform(this->transform());
      auto vdbAccessor = m_VDBHessians[i]->getAccessor();

      ImageRegionIterator<ImageType> hessIt(hess, hess->GetRequestedRegion());
      ImageRegionIterator<ImageType> it(I, I->GetRequestedRegion());
      for (; !hessIt.IsAtEnd(); ++hessIt, ++it) {
        const auto idx = hessIt.GetIndex();
        if(idx != it.GetIndex()) {
          // We are relying on the assumption that the iteration over the
          // distance transform and its hessian proceed in the same order.
          // If this assumption is violated, throw an error.
          throw std::runtime_error("Bad index: iterators for Image and Hessian out of sync");
        }
        const auto hess = hessIt.Get();
        const auto pixel = it.Get();
        if(abs(pixel) > narrow_band) {
          continue;
        }

        const auto coord = openvdb::Coord(idx[0], idx[1], idx[2]);
        vdbAccessor.setValue(coord, hess);
      }
    };

    typename DiscreteGaussianImageFilter<ImageType, ImageType>::Pointer
      gaussian = DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    gaussian->SetVariance(m_Sigma * m_Sigma);
    gaussian->SetInput(I);
    gaussian->SetUseImageSpacingOn();
    gaussian->Update();
    
    // Compute the second derivatives
    for (unsigned int i = 0; i < DIMENSION; i++)
      {
      typename DerivativeImageFilter<ImageType, ImageType>::Pointer
        deriv = DerivativeImageFilter<ImageType, ImageType>::New();
      deriv->SetInput(gaussian->GetOutput());
      deriv->SetDirection(i);
      deriv->SetOrder(2);
      deriv->SetUseImageSpacingOn();
      deriv->Update();

      const auto hess = deriv->GetOutput();
      LoadVDBHessian(i, hess);

      }

    // Compute the cross derivatives and set up the interpolators
    unsigned int k = DIMENSION;
    for (unsigned int i = 0; i < DIMENSION; i++)
      {
      for (unsigned int j = i+1; j < DIMENSION; j++, k++)
        {
        typename DerivativeImageFilter<ImageType, ImageType>::Pointer
          deriv1 = DerivativeImageFilter<ImageType, ImageType>::New();
        deriv1->SetInput(gaussian->GetOutput());
        deriv1->SetDirection(i);
        deriv1->SetUseImageSpacingOn();
        deriv1->SetOrder(1);
        deriv1->Update();

        typename DerivativeImageFilter<ImageType, ImageType>::Pointer
          deriv2 = DerivativeImageFilter<ImageType, ImageType>::New();
        deriv2->SetInput(deriv1->GetOutput());
        deriv2->SetDirection(j);
        deriv2->SetUseImageSpacingOn();
        deriv2->SetOrder(1);
        
        deriv2->Update();

        const auto hess = deriv2->GetOutput();
        LoadVDBHessian(k, hess);
        }
      }


  } // end setimage

  /** Sample the Hessian at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleHessiansVnl returns a vnl
      matrix of size VDimension x VDimension. */
  inline VnlMatrixType SampleHessianVnl(const PointType &p) const
  {
    const auto coord = this->ToVDBCoord(p);

    VnlMatrixType vdbAns;
    for (unsigned int i = 0; i < DIMENSION; i++)
    {
      vdbAns[i][i] = openvdb::tools::BoxSampler::sample(m_VDBHessians[i]->tree(), coord);
    }

    // Cross derivatives
    unsigned int k = DIMENSION;
    for (unsigned int i =0; i < DIMENSION; i++)
    {
      for (unsigned int j = i+1; j < DIMENSION; j++, k++)
      {
        vdbAns[i][j] = vdbAns[j][i] = openvdb::tools::BoxSampler::sample(m_VDBHessians[k]->tree(), coord);
      }
    }
    return vdbAns;
  }


  inline vnl_matrix_fixed<float, DIMENSION, DIMENSION> SampleHessianAtPoint(const PointType &p) const override {
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
    for (unsigned int i = 0; i < DIMENSION + ((DIMENSION * DIMENSION) - DIMENSION) / 2; i++) {
      m_VDBHessians[i] = 0;
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
  // Partials are stored:
  //     0: dxx  3: dxy  4: dxz
  //             1: dyy  5: dyz
  //                     2: dzz
  typename openvdb::FloatGrid::Ptr m_VDBHessians[
          DIMENSION + ((DIMENSION * DIMENSION) - DIMENSION) / 2];
};

} // end namespace itk
