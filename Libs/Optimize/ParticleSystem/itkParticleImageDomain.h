/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkImage.h"
#include "itkParticleRegionDomain.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk
{
/** \class ParticleImageDomain
 *  A bounding-box region domain that sets its bounding box according to the
 *  origin, spacing, and RequestedRegion of a specified itk::Image.  This
 *  Domain object may be sampled for interpolated image values using the
 *  Sample(Point) method.
 */
template <class T, unsigned int VDimension=3>
class ParticleImageDomain : public ParticleRegionDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleImageDomain>  Pointer;

  /** Type of the ITK image used by this class. */
  typedef Image<T, VDimension> ImageType;

  /** Point type of the domain (not the image). */
  typedef typename ParticleRegionDomain<VDimension>::PointType PointType;

  typedef LinearInterpolateImageFunction<ImageType, typename PointType::CoordRepType> ScalarInterpolatorType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    this->Modified();
    m_Image = I;

    // Set up the scalar image and interpolation.
    m_ScalarInterpolator->SetInputImage(m_Image);

    // Grab the upper-left and lower-right corners of the bounding box.  Points
    // are always in physical coordinates, not image index coordinates.
    typename ImageType::RegionType::IndexType idx = m_Image->GetRequestedRegion().GetIndex(); // upper lh corner
    typename ImageType::RegionType::SizeType sz = m_Image->GetRequestedRegion().GetSize();  // upper lh corner

    typename ImageType::PointType l0;
    m_Image->TransformIndexToPhysicalPoint(idx, l0);
    for (unsigned int i = 0; i < VDimension; i++)
        idx[i] += sz[i]-1;

    typename ImageType::PointType u0;
    m_Image->TransformIndexToPhysicalPoint(idx, u0);

    // Cast points to higher precision if needed.  Parent class uses doubles
    // because they are compared directly with points in the particle system,
    // which are always double precision.
    PointType l;
    PointType u;
    
    for (unsigned int i = 0; i < VDimension; i++)
      {
      l[i] = static_cast<double>(l0[i]);
      u[i] = static_cast<double>(u0[i]);
      }
    
    this->SetLowerBound(l);
    this->SetUpperBound(u);
  }
  virtual ImageType* GetImage() {
    return this->m_Image.GetPointer();
  }
  virtual const ImageType* GetImage() const {
    return this->m_Image.GetPointer();
  }

  /** Returns position of first zero-crossing pixel */
  virtual PointType GetPositionOnSurface() const override 
  {
    const ImageType *img = GetImage();

    itk::ZeroCrossingImageFilter < ImageType, ImageType > ::Pointer zc =
      itk::ZeroCrossingImageFilter < ImageType, ImageType > ::New();
    zc->SetInput(img);
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex < ImageType > it(zc->GetOutput(),
      zc->GetOutput()->GetRequestedRegion());

    for (it.GoToReverseBegin(); !it.IsAtReverseEnd(); --it) {
      if (it.Get() == 1.0) {
        PointType pos;
        img->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        return pos;
      }
    }
  }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer. */
  inline T Sample(const PointType &p) const
  {
      if(IsInsideBuffer(p))
        return  m_ScalarInterpolator->Evaluate(p);
      else
        return 0.0;
  }

  /** Check whether the point p may be sampled in this image domain. */
  inline bool IsInsideBuffer(const PointType &p) const
  { return m_ScalarInterpolator->IsInsideBuffer(p); }

  /** Used when a domain is fixed. */
  void DeleteImages()
  {
    m_Image = 0;
    m_ScalarInterpolator = 0;
  }

  /** Allow public access to the scalar interpolator. */
  virtual ScalarInterpolatorType* GetScalarInterpolator() {
    return this->m_ScalarInterpolator.GetPointer();
  }
  
protected:
  ParticleImageDomain()
  {
    m_ScalarInterpolator = ScalarInterpolatorType::New();
  }
  virtual ~ParticleImageDomain() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleRegionDomain<VDimension>::PrintSelf(os, indent);
    os << indent << "m_Image = " << m_Image << std::endl;
    os << indent << "m_ScalarInterpolator = " << m_ScalarInterpolator << std::endl;
  }
  
private:
  typename ImageType::Pointer m_Image;
  typename ScalarInterpolatorType::Pointer m_ScalarInterpolator;
};

} // end namespace itk
