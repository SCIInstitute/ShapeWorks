/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImageDomain.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImageDomain_h
#define __itkParticleImageDomain_h

#include "itkImage.h"
#include "itkParticleClipRegionDomain.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk
{
/** \class ParticleImageDomain
 *  A bounding-box region domain that sets its bounding box according to the
 *  origin, spacing, and RequestedRegion of a specified itk::Image.  This
 *  Domain object may be sampled for interpolated image values using the
 *  Sample(Point) method.
 *
 * \sa ParticleImageDomainWithGradients
 * \sa ParticleRegionDomain
 *
 */
template <class T, unsigned int VDimension=3>
class ITK_EXPORT ParticleImageDomain : public ParticleClipRegionDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomain Self;
  typedef ParticleRegionDomain<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Type of the ITK image used by this class. */
  typedef Image<T, VDimension> ImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleImageDomain, ParticleRegionDomain);

  /** Point type of the domain (not the image). */
  typedef typename Superclass::PointType PointType;

  typedef LinearInterpolateImageFunction<ImageType, typename PointType::CoordRepType>
  ScalarInterpolatorType;



  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    this->Modified();
    m_Image= I;

    // Set up the scalar image and interpolation.
    m_ScalarInterpolator->SetInputImage(m_Image);

    // Grab the upper-left and lower-right corners of the bounding box.  Points
    // are always in physical coordinates, not image index coordinates.
    typename ImageType::RegionType::IndexType idx
      = m_Image->GetRequestedRegion().GetIndex(); // upper lh corner
    typename ImageType::RegionType::SizeType sz
      = m_Image->GetRequestedRegion().GetSize();  // upper lh corner

    typename ImageType::PointType l0;
    m_Image->TransformIndexToPhysicalPoint(idx, l0);
    typename ImageType::PointType u0;
    m_Image->TransformIndexToPhysicalPoint(idx + sz, u0);

    // Cast points to higher precision if needed.  Parent class uses doubles
    // because they are compared directly with points in the particle system,
    // which are always double precision.
    typename Superclass::PointType l;
    typename Superclass::PointType u;
    
    for (unsigned int i = 0; i < VDimension; i++)
      {
      l[i] = static_cast<double>(l0[i]);
      u[i] = static_cast<double>(u0[i]);
      }
    
    this->SetLowerBound(l);
    this->SetUpperBound(u);
  }
  itkGetObjectMacro(Image, ImageType);
  itkGetConstObjectMacro(Image, ImageType);

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer. */
  inline T Sample(const PointType &p) const
  {   return  m_ScalarInterpolator->Evaluate(p);  }

  /** Check whether the point p may be sampled in this image domain. */
  inline bool IsInsideBuffer(const PointType &p) const
  { return m_ScalarInterpolator->IsInsideBuffer(p); }

  /** Allow public access to the scalar interpolator. */
  itkGetObjectMacro(ScalarInterpolator, ScalarInterpolatorType);
  
protected:
  ParticleImageDomain()
  {
    m_ScalarInterpolator = ScalarInterpolatorType::New();
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    
    os << indent << "m_Image = " << m_Image << std::endl;
    os << indent << "m_ScalarInterpolator = " << m_ScalarInterpolator << std::endl;
  }
  virtual ~ParticleImageDomain() {};
  
private:
  ParticleImageDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename ImageType::Pointer m_Image;
  typename ScalarInterpolatorType::Pointer m_ScalarInterpolator;
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleImageDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleImageDomain.txx"
#endif

#endif
