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
#include "itkVectorLinearInterpolateImageFunction.h"

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

  typedef FixedArray<T, 3> VectorType;
  typedef vnl_vector_fixed<T, 3> VnlVectorType;

  typedef GradientImageFilter<ImageType> GradientImageFilterType;
  typedef typename GradientImageFilterType::OutputImageType GradientImageType;
  typedef VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType> GradientInterpolatorType;

  typename GradientImageType::Pointer tempGrad;
  typename GradientInterpolatorType::Pointer tempGradInterp;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I) {
    ParticleImageDomain<T, VDimension>::SetImage(I);
    m_VDBGradient = openvdb::tools::gradient(*this->GetVDBImage());

    tempGradInterp = GradientInterpolatorType::New();
    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
    filter->SetInput(I);
    filter->SetUseImageSpacingOn();
    filter->Update();
    tempGrad = filter->GetOutput();
    tempGradInterp->SetInputImage(tempGrad);
  }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl
      vector of length VDimension instead of an itk::CovariantVector
      (itk::FixedArray). */
  inline VectorType SampleGradient(const PointType &p) const
  {
    if(this->IsInsideBuffer(p)) {
      const auto coord = this->ToVDBCoord(p);
      // std::cout << "Gradient: " << p << " -> " << coord << std::endl;

      const auto _v = openvdb::tools::BoxSampler::sample(m_VDBGradient->tree(), coord);
      const auto v2 = tempGradInterp->Evaluate(p);
      if(abs(_v[0] - v2[0]) + abs(_v[1] - v2[1]) + abs(_v[2] - v2[2]) > 1e-6f) {
        std::cout << _v << "(vdb) vs " << v2 << "(itk)" << std::endl;
        std::cout << "Sample: " << this->Sample(p) << std::endl;
        std::cout << "^^BAD SAMPLING!" << std::endl;
        // throw std::runtime_error("Gradients: Bad sampling!");
      }

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
  inline VnlVectorType SampleNormalVnl(const PointType &p, T epsilon = 1.0e-5) const
  {
    VnlVectorType grad = this->SampleGradientVnl(p);
    return grad.normalize();
  }

  /** This method is called by an optimizer after a call to Evaluate and may be
      used to apply any constraints the resulting vector, such as a projection
      to the surface tangent plane. Returns true if the gradient was modified.*/
  vnl_vector_fixed<double, VDimension> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, VDimension> &gradE,
                                      const PointType &pos) const override
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
  void DeleteImages() override
  {
    ParticleImageDomain<T, VDimension>::DeleteImages();
    m_VDBGradient = 0;
  }
  
protected:
  ParticleImageDomainWithGradients() {}
  virtual ~ParticleImageDomainWithGradients() {}

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleImageDomain<T, VDimension>::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBGradient->activeVoxelCount() << std::endl;
  }
  
private:
  openvdb::VectorGrid::Ptr m_VDBGradient;
};

} // end namespace itk
