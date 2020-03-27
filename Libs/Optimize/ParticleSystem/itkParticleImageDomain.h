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
#include <chrono>
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

// we have to undef foreach here because both Qt and OpenVDB define foreach
#undef foreach
#ifndef Q_MOC_RUN
#include "openvdb/openvdb.h"
#include "openvdb/tools/SignedFloodFill.h"
#include "openvdb/tools/Interpolation.h"
#endif

#include "H5Cpp.h"

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

  H5::DataSet m_H5_dataset;
  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    m_Image = I;
    m_ScalarInterpolator = ScalarInterpolatorType::New();
    m_ScalarInterpolator->SetInputImage(m_Image);

    std::string h5_filename = std::tmpnam(nullptr);
    h5_filename += ".h5";
    std::string h5_datasetname = "DT";
    H5::H5File file( h5_filename,H5F_ACC_TRUNC );

    m_Size = I->GetRequestedRegion().GetSize();
    m_Spacing = I->GetSpacing();

    hsize_t h5_dims[3];
    h5_dims[0] = m_Size[0];
    h5_dims[1] = m_Size[1];
    h5_dims[2] = m_Size[2];
    H5::DataSpace dataspace(3, h5_dims);
    // H5::FloatType datatype( H5::PredType::NATIVE_FLOAT);
    // datatype.setOrder( H5T_ORDER_LE );
    m_H5_dataset = file.createDataSet(h5_datasetname, H5::PredType::NATIVE_FLOAT, dataspace );
    m_H5_dataset.write(I->GetBufferPointer(), H5::PredType::NATIVE_FLOAT);

    /*
    std::vector<float> imagedata;
    imagedata.reserve(m_Size[0] * m_Size[1] * m_Size[2]);
    ImageRegionIterator<ImageType> it(I, I->GetRequestedRegion());
    it.GoToBegin();
    while(!it.IsAtEnd()) {
      const auto idx = it.GetIndex();
      const auto pixel = it.Get();

      const int i = idx[0] + idx[1] * m_Size[0] + idx[2] * m_Size[0] * m_Size[1];
      imagedata[i] = pixel;

      ++it;
    }
    m_H5_dataset.write(imagedata.data(), H5::PredType::NATIVE_FLOAT);
    */




    std::cout << "Initialized OpenVDB" << std::endl;
    m_VDBImage = openvdb::FloatGrid::create(500000.0);
    m_VDBImage->setGridClass(openvdb::GRID_LEVEL_SET);
    auto vdbAccessor = m_VDBImage->getAccessor();
    ImageRegionIterator<ImageType> it(I, I->GetRequestedRegion());
    it.GoToBegin();
    while(!it.IsAtEnd()) {
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

    benchmark(I->GetBufferPointer());
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
    typename ImageType::Pointer m_Image;
    typename ScalarInterpolatorType::Pointer m_ScalarInterpolator;

  void benchmark(const float *imgPtr) {
    float nx = 270;
    float ny = 270;
    float nz = 270;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i<1000000; i++) {
      PointType p;
      p[0] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/nx)) - nx / 2;
      p[1] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/ny)) - ny / 2;
      p[2] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/nz)) - nz / 2;

      Sample(p);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Took " << duration.count() << "us" << std::endl;

    /*
    for(int i=0; i<5; i++) {
      for(int j=0; j<5; j++) {
        for (int k = 0; k < 5; k++) {
          PointType p;
          p[0] = i; p[1] = j; p[2] = k;

          float v = Sample(p);
          std::cout << v << std::endl;
        }
      }
    }
     */
  }

  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer. */
  inline T Sample(const PointType &p) const
  {
    if(IsInsideBuffer(p)) {
      // PointType p2 = p;
      // for (int i = 0; i < 3; i++) { p2[i] = int(p[i]); }

      /*
      float v0 = m_ScalarInterpolator->Evaluate(p);
      return v0;
       */


      auto o = GetOrigin();
      auto sp = p;
      for (int i = 0; i < 3; i++) { sp[i] -= o[i]; }
      const auto coord = openvdb::Vec3R(sp[0], sp[1], sp[2]);
      const T v2 = openvdb::tools::BoxSampler::sample(m_VDBImage->tree(), coord);
      return v2;

      // auto testo = m_Image->TransformPhysicalPointToIndex(p);

      H5::DataSpace dataspace = m_H5_dataset.getSpace();
      hsize_t      offset[3];   // hyperslab offset in the file
      hsize_t      count[3];    // size of the hyperslab in the file
      offset[0] = int(sp[0]); // Add one????
      offset[1] = int(sp[1]);
      offset[2] = int(sp[2]);
      count[0]  = 3;
      count[1]  = 3;
      count[2]  = 3;
      dataspace.selectHyperslab( H5S_SELECT_SET, count, offset );

      hsize_t     dimsm[3];              /* memory space dimensions */
      dimsm[0] = 3;
      dimsm[1] = 3;
      dimsm[2] = 3 ;
      H5::DataSpace memspace(3, dimsm );
      /*
       * Define memory hyperslab.
       */
      hsize_t      offset_out[3];   // hyperslab offset in memory
      hsize_t      count_out[3];    // size of the hyperslab in memory
      offset_out[0] = 0;
      offset_out[1] = 0;
      offset_out[2] = 0;
      count_out[0]  = 3;
      count_out[1]  = 3;
      count_out[2]  = 3;
      memspace.selectHyperslab( H5S_SELECT_SET, count_out, offset_out );
      float v[27];
      m_H5_dataset.read(v, H5::PredType::NATIVE_FLOAT, memspace, dataspace );

      return v[13];
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
