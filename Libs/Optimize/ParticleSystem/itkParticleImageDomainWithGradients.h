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
class ParticleImageDomainWithGradients : public ParticleImageDomain<T, VDimension>
{
public:
  typedef SmartPointer<ParticleImageDomainWithGradients<T, VDimension>>  Pointer;

  /** Point type of the domain (not necessarily of the image). */
  typedef typename ParticleImageDomain<T, VDimension>::PointType PointType;
  typedef typename ParticleImageDomain<T, VDimension>::ImageType ImageType;
  typedef typename ParticleImageDomain<T, VDimension>::ScalarInterpolatorType ScalarInterpolatorType;

  typedef GradientImageFilter<ImageType> GradientImageFilterType;
  typedef typename GradientImageFilterType::OutputImageType GradientImageType;
  typedef VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType> GradientInterpolatorType;

  typedef FixedArray<T, 3> VectorType;
  typedef vnl_vector_fixed<T, 3> VnlVectorType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    ParticleImageDomain<T, VDimension>::SetImage(I);
 
    // Compute gradient image and set up gradient interpolation.
    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
    filter->SetInput(I);
    filter->SetUseImageSpacingOn();
    filter->Update();
    m_GradientImage = filter->GetOutput();
    
    m_GradientInterpolator->SetInputImage(m_GradientImage);
  }
  virtual GradientImageType* GetGradientImage() {
    return this->m_GradientImage.GetPointer();
  }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl
      vector of length VDimension instead of an itk::CovariantVector
      (itk::FixedArray). */
  inline VectorType SampleGradient(const PointType &p) const
  {
      if(this->IsInsideBuffer(p))
        return  m_GradientInterpolator->Evaluate(p);
      else {
          itkExceptionMacro("Gradient queried for a Point, " << p << ", outside the given image domain." );
         VectorType g(1.0e-5);
          return g;
      }
  }
  inline VnlVectorType SampleGradientVnl(const PointType &p) const
  { 
    return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); 
  }
  inline VnlVectorType SampleNormalVnl(const PointType &p, T epsilon = 1.0e-5) const
  {
    VnlVectorType grad = this->SampleGradientVnl(p);
    return grad.normalize();
  }

  virtual GradientInterpolatorType* GetGradientInterpolator() {
    return this->m_GradientInterpolator.GetPointer();
  }

  /** This method is called by an optimizer after a call to Evaluate and may be
      used to apply any constraints the resulting vector, such as a projection
      to the surface tangent plane. Returns true if the gradient was modified.*/
  vnl_vector_fixed<double, VDimension> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, VDimension> &gradE,
                                      const PointType &pos) const
  {
    const double epsilon = 1.0e-10;
    double dotprod = 0.0;  
    VnlVectorType normal =  this->SampleNormalVnl(pos, epsilon);
    for (unsigned int i = 0; i < VDimension; i++) {   dotprod  += normal[i] * gradE[i]; }
    vnl_vector_fixed<double, VDimension> result;
    for (unsigned int i = 0; i < VDimension; i++) { result[i] = gradE[i] - normal[i] * dotprod; }
    return result;
  }

  /** Used when a domain is fixed. */
  void DeleteImages()
  {
    ParticleImageDomain<T, VDimension>::DeleteImages();
    m_GradientImage = 0;
    m_GradientInterpolator = 0;
  }
  
protected:
  ParticleImageDomainWithGradients()
  {
    m_GradientInterpolator = GradientInterpolatorType::New();
  }
  virtual ~ParticleImageDomainWithGradients() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleImageDomain<T, VDimension>::PrintSelf(os, indent);
    os << indent << "m_GradientImage = " << m_GradientImage << std::endl;
    os << indent << "m_GradientInterpolator = " << m_GradientInterpolator << std::endl;
  }
  
private:
  typename GradientImageType::Pointer m_GradientImage;
  typename GradientInterpolatorType::Pointer m_GradientInterpolator;
};

} // end namespace itk
