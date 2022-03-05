---
title: Libs/Optimize/ParticleSystem/itkParticleImageDomainWithCurvature.h

---

# Libs/Optimize/ParticleSystem/itkParticleImageDomainWithCurvature.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)**  |




## Source code

```cpp
/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleImageDomainWithGradN.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkDiscreteGaussianImageFilter.h"

namespace itk
{
template <class T>
class ParticleImageDomainWithCurvature : public ParticleImageDomainWithGradN<T>
{
public:
  typedef ParticleImageDomainWithGradN<T> Superclass;

  typedef typename Superclass::PointType PointType;  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::VnlMatrixType VnlMatrixType;

  void SetImage(ImageType *I, double narrow_band)
  {
    // Computes partial derivatives in parent class
    Superclass::SetImage(I, narrow_band);
    m_VDBCurvature = openvdb::tools::meanCurvature(*this->GetVDBImage());
    this->ComputeSurfaceStatistics(I);
  }

  double GetCurvature(const PointType &p, int idx) const override
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
        double mc = this->GetCurvature(pos, -1);
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
```


-------------------------------

Updated on 2022-03-05 at 23:20:35 +0000
