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
#include <itkZeroCrossingImageFilter.h>
#include <vtkContourFilter.h>
#include <vtkMassProperties.h>
#include <itkImageRegionConstIteratorWithIndex.h>

namespace itk
{
/** \class ParticleImageDomain
 *  A bounding-box region domain that sets its bounding box according to the
 *  origin, spacing, and RequestedRegion of a specified itk::Image.  This
 *  Domain object may be sampled for interpolated image values using the
 *  Sample(Point) method.
 */
template <class T, unsigned int VDimension=3>
class ParticleImageDomain : public ParticleRegionDomain<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleImageDomain>  Pointer;

  /** Type of the ITK image used by this class. */
  typedef Image<T, VDimension> ImageType;

  /** Point type of the domain (not the image). */
  typedef typename ParticleRegionDomain<T, VDimension>::PointType PointType;

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

    // Precompute and save values that are used in parts of the optimizer
    this->UpdateZeroCrossingPoint(I);
    this->UpdateSurfaceArea(I);

    m_Size = I->GetRequestedRegion().GetSize();
    m_Spacing = I->GetSpacing();
    m_Origin = I->GetOrigin();
    m_Index = I->GetRequestedRegion().GetIndex();
  }
  virtual ImageType* GetImage() {
    return this->m_Image.GetPointer();
  }
  virtual const ImageType* GetImage() const {
    return this->m_Image.GetPointer();
  }

  inline double GetSurfaceArea() const {
    throw std::runtime_error("Surface area is not computed currently.");
    return m_SurfaceArea;
  }

  inline PointType GetOrigin() const {
    return m_Origin;
  }

  inline typename ImageType::SizeType GetSize() const {
    return m_Size;
  }

  inline typename ImageType::SpacingType GetSpacing() const {
    return m_Spacing;
  }

  inline typename ImageType::RegionType::IndexType GetIndex() const {
    return m_Index;
  }

  inline PointType GetZeroCrossingPoint() const {
    return m_ZeroCrossingPoint;
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
    ParticleRegionDomain<T, VDimension>::PrintSelf(os, indent);
    os << indent << "m_Image = " << m_Image << std::endl;
    os << indent << "m_ScalarInterpolator = " << m_ScalarInterpolator << std::endl;
  }
  
private:
  typename ImageType::Pointer m_Image;
  typename ScalarInterpolatorType::Pointer m_ScalarInterpolator;

  typename ImageType::SizeType m_Size;
  typename ImageType::SpacingType m_Spacing;
  PointType m_Origin;
  PointType m_ZeroCrossingPoint;
  typename ImageType::RegionType::IndexType m_Index;
  double m_SurfaceArea;

  void UpdateZeroCrossingPoint(ImageType *I) {
    typename itk::ZeroCrossingImageFilter < ImageType, ImageType > ::Pointer zc =
            itk::ZeroCrossingImageFilter < ImageType, ImageType > ::New();
    zc->SetInput(I);
    zc->Update();
    typename itk::ImageRegionConstIteratorWithIndex < ImageType > zcIt(zc->GetOutput(),
                                                                       zc->GetOutput()->GetRequestedRegion());

    for (zcIt.GoToReverseBegin(); !zcIt.IsAtReverseEnd(); --zcIt) {
      if (zcIt.Get() == 1.0) {
        PointType pos;
        I->TransformIndexToPhysicalPoint(zcIt.GetIndex(), pos);
        this->m_ZeroCrossingPoint = pos;
        break;
      }
    }
  }

  void UpdateSurfaceArea(ImageType *I) {
    //TODO: This code has been copied from Optimize.cpp. It does not work
    /*
    typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
    itk2vtkConnector = itk::ImageToVTKImageFilter < ImageType > ::New();
    itk2vtkConnector->SetInput(I);
    vtkSmartPointer < vtkContourFilter > ls = vtkSmartPointer < vtkContourFilter > ::New();
    ls->SetInputData(itk2vtkConnector->GetOutput());
    ls->SetValue(0, 0.0);
    ls->Update();
    vtkSmartPointer < vtkMassProperties > mp = vtkSmartPointer < vtkMassProperties > ::New();
    mp->SetInputData(ls->GetOutput());
    mp->Update();
    m_SurfaceArea = mp->GetSurfaceArea();
    */
  }
};

} // end namespace itk
