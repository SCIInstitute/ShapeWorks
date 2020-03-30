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

#define USE_OPENVDB

#include <fstream>
#include <vtkContourFilter.h>
#include <vtkMassProperties.h>
#include <itkImage.h>
#include <itkParticleRegionDomain.h>
#include <itkLinearInterpolateImageFunction.h>
#include "itkImageToVTKImageFilter.h"
#include <itkZeroCrossingImageFilter.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <chrono>

// we have to undef foreach here because both Qt and OpenVDB define foreach
#undef foreach
#ifndef Q_MOC_RUN
#include "openvdb/openvdb.h"
#include "openvdb/tools/SignedFloodFill.h"
#include "openvdb/tools/Interpolation.h"
#include "openvdb/tools/Composite.h"
#endif

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
class ITK_EXPORT ParticleImageDomain : public ParticleRegionDomain<VDimension>
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

#ifdef USE_OPENVDB
  openvdb::FloatGrid::Ptr m_VDBImage;
#endif

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

  typename ImageType::SizeType m_Size;
  typename ImageType::SpacingType m_Spacing;

  PointType m_Origin;
  PointType m_ZeroCrossingPoint;
  typename ImageType::RegionType::IndexType m_Index;

  //TODO: Move into proper file(statistics?)
  double m_SurfaceArea;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
#ifdef USE_OPENVDB
    openvdb::initialize();
    std::cout << "Initialized OpenVDB" << std::endl;
    m_VDBImage = openvdb::FloatGrid::create(500000.0);
    m_VDBImage->setGridClass(openvdb::GRID_LEVEL_SET);

    m_Size = I->GetRequestedRegion().GetSize();
    m_Spacing = I->GetSpacing();

    auto start = std::chrono::high_resolution_clock::now();

    const auto slices = m_Size[2];
    std::vector<openvdb::FloatGrid::Ptr> vdbGrids(slices);

#pragma omp parallel for schedule(static)
    for(int i=0; i<slices; i++) {
      const auto vdbGrid = openvdb::FloatGrid::create(500000.0);
      vdbGrids[i] = vdbGrid;
      vdbGrid->setGridClass(openvdb::GRID_LEVEL_SET);
      auto vdbAccessor = vdbGrid->getAccessor();

      ImageSliceConstIteratorWithIndex<ImageType> it(I, I->GetRequestedRegion());
      it.SetFirstDirection(0);
      it.SetSecondDirection(1);
      typename ImageType::IndexType firstIdx;
      firstIdx[0] = 0; firstIdx[1] = 0; firstIdx[2] = i;
      it.SetIndex(firstIdx);
      while(!it.IsAtEndOfSlice()) {
        while(!it.IsAtEndOfLine()) {
          const auto idx = it.GetIndex();
          const auto pixel = it.Get();
          if(abs(pixel) > 4.0) {
            ++it;
            continue;
          }
          const auto coord = openvdb::Coord(idx[0], idx[1], idx[2]);
          vdbAccessor.setValue(coord, pixel);
          ++it;
        }
        it.NextLine();
      }
    }

    // Combine the grids
    for(const auto vdbGrid : vdbGrids) {
      // The OpenVDB cookbook uses `csgUnion`, but `merge` is 2x faster
      // openvdb::tools::csgUnion(*m_VDBImage, *vdbGrid);
      m_VDBImage->merge(*vdbGrid);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "VDB Load time: " << duration.count() << "us" << std::endl;

    m_Origin = I->GetOrigin();
    m_Index = I->GetRequestedRegion().GetIndex();

    // Compute surface area
    //TODO: This doesn't work. Also, its probably not used.
    //TODO: Refactor
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


    // Compute zero crossing point
    //TODO: Refactor
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


    // Set up the scalar image and interpolation.
    // m_ScalarInterpolator->SetInputImage(m_Image);

    // Grab the upper-left and lower-right corners of the bounding box.  Points
    // are always in physical coordinates, not image index coordinates.
    typename ImageType::RegionType::IndexType idx
      = I->GetRequestedRegion().GetIndex(); // upper lh corner
    typename ImageType::RegionType::SizeType sz
      = I->GetRequestedRegion().GetSize();  // upper lh corner

    typename ImageType::PointType l0;
    I->TransformIndexToPhysicalPoint(idx, l0);
    for (unsigned int i = 0; i < VDimension; i++)
        idx[i] += sz[i]-1;

    typename ImageType::PointType u0;
    I->TransformIndexToPhysicalPoint(idx, u0);

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
#else
#endif
  }

  inline double GetSurfaceArea() const {
    //TODO: Remove me
    throw std::runtime_error("Surface area: Somebody wants it!");
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
      if(IsInsideBuffer(p)) {
#ifdef USE_OPENVDB
          auto o = GetOrigin();
          auto sp = p;
          for(int i=0; i<3; i++) { sp[i] -= o[i]; }
          const auto coord = openvdb::Vec3R(sp[0], sp[1], sp[2]);
          const T v2 = openvdb::tools::BoxSampler::sample(m_VDBImage->tree(), coord);
          return v2;
#else
          const T v1 =  m_ScalarInterpolator->Evaluate(p);
          return v1;
#endif
      } else {
          return 0.0;
      }
  }

  //TODO: Remove, this is misleading
  unsigned long GetMemUsage() const {
#ifdef USE_OPENVDB
      return m_VDBImage->memUsage();
#else
      return m_Image->Capacity() * sizeof(T);
#endif
  }

  /** Check whether the point p may be sampled in this image domain. */
  inline bool IsInsideBuffer(const PointType &p) const
  {
#ifdef USE_OPENVDB
      //TODO: Hack because we are deleting interpolator and images now...
      return true;
#else
      return m_ScalarInterpolator->IsInsideBuffer(p);
#endif
  }

  /** Used when a domain is fixed. */
  void DeleteImages()
  {
    m_VDBImage = 0;
  }

protected:
  ParticleImageDomain()
  {
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleImageDomain() {};
  
private:
  ParticleImageDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleImageDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleImageDomain.txx"
#endif

#endif
