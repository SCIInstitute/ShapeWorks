---
title: Libs/Optimize/ParticleSystem/itkParticleGoodBadAssessment.h

---

# Libs/Optimize/ParticleSystem/itkParticleGoodBadAssessment.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleGoodBadAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md)**  |
| struct | **[itk::ParticleGoodBadAssessment::IdxCompare](../Classes/structitk_1_1ParticleGoodBadAssessment_1_1IdxCompare.md)**  |




## Source code

```cpp
#pragma once

#include "itkParticleSystem.h"
#include "ParticleImageDomainWithGradients.h"
#include "ParticleImageDomainWithCurvature.h"
#include <algorithm>
namespace itk
{
template <class TGradientNumericType, unsigned int VDimension>
class ParticleGoodBadAssessment : public Object
{
public:

    typedef ParticleGoodBadAssessment Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef WeakPointer<const Self>  ConstWeakPointer;

    typedef ParticleSystem ParticleSystemType;
    typedef typename ParticleSystemType::PointType PointType;

    typedef shapeworks::ParticleImageDomainWithGradients<TGradientNumericType> DomainType;
    typedef typename DomainType::VnlVectorType NormalType;

    typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> MeanCurvatureCacheType;

    itkNewMacro(Self)

    
    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i; }
    int GetDomainsPerShape() const
    { return m_DomainsPerShape; }

    void SetEpsilon(double i)
    { epsilon = i; }

    void SetCriterionAngle(double a)
    { m_CriterionAngle = a;}

    void SetPerformAssessment(bool b)
    { m_PerformAssessment = b; }

    std::vector<std::vector<int> > RunAssessment(const ParticleSystemType * m_ParticleSystem, MeanCurvatureCacheType * m_MeanCurvatureCache);
    vnl_matrix<double> computeParticlesNormals(int d, const ParticleSystemType * m_ParticleSystem);

    struct IdxCompare
    {
        const std::vector<double>& target;

        IdxCompare(const std::vector<double>& target): target(target) {}

        bool operator()(int a, int b) const { return target[a] < target[b]; }
    };

    std::vector<int> sortIdcs(std::vector<double> v)
    {
        std::vector<int> index(v.size(), 0);
        for (int i = 0; i < v.size(); i++)
            index[i] = i;
        std::sort(index.begin(), index.end(), IdxCompare(v));
        return index;
    }

protected:
  ParticleGoodBadAssessment() : m_DomainsPerShape(1), m_CriterionAngle(90.0), m_PerformAssessment(true) {  }
  virtual ~ParticleGoodBadAssessment() {}

private:
  ParticleGoodBadAssessment(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_DomainsPerShape;
  double m_CriterionAngle;
  bool m_PerformAssessment;
  double epsilon; // m_Spacing from ShapeWorksRunApp


};

} //end namespace


#include "itkParticleGoodBadAssessment.txx"
```


-------------------------------

Updated on 2022-07-23 at 17:50:04 -0600