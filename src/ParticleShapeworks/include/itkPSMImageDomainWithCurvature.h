/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMImageDomainWithCurvature_h
#define __itkPSMImageDomainWithCurvature_h

#include "itkPSMImageDomainWithHessians.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkDiscreteGaussianImageFilter.h"

namespace itk
{
/** \class PSMImageDomainWithCurvature
 *
 * An image domain that extends PSMImageDomainWithHessianGradients with Hessian
 * information.  Hessian values are interpolated with the SampleCurvature(point)
 * method.  Curvature may be smoothed by specifying a sigma for Gaussian
 * blurring of the image prior to initialization.
 *
 * \sa PSMImageDomain
 * \sa ParticleClipRegionDomain
 * \sa PSMDomain
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT PSMImageDomainWithCurvature
  : public PSMImageDomainWithHessians<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef PSMImageDomainWithCurvature Self;
  typedef PSMImageDomainWithHessians<T, VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::ScalarInterpolatorType ScalarInterpolatorType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMImageDomainWithCurvature, PSMImageDomainWithHessians);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    // Computes partial derivatives in parent class
    Superclass::SetImage(I);

    typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> DiscreteGaussianImageFilterType;
    typename DiscreteGaussianImageFilterType::Pointer f = DiscreteGaussianImageFilterType::New();

    double sig =  this->GetImage()->GetSpacing()[0] * 0.5;
    
    f->SetVariance(sig);
    f->SetInput(this->GetImage());
    f->SetUseImageSpacingOn();
    f->Update();

    // NOTE: Running the image through a filter seems to be the only way
    // to get all of the image information correct!  Set the variance to
    // positive value to smooth the curvature calculations.
    m_CurvatureImage = f->GetOutput();
    
    // Loop through the image and compute all curvature values
    itk::ImageRegionIteratorWithIndex<ImageType> it(f->GetOutput(),
                                                    f->GetOutput()->GetRequestedRegion());
    itk::ImageRegionIterator<ImageType> oit(m_CurvatureImage, f->GetOutput()->GetRequestedRegion());
    
    for (; !it.IsAtEnd(); ++it, ++oit)
      {
      // Only compute in a narrow band.
      if (it.Get() < 4.0 && it.Get() > -4.0)
        {      
         PointType pos;
         this->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
         oit.Set(this->MeanCurvature(pos));
        }
      else oit.Set(1.0e-6);
      }
    
    // Release the memory in the parent hessian images.
    this->DeletePartialDerivativeImages();
    
    m_CurvatureInterpolator->SetInputImage(m_CurvatureImage);
  } // end setimage
  
  double GetCurvature(const PointType &pos) const
  {
    return m_CurvatureInterpolator->Evaluate(pos);
  }
  
  typename ImageType::Pointer *GetCurvatureImage()
  { return m_CurvatureImage; }

protected:
  PSMImageDomainWithCurvature()
  {
    m_CurvatureInterpolator = ScalarInterpolatorType::New();
    m_CurvatureImage = ImageType::New();
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~PSMImageDomainWithCurvature() {};

  double MeanCurvature(const PointType& pos)
  {
    // See Kindlmann paper "Curvature-Based Transfer Functions for Direct Volume
    // Rendering..." for detailss
    
    // Get the normal vector associated with this position.
    //VnlVectorType posnormal = this->SampleNormalVnl(pos, 1.0e-10);
    typename Superclass::VnlVectorType posnormal = this->SampleNormalVnl(pos, 1.0e-6);

    // Sample the Hessian for this point and compute gradient of the normal.
    typename Superclass::VnlMatrixType I;
    I.set_identity();
    
    typename Superclass::VnlMatrixType H = this->SampleHessianVnl(pos);
    typename Superclass::VnlMatrixType P = I - outer_product(posnormal, posnormal);
    typename Superclass::VnlMatrixType G = P.transpose() * H * P;
  
    // Compute the principal curvatures k1 and k2
    double frobnorm = sqrt(fabs(this->vnl_trace(G * G.transpose())) + 1.0e-10);
    double trace = this->vnl_trace(G);
    //    std::cout << "trace = " << trace << std::endl;
    //    std::cout << "G =  " << G << std::endl;
    //        std::cout << "frobnorm = " << frobnorm << std::endl;
    double diff1 = frobnorm * frobnorm *2.0;
    double diff2 = trace * trace;
    double frobnorm2;

    if (frobnorm <= trace) frobnorm2 = 0.0;
	else  frobnorm2 = sqrt((diff1 - diff2 < 0.0) ? 0.0 : diff1 - diff2);
    double k1 = (trace + frobnorm2) * 0.5;
    double k2 = (trace - frobnorm2) * 0.5;
    //        std::cout << "k1 = " << k1 << std::endl;
    //        std::cout << "k2= " << k2 << std::endl;
    // Compute the mean curvature.
    //  double mc = fabs((k1 + k2) / 2.0);
    return sqrt(k1 * k1 + k2 * k2);  
  }

  inline double vnl_trace(const VnlMatrixType &m) const
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {
      sum += m[i][i];
      }
    return sum;
  }
  
private:
  PSMImageDomainWithCurvature(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  // Curvature values are stored in an image
  typename ImageType::Pointer m_CurvatureImage;
  typename ScalarInterpolatorType::Pointer m_CurvatureInterpolator;
};

} // end namespace itk

#endif
