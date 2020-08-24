/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include <fstream>
#include <vtkContourFilter.h>
#include <vtkMassProperties.h>
#include <itkImage.h>
#include <itkParticleRegionDomain.h>
#include <itkImageToVTKImageFilter.h>
#include <itkZeroCrossingImageFilter.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <chrono>

// we have to undef foreach here because both Qt and OpenVDB define foreach
#undef foreach
#ifndef Q_MOC_RUN
#include <openvdb/openvdb.h>
#include <openvdb/tools/SignedFloodFill.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tools/GridOperators.h>
#include <openvdb/math/Math.h>
#include <openvdb/math/Transform.h>
#endif

namespace itk
{
/** \class ParticleImageDomain
 *  A bounding-box region domain that sets its bounding box according to the
 *  origin, spacing, and RequestedRegion of a specified itk::Image.  This
 *  Domain object may be sampled for interpolated image values using the
 *  Sample(Point) method.
 */
template <class T>
class ParticleImageDomain : public ParticleRegionDomain
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleImageDomain>  Pointer;

  /** Type of the ITK image used by this class. */
  typedef Image<T, DIMENSION> ImageType;

  /** Point type of the domain (not the image). */
  typedef typename ParticleRegionDomain::PointType PointType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I, double narrow_band)
  {
    this->m_FixedDomain = false;
    this->Modified();

    openvdb::initialize(); // It is safe to initialize multiple times.

    // Set a large background value, so that we quickly catch particles outside or on the edge the narrow band.
    // (Downside: its more difficult to display the correct location of the point of failure.)
    m_VDBImage = openvdb::FloatGrid::create(1e8);
    m_VDBImage->setGridClass(openvdb::GRID_LEVEL_SET);
    auto vdbAccessor = m_VDBImage->getAccessor();

    // Save properties of the Image needed for the optimizer
    m_Size = I->GetRequestedRegion().GetSize();
    m_Spacing = I->GetSpacing();
    m_Origin = I->GetOrigin();
    m_Index = I->GetRequestedRegion().GetIndex();

    // Transformation from index space to world space
    openvdb::math::Mat4f mat;
    mat.setIdentity();
    mat.postScale(openvdb::Vec3f(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
    mat.postTranslate(openvdb::Vec3f(m_Origin[0], m_Origin[1], m_Origin[2]));
    const auto xform = openvdb::math::Transform::createLinearTransform(mat);
    m_VDBImage->setTransform(xform);

    ImageRegionIterator<ImageType> it(I, I->GetRequestedRegion());
    it.GoToBegin();

    while(!it.IsAtEnd()) {
      const auto idx = it.GetIndex();
      const auto pixel = it.Get();
      if(abs(pixel) > narrow_band) {
        ++it;
        continue;
      }
      const auto coord = openvdb::Coord(idx[0], idx[1], idx[2]);
      vdbAccessor.setValue(coord, pixel);
      ++it;
    }

    typename ImageType::PointType l0;
    I->TransformIndexToPhysicalPoint(m_Index, l0);
    for (unsigned int i = 0; i < DIMENSION; i++)
        m_Index[i] += m_Size[i]-1;

    typename ImageType::PointType u0;
    I->TransformIndexToPhysicalPoint(m_Index, u0);

    // Cast points to higher precision if needed.  Parent class uses doubles
    // because they are compared directly with points in the particle system,
    // which are always double precision.
    PointType l;
    PointType u;
    
    for (unsigned int i = 0; i < DIMENSION; i++)
      {
      l[i] = static_cast<double>(l0[i]);
      u[i] = static_cast<double>(u0[i]);
      }
    
    this->SetLowerBound(l);
    this->SetUpperBound(u);

    // Precompute and save values that are used in parts of the optimizer
    this->SetupImageForCrossingPointUpdate(I); // this->UpdateZeroCrossingPoint(I);
    this->UpdateSurfaceArea(I);
  }

  inline double GetSurfaceArea() const override
  {
    throw std::runtime_error("Surface area is not computed currently.");
    return m_SurfaceArea;
  }

  inline PointType GetOrigin() const {
    return m_Origin;
  }

  inline typename ImageType::SizeType GetSize() const
  {
    return m_Size;
  }

  inline typename ImageType::SpacingType GetSpacing() const
  {
    return m_Spacing;
  }

  inline typename ImageType::RegionType::IndexType GetIndex() const
  {
    return m_Index;
  }

  inline PointType GetZeroCrossingPoint() const override
  {
    return m_ZeroCrossingPoint;
  }

  /** Sample the image at a point.  This method performs bounds checking. */
  inline T Sample(const PointType &p) const
  {
    if(this->IsInsideBuffer(p)) {
      const auto coord = this->ToVDBCoord(p);
      return openvdb::tools::BoxSampler::sample(m_VDBImage->tree(), coord);
    } else {
      itkExceptionMacro("Distance transform queried for a Point, " << p << ", outside the given image domain. Consider increasing the narrow band" );
    }
  }

  inline double GetMaxDimRadius() const override
  {
    double bestRadius = 0;
    double maxdim = 0;
    for (unsigned int i = 0; i < ImageType::ImageDimension; i++)
    {
      if (GetSize()[i] > maxdim)
      {
        maxdim = GetSize()[i];
        bestRadius = maxdim * GetSpacing()[i];
      }
    }
    return bestRadius;
  }

  /** Used when a domain is fixed. */
  void DeleteImages() override
  {
    m_VDBImage = 0;
  }

  // Updates zero crossing points. Raster scans candidate zero crossing points, and finds one that does not violate any constraints.
  void UpdateZeroCrossingPoint() override {
    for (size_t i = 0; i < m_possible_zero_crossings.size(); i++) {
        this->m_ZeroCrossingPoint = m_possible_zero_crossings[i];
        if(!this->GetConstraints()->IsAnyViolated(this->m_ZeroCrossingPoint))
            break;

    }

    if(this->GetConstraints()->IsAnyViolated(this->m_ZeroCrossingPoint)){std::cerr << "A particle initialization violates at least one constraint. Make sure at least one point satisfies all constraints" << std::endl;}
  }

protected:

  openvdb::FloatGrid::Ptr GetVDBImage() const {
    return m_VDBImage;
  }

  ParticleImageDomain() { }
  virtual ~ParticleImageDomain() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleRegionDomain::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBImage->activeVoxelCount() << std::endl;
  }

  inline openvdb::math::Transform::Ptr transform() const {
    return this->m_VDBImage->transformPtr();
  }

  // Converts a coordinate from an ITK Image point in world space to the corresponding
  // coordinate in OpenVDB Index space. Raises an exception if the narrow band is not
  // sufficiently large to sample the point.
  inline openvdb::Vec3R ToVDBCoord(const PointType &p) const {
    const auto worldCoord = openvdb::Vec3R(p[0], p[1], p[2]);
    const auto idxCoord = this->transform()->worldToIndex(worldCoord);

    // Make sure the coordinate is part of the narrow band
    if(m_VDBImage->tree().isValueOff(openvdb::Coord::round(idxCoord))) { // `isValueOff` requires an integer coordinate
      // If multiple threads crash here at the same time, the error message displayed is just "terminate called recursively",
      // which isn't helpful. So we std::cerr the error to make sure its printed to the console.
      std::cerr << "Sampled point outside the narrow band: " << p << std::endl;
      itkExceptionMacro("Attempt to sample at a point outside the narrow band: " << p << ". Consider increasing the narrow band")
    }

    return idxCoord;
  }

private:

  openvdb::FloatGrid::Ptr m_VDBImage;
  typename ImageType::SizeType m_Size;
  typename ImageType::SpacingType m_Spacing;
  PointType m_Origin;
  PointType m_ZeroCrossingPoint;
  typename ImageType::RegionType::IndexType m_Index; // Index defining the corner of the region
  double m_SurfaceArea;
  std::vector<PointType> m_possible_zero_crossings;

  // Computes possible zero crossing points. Later on, one can find the ones that do not violate constraints.
  void SetupImageForCrossingPointUpdate(ImageType *I){
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
          m_possible_zero_crossings.push_back(pos);
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
