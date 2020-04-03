/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImageDomainWithCurvature.h,v $
  Date:      $Date: 2011/03/23 22:40:10 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImageDomainWithCurvature_h
#define __itkParticleImageDomainWithCurvature_h

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
class ITK_EXPORT ParticleImageDomainWithCurvature
  : public ParticleImageDomainWithHessians<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithCurvature Self;
  typedef ParticleImageDomainWithHessians<T, VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::ScalarInterpolatorType ScalarInterpolatorType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;

  openvdb::FloatGrid::Ptr m_VDBCurvature;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleImageDomainWithCurvature, ParticleImageDomainWithHessians);

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

    double sig =  this->GetSpacing()[0] * 0.5;
    
    f->SetVariance(sig);
    f->SetInput(I);
    f->SetUseImageSpacingOn();
    f->Update();

    // NOTE: Running the image through a filter seems to be the only way
    // to get all of the image information correct!  Set the variance to
    // positive value to smooth the curvature calculations.

    m_VDBCurvature = openvdb::FloatGrid::create(1e-6f);
    auto vdbAccessor = m_VDBCurvature->getAccessor();

    itk::ImageRegionIteratorWithIndex<ImageType> it(I, I->GetRequestedRegion());
    itk::ImageRegionIteratorWithIndex<ImageType> curvIt(f->GetOutput(),
                                                    f->GetOutput()->GetRequestedRegion());

    for (; !curvIt.IsAtEnd(); ++curvIt, ++it) {
      const auto idx = curvIt.GetIndex();
      if (idx != it.GetIndex()) {
        throw std::runtime_error("Bad index");
      }
      const auto pixel = it.Get();

      if (abs(pixel) > 4.0) {
        continue;
      }

      typename ImageType::PointType itkPt;
      I->TransformIndexToPhysicalPoint(idx, itkPt);
      const auto coord = openvdb::Coord(itkPt[0], itkPt[1], itkPt[2]);
      vdbAccessor.setValue(coord, this->MeanCurvature(itkPt));
    }

    // Release the memory in the parent hessian images.
    //this->DeletePartialDerivativeImages();
    

  } // end setimage

  double GetCurvature(const PointType &p) const
  {
    const auto coord = openvdb::Vec3R(p[0], p[1], p[2]);
    const T v2 = openvdb::tools::BoxSampler::sample(m_VDBCurvature->tree(), coord);
    return v2;
  }

  /*
  typename ImageType::Pointer *GetCurvatureImage()
  { return m_CurvatureImage; }
  */

protected:
  ParticleImageDomainWithCurvature()
  {
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleImageDomainWithCurvature() {};

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
  ParticleImageDomainWithCurvature(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleImageDomainWithCurvature+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleImageDomainWithCurvature.txx"
#endif

#endif
