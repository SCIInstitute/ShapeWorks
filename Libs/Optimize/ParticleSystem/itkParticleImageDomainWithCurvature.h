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

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I, double narrow_band)
  {
    // Computes partial derivatives in parent class
    Superclass::SetImage(I, narrow_band);

    // Because of this filter, the proportion of the image we are considering for the narrow band
    // is extended. The curvature is computed outside the narrow band of the original image, where
    // we don't have the gradient stored. At these points, there is incorrect curvature. This is
    // not a problem in practice since we don't have particles far away from the surface.
    typename DiscreteGaussianImageFilter<ImageType, ImageType>::Pointer f = DiscreteGaussianImageFilter<ImageType, ImageType>::New();

    double sig =  this->GetSpacing()[0] * 0.5;
    
    f->SetVariance(sig);
    f->SetInput(I);
    f->SetUseImageSpacingOn();
    f->Update();

    m_VDBCurvature = openvdb::FloatGrid::create();
    auto vdbAccessor = m_VDBCurvature->getAccessor();

    itk::ImageRegionIteratorWithIndex<ImageType> it(I, I->GetRequestedRegion());
    itk::ImageRegionIteratorWithIndex<ImageType> curvIt(f->GetOutput(),
                                                    f->GetOutput()->GetRequestedRegion());

    for (; !curvIt.IsAtEnd(); ++curvIt, ++it) {
      const auto idx = curvIt.GetIndex();
      if (idx != it.GetIndex()) {
        // This code assumes that the two iterators proceed with the same index
        throw std::runtime_error("Bad index");
      }
      const auto pixel = it.Get();

      if (abs(pixel) > narrow_band) {
        continue;
      }

      PointType pos;
      I->TransformIndexToPhysicalPoint(idx, pos);

      const auto coord = openvdb::Coord(idx[0], idx[1], idx[2]);
      vdbAccessor.setValue(coord, this->MeanCurvature(pos));
    }

    this->ComputeSurfaceStatistics(I);
  } // end setimage

  double GetCurvature(const PointType &p) const override
  {
    if (this->m_FixedDomain) {
      return 0;
    }
    const auto coord = this->ToVDBCoord(p);
    return openvdb::tools::BoxSampler::sample(m_VDBCurvature->tree(), coord);
  }

  inline double GetSurfaceMeanCurvature() const override
  {
    return m_SurfaceMeanCurvature;
  }

  inline double GetSurfaceStdDevCurvature() const override
  {
    return m_SurfaceStdDevCurvature;
  }

protected:
  ParticleImageDomainWithCurvature() {}

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBCurvature->activeVoxelCount() << std::endl;
  }
  virtual ~ParticleImageDomainWithCurvature() {};

  double MeanCurvature(const PointType& pos)
  {
    // See Kindlmann paper "Curvature-Based Transfer Functions for Direct Volume
    // Rendering..." for detailss
    
    // Get the normal vector associated with this position.
    typename Superclass::VnlVectorType posnormal = this->SampleNormalAtPoint(pos);

    // Sample the Hessian for this point and compute gradient of the normal.
    typename Superclass::VnlMatrixType I;
    I.set_identity();
    
    typename Superclass::VnlMatrixType H = this->SampleHessianVnl(pos);
    typename Superclass::VnlMatrixType P = I - outer_product(posnormal, posnormal);
    typename Superclass::VnlMatrixType G = P.transpose() * H * P;
  
    // Compute the principal curvatures k1 and k2
    double frobnorm = sqrt(fabs(this->vnl_trace(G * G.transpose())) + 1.0e-10);
    double trace = this->vnl_trace(G);
    //    std::cout << "trace = " << trace << std::endl;
    //    std::cout << "G =  " << G << std::endl;
    //        std::cout << "frobnorm = " << frobnorm << std::endl;
    double diff1 = frobnorm * frobnorm *2.0;
    double diff2 = trace * trace;
    double frobnorm2;

    if (frobnorm <= trace) frobnorm2 = 0.0;
	else  frobnorm2 = sqrt((diff1 - diff2 < 0.0) ? 0.0 : diff1 - diff2);
    double k1 = (trace + frobnorm2) * 0.5;
    double k2 = (trace - frobnorm2) * 0.5;
    //        std::cout << "k1 = " << k1 << std::endl;
    //        std::cout << "k2= " << k2 << std::endl;
    // Compute the mean curvature.
    //  double mc = fabs((k1 + k2) / 2.0);
    return sqrt(k1 * k1 + k2 * k2);  
  }

  inline double vnl_trace(const VnlMatrixType &m) const
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {
      sum += m[i][i];
      }
    return sum;
  }
  
private:
  openvdb::FloatGrid::Ptr m_VDBCurvature;

  // Cache surface statistics
  double m_SurfaceMeanCurvature;
  double m_SurfaceStdDevCurvature;

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

};

} // end namespace itk
