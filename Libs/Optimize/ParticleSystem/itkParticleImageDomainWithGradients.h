/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImageDomainWithGradients.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImageDomainWithGradients_h
#define __itkParticleImageDomainWithGradients_h

#include <itkImageLinearIteratorWithIndex.h>
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkParticleImageDomain.h"
#include "itkVectorLinearInterpolateImageFunction.h"
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
template <class T, unsigned int VDimension=3>
class ITK_EXPORT ParticleImageDomainWithGradients : public ParticleImageDomain<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithGradients Self;
  typedef ParticleImageDomain<T, VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

    /** Point type of the domain (not necessarily of the image). */
  typedef typename Superclass::PointType PointType;
  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::ScalarInterpolatorType ScalarInterpolatorType;
  typedef GradientImageFilter<ImageType> GradientImageFilterType;
  typedef typename GradientImageFilterType::OutputImageType GradientImageType;
  typedef VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>
  GradientInterpolatorType;

  typedef FixedArray<T, 3> VectorType;
  typedef vnl_vector_fixed<T, 3> VnlVectorType;

  openvdb::VectorGrid::Ptr m_VDBGradient;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleImageDomainWithGradients, ParticleImageDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    Superclass::SetImage(I);

    // Compute gradient image and set up gradient interpolation.
    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
    filter->SetInput(I);
    filter->SetUseImageSpacingOn();
    filter->Update();
    const auto gradI = filter->GetOutput();

#ifdef USE_OPENVDB
    m_VDBGradient = openvdb::VectorGrid::create();
    auto vdbAccessor = m_VDBGradient->getAccessor();

    ImageRegionIterator<GradientImageType> gradIt(gradI, gradI->GetRequestedRegion());
    ImageRegionIterator<ImageType> it(I, I->GetRequestedRegion());
    gradIt.GoToBegin();
    it.GoToBegin();
    while(!gradIt.IsAtEnd()) {
        const auto idx = gradIt.GetIndex();
        if(idx != it.GetIndex()) {
            throw std::runtime_error("Bad index");
        }
        const vnl_vector_ref<float> grad = gradIt.Get().GetVnlVector();
        const auto pixel = it.Get();
        if(abs(pixel) > 3.0) {
            ++gradIt; ++it;
            continue;
        }
        const auto coord = openvdb::Coord(idx[0], idx[1], idx[2]);
        vdbAccessor.setValue(coord, openvdb::Vec3f(grad[0], grad[1], grad[2]));
        ++gradIt; ++it;
    }
#endif

  }
  // itkGetObjectMacro(GradientImage, GradientImageType);

    //TODO: Remove
    unsigned long GetMemUsage() const {
        const auto size = Superclass::GetMemUsage();
#ifdef USE_OPENVDB
        return size + m_VDBGradient->memUsage();
#else
        // return size + m_GradientImage->Capacity() * sizeof(T) * 3;
        return 0;
#endif
    }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl
      vector of length VDimension instead of an itk::CovariantVector
      (itk::FixedArray). */
  inline VectorType SampleGradient(const PointType &p) const
  {

      if(this->IsInsideBuffer(p)) {
#ifdef USE_OPENVDB
          auto o = this->GetOrigin();
          auto sp = p;
          for(int i=0; i<3; i++) { sp[i] -= o[i]; }
          const auto coord = openvdb::Vec3R(sp[0], sp[1], sp[2]);

          const auto _v2 = openvdb::tools::BoxSampler::sample(m_VDBGradient->tree(), coord);
          const VectorType v2(_v2.asPointer());
          return v2;
#else
          const VectorType v1 =  m_GradientInterpolator->Evaluate(p);
          return v1;
#endif
      }
      else {
          itkExceptionMacro("Gradient queried for a Point, " << p << ", outside the given image domain." );
         VectorType g(1.0e-5);
          return g;
      }
  }
  inline VnlVectorType SampleGradientVnl(const PointType &p) const
  { return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); }
  inline VnlVectorType SampleNormalVnl(const PointType &p, T epsilon = 1.0e-5) const
  {
    VnlVectorType grad = this->SampleGradientVnl(p);
    grad = grad.normalize();
//    double q = 1.0 / (grad.magnitude() + epsilon);
//    for (unsigned int i = 0; i < VDimension; i++) { grad[i] *= q;
        /* PRATEEP */
        // reverse normal directions
//        grad[i] *= -1.0;
//    }
    return grad;
  }

  /** Allow public access to the scalar interpolator. */
  // itkGetObjectMacro(GradientInterpolator, GradientInterpolatorType);


  /** This method is called by an optimizer after a call to Evaluate and may be
      used to apply any constraints the resulting vector, such as a projection
      to the surface tangent plane. Returns true if the gradient was modified.*/
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                                      const PointType &pos,
                                      double maxtimestep) const
    
  {
    if (this->m_ConstraintsEnabled == true)
      {
      const double epsilon = 1.0e-10;
      
      double dotprod = 0.0;  
      VnlVectorType normal =  this->SampleNormalVnl(pos, epsilon);
      for (unsigned int i = 0; i < VDimension; i++) {   dotprod  += normal[i] * gradE[i]; }
      for (unsigned int i = 0; i < VDimension; i++) {   gradE[i] -= normal[i] * dotprod; }
     
      return true;
      }
    else return false;
  }

  /** Used when a domain is fixed. */
  void DeleteImages()
  {
    Superclass::DeleteImages();
    m_VDBGradient = 0;
    // m_GradientImage = 0;
    // m_GradientInterpolator = 0;
  }
  
protected:
  ParticleImageDomainWithGradients()
  {
    // m_GradientInterpolator = GradientInterpolatorType::New();
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    // os << indent << "m_GradientImage = " << m_GradientImage << std::endl;
    // os << indent << "m_GradientInterpolator = " << m_GradientInterpolator << std::endl;
  }
  virtual ~ParticleImageDomainWithGradients() {};
  
private:
  ParticleImageDomainWithGradients(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // typename GradientImageType::Pointer m_GradientImage;
  // typename GradientInterpolatorType::Pointer m_GradientInterpolator;
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleImageDomainWithGradients+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleImageDomainWithGradients.txx"
#endif

#endif
