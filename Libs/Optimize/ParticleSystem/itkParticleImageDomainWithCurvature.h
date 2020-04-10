/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

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
class ParticleImageDomainWithCurvature : public ParticleImageDomainWithHessians<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithHessians<T, VDimension> Superclass;
  typedef SmartPointer<ParticleImageDomainWithCurvature<T, VDimension>>  Pointer;

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    // Computes partial derivatives in parent class
    Superclass::SetImage(I);
    m_VDBCurvature = openvdb::tools::meanCurvature(*this->GetVDBImage());
    this->ComputeSurfaceStatistics(I);
  } // end setimage

  double GetCurvature(const PointType &p) const
  {
    const auto coord = this->ToVDBCoord(p);
    return openvdb::tools::BoxSampler::sample(m_VDBCurvature->tree(), coord);
  }

  inline double GetSurfaceMeanCurvature() const
  {
    return m_SurfaceMeanCurvature;
  }

  inline double GetSurfaceStdDevCurvature() const
  {
    return m_SurfaceStdDevCurvature;
  }

protected:
  ParticleImageDomainWithCurvature() {}
  virtual ~ParticleImageDomainWithCurvature() {}

  void ComputeSurfaceStatistics(ImageType *I)
  {
    // TODO: This computation is copied from itkParticleMeanCurvatureAttribute
    // Since the entire Image is not available after the initial load, its simplest
    // to calculate it now. But it should be a part of itkParticleMeanCurvatureAttribute

    // Loop through a zero crossing image, project all the zero crossing points
    // to the surface, and use those points to comput curvature stats.
    typedef itk::ZeroCrossingImageFilter<ImageType, ImageType> ZeroCrossingImageFilterType;
    typename ZeroCrossingImageFilterType::Pointer zc = ZeroCrossingImageFilterType::New();

    zc->SetInput(I);
    zc->Update();

    itk::ImageRegionConstIteratorWithIndex<ImageType> it(zc->GetOutput(),
                                                         zc->GetOutput()->GetRequestedRegion());
    std::vector<double> datalist;
    m_SurfaceMeanCurvature = 0.0;
    m_SurfaceStdDevCurvature = 0.0;

    for (; !it.IsAtEnd(); ++it) {
      if (it.Get() == 1.0) {
        // Find closest pixel location to surface.
        PointType pos;
        //dynamic_cast<const DomainType
        //*>(system->GetDomain(d))->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        I->TransformIndexToPhysicalPoint(it.GetIndex(), pos);

        // Project point to surface.
        // Make sure constraints are enabled
        //      bool c = domain->GetConstraintsEnabled();

        //      domain->EnableConstraints();
        this->ApplyConstraints(pos);

        //      domain->SetConstraintsEnabled(c);

        // Compute curvature at point.
//      std::cout << "pos : " << pos[0] << ' ' << pos[1] << ' ' << pos[2] << std::endl;
        double mc = this->GetCurvature(pos);
        m_SurfaceMeanCurvature += mc;
        datalist.push_back(mc);
      }
    }
    double n = static_cast<double>(datalist.size());
    m_SurfaceMeanCurvature /= n;

    // Compute std deviation using point list
    for (unsigned int i = 0; i < datalist.size(); i++) {
      m_SurfaceStdDevCurvature += (datalist[i] - m_SurfaceMeanCurvature) * (datalist[i] - m_SurfaceMeanCurvature);
    }
    m_SurfaceStdDevCurvature = sqrt(m_SurfaceStdDevCurvature / (n - 1));
  }

private:
  openvdb::FloatGrid::Ptr m_VDBCurvature;

  // Cache surface statistics
  double m_SurfaceMeanCurvature;
  double m_SurfaceStdDevCurvature;
};

} // end namespace itk
