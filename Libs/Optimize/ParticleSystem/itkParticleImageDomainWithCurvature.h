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
    m_VDBCurvature = openvdb::tools::meanCurvature(*this->GetVDBImage());
  } // end setimage

  double GetCurvature(const PointType &p) const
  {
    auto o = this->GetOrigin();
    auto sp = p;
    for(int i=0; i<3; i++) { sp[i] -= o[i]; }
    const auto coord = openvdb::Vec3R(sp[0], sp[1], sp[2]);
    const T v2 = openvdb::tools::BoxSampler::sample(m_VDBCurvature->tree(), coord);
    return v2;
  }

protected:
  ParticleImageDomainWithCurvature()
  {
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleImageDomainWithCurvature() {};

  openvdb::FloatGrid::Ptr m_VDBCurvature;

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
