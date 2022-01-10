---
title: Libs/Optimize/ParticleSystem/itkParticleSystem.h

---

# Libs/Optimize/ParticleSystem/itkParticleSystem.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)** <br>A facade class managing interactions with a particle system.  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSystem.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSystem_h
#define __itkParticleSystem_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkParticleAttribute.h"
#include "itkParticleDomain.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkParticleContainer.h"
#include "itkParticleEvents.h"
#include "itkCommand.h"
#include "itkEventObject.h"
#include "itkParticleNeighborhood.h"
#include "vnl/vnl_inverse.h"
#include <map>
#include <vector>
#include <random>

namespace itk
{
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleSystem : public DataObject
{
public:
  typedef ParticleSystem Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleSystem, DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef ParticleDomain DomainType;
  
  typedef Point<double, VDimension> PointType;

  typedef ParticleNeighborhood<VDimension> NeighborhoodType;
  
  typedef ParticleContainer<PointType> PointContainerType;

  typedef typename NeighborhoodType::PointVectorType PointVectorType;
  
  //  typedef Transform<double, VDimension, VDimension> TransformType;
  typedef vnl_matrix_fixed<double, VDimension +1, VDimension +1> TransformType;
  typedef vnl_vector_fixed<double, VDimension> VectorType;
  typedef vnl_matrix <double> VnlMatrixType;

  void RegisterAttribute( ParticleAttribute<VDimension> *);


  void SynchronizePositions()
  {
    for (unsigned int d = 0; d < this->GetNumberOfDomains(); d++)
      {
      for (unsigned int p = 0; p < this->GetNumberOfParticles(d); p++)
        {
        this->SetPosition(this->GetPosition(p,d), p, d);
        }
      } 
  }

  unsigned long int GetNumberOfParticles(unsigned int d = 0) const
  { return m_Positions[d]->GetSize(); }
  
  const PointType &AddPosition( const PointType &, unsigned int d=0, int threadId=0);
  const PointType &SetPosition( const PointType &,  unsigned long int k,  unsigned int d=0, int threadId=0);

  //  inline const PointType &SetTransformedPosition(const PointType &p,
  //                                                 unsigned long int k,  unsigned int d=0, int threadId=0)
  //  {
  //    this->SetPosition( this->TransformPoint(p, m_InverseTransform[d]), k, d, threadId );
  //  }

  void RemovePosition(unsigned long int k, unsigned int d=0,  int threadId=0);
  
  PointType &GetPosition(unsigned long int k, unsigned int d=0)
  {    return m_Positions[d]->operator[](k);  }
  const PointType &GetPosition(unsigned long int k, unsigned int d=0) const
  {    return m_Positions[d]->operator[](k);  }
  PointType GetTransformedPosition(unsigned long int k, unsigned int d=0) const
  {    return this->TransformPoint(m_Positions[d]->operator[](k),
                                  m_Transforms[d] * m_PrefixTransforms[d]);  }
  PointType GetPrefixTransformedPosition(unsigned long int k, unsigned int d=0) const
  {    return this->TransformPoint(m_Positions[d]->operator[](k), m_PrefixTransforms[d]); }

  void SplitAllParticles(double epsilon, int threadId=0);
  void SplitParticle(double epsilon, unsigned int idx,  unsigned int d=0, int threadId=0);
  void AdvancedAllParticleSplitting(double epsilon, unsigned int domains_per_shape, unsigned int dom_to_process);
  // Debug function
  void PrintParticleSystem();
  void SplitAllParticlesInDomain(const vnl_vector_fixed<double, VDimension> &, unsigned int d=0, int threadId=0);
  
  void SetNeighborhood(unsigned int,  NeighborhoodType *, int threadId=0);
  void SetNeighborhood(NeighborhoodType *n, int threadId=0)
  { this->SetNeighborhood(0, n, threadId); }
  typename NeighborhoodType::ConstPointer GetNeighborhood(unsigned int k) const
  { return m_Neighborhoods[k]; }

  inline PointVectorType FindNeighborhoodPoints(const PointType &p, int idx,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p, idx, r); }
  inline PointVectorType FindNeighborhoodPoints(const PointType &p, int idx,
                                                std::vector<double> &w,
                                                std::vector<double> &distances,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p,idx,w,distances,r); }
  inline PointVectorType FindNeighborhoodPoints(const PointType &p, int idx,
                                                std::vector<double> &w,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p,idx,w,r); }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(this->GetPosition(idx,d),idx, r); }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx,
                                                std::vector<double> &w,
                                                std::vector<double> &distances,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(this->GetPosition(idx,d),idx,w,distances, r); }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx,
                                                std::vector<double> &w,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(this->GetPosition(idx,d),idx,w,r); }

  //  inline int FindNeighborhoodPoints(const PointType &p,  double r, PointVectorType &vec, unsigned int d = 0) const
  //  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p, r, vec); }
  
  //   PointVectorType FindTransformedNeighborhoodPoints(const PointType &p, double r, unsigned int d = 0) const
  //   {
  //     PointVectorType ans = m_Neighborhoods[d]
  //       ->FindNeighborhoodPoints(this->TransformPoint(p, InverseTransform[d]), r);
  //     for (unsigned int i = 0; i < ans.size(); i++)
  //       {
  //       ans.Point[i] = this->TransformPoint(ans.Point[i], m_Transform[d]);
  //       }
  //     return ans;
  //   }

  void AddDomain( DomainType *, int threadId =0);
  
  typename std::vector< typename DomainType::Pointer >::const_iterator GetDomainsBegin() const
  { return m_Domains.begin(); }

  typename std::vector< typename DomainType::Pointer >::const_iterator GetDomainsEnd() const
  { return m_Domains.end(); }

  DomainType * GetDomain(unsigned int i)
  { return m_Domains[i].GetPointer(); }
  
  DomainType * GetDomain()
  {return m_Domains[0].GetPointer(); }
  
  const DomainType *GetDomain(unsigned int i) const
  { return m_Domains[i].GetPointer(); }
  
  const DomainType *GetDomain() const
  {return m_Domains[0].GetPointer(); }

  unsigned int GetNumberOfDomains() const
  { return m_Domains.size(); }
  
  void SetTransform(unsigned int i, const TransformType &, int threadId=0);
  void SetTransform(const TransformType &p, int threadId=0)
  { this->SetTransform(0, p, threadId); }
  void SetPrefixTransform(unsigned int i, const TransformType &, int threadId=0);
  void SetPrefixTransform(const TransformType &p, int threadId=0)
  { this->SetPrefixTransform(0, p, threadId); }

  typename std::vector< TransformType >::const_iterator
  GetTransformsBegin() const  { return m_Transforms.begin(); }

  typename std::vector< TransformType >::const_iterator
  GetTransformsEnd() const  { return m_Transforms.end(); }
  
  const TransformType &GetTransform(unsigned int i) const
  { return m_Transforms[i]; }
  
  const TransformType &GetTransform() const
  {return m_Transforms[0]; }

  TransformType GetTransform(unsigned int i)
  { return m_Transforms[i]; }
  
  TransformType GetTransform()
  {return m_Transforms[0]; }

  const TransformType &GetPrefixTransform(unsigned int i) const
  { return m_PrefixTransforms[i]; }
  
  const TransformType &GetPrefixTransform() const
  {return m_PrefixTransforms[0]; }

  TransformType GetPrefixTransform(unsigned int i)
  { return m_PrefixTransforms[i]; }
  
  TransformType GetPrefixTransform()
  {return m_PrefixTransforms[0]; }
  
  typename std::vector< TransformType >::const_iterator
  GetInverseTransformsBegin() const
  { return m_InverseTransforms.begin(); }

  typename std::vector< TransformType >::const_iterator
  GetInverseTransformsEnd() const
  { return m_InverseTransforms.end(); }
  
  const TransformType &GetInverseTransform(unsigned int i) const
  { return m_InverseTransforms[i]; }
  
  const TransformType &GetInverseTransform() const
  {return m_InverseTransforms[0]; }

  const TransformType &GetInversePrefixTransform(unsigned int i) const
  { return m_InversePrefixTransforms[i]; }
  
  const TransformType &GetInversePrefixTransform() const
  {return m_InversePrefixTransforms[0]; }

  const  std::vector<typename  PointContainerType::Pointer> &  GetPositions() const
  { return m_Positions; }
  const  typename  PointContainerType::Pointer & GetPositions(unsigned int d) const
  { return m_Positions[d]; }

  void AddPositionList(const std::vector<PointType> &, unsigned int d = 0, int threadId = 0);

  PointType TransformPoint(const PointType &, const TransformType &) const;

  VectorType TransformVector(const VectorType &, const TransformType &) const;

  VnlMatrixType TransformNormalDerivative(const VnlMatrixType &, const TransformType & ) const;

  inline TransformType InvertTransform(const TransformType &T) const
  {
    // Note, vnl_inverse is optimized for small matrices 1x1 - 4x4
    return vnl_inverse(T);
  }

  void FlagDomain(unsigned int i)
  {
    // ensure large enough
    while (i >= this->m_DomainFlags.size()) {
      m_DomainFlags.push_back(false);
    }

    // set the flag
    m_DomainFlags[i] = true;
  }
  void UnflagDomain(unsigned int i)
  { m_DomainFlags[i] = false; }
  bool GetDomainFlag(unsigned int i) const
  {
    if (i >= m_DomainFlags.size()) {
      // not set
      return false;
    }
    return m_DomainFlags[i];
  }
  const std::vector<bool> &GetDomainFlags() const
  { return m_DomainFlags; }
  void SetDomainFlags()
  { for (unsigned int i = 0; i < m_DomainFlags.size(); i++)  { m_DomainFlags[i] = true; }  }
  void ResetDomainFlags()
  { for (unsigned int i = 0; i < m_DomainFlags.size(); i++)  { m_DomainFlags[i] = false; }  }


  void SetFixedParticleFlag(unsigned int d, unsigned int i)
  { m_FixedParticleFlags[d][i] = true; }
  void ResetFixedParticleFlag(unsigned int d, unsigned int i)
  { m_FixedParticleFlags[d][i] = false; }
  bool GetFixedParticleFlag(unsigned int d, unsigned int i) const
  { return m_FixedParticleFlags[d][i]; }
  void ResetFixedParticleFlags()
  {
      for (unsigned d = 0; d < m_FixedParticleFlags.size(); d++)
      {
          for (unsigned int i = 0; i < m_FixedParticleFlags[d].size(); i++)
              m_FixedParticleFlags[d][i] = false;
      }
  }

  void SetDomainsPerShape(unsigned int num)
  {
       m_DomainsPerShape = num;
       m_FixedParticleFlags.resize(m_DomainsPerShape);
  }
  unsigned int GetDomainsPerShape()
  { return m_DomainsPerShape; }

  void SetNumberOfDomains( unsigned int );

protected:
  ParticleSystem();
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~ParticleSystem() {};


  typename std::vector< typename DomainType::Pointer >::iterator GetDomainsBegin()
  { return m_Domains.begin(); }

  typename std::vector< typename DomainType::Pointer >::iterator GetDomainsEnd()
  { return m_Domains.end(); }
  
  typename std::vector< TransformType >::iterator GetTransformsBegin()
  { return m_Transforms.begin(); }

  typename std::vector< TransformType >::iterator GetTransformsEnd()
  { return m_Transforms.end(); }
  
  typename std::vector< TransformType >::iterator
  GetInverseTransformsBegin()
  { return m_InverseTransforms.begin(); }

  typename std::vector< TransformType >::iterator
  GetInverseTransformsEnd()
  { return m_InverseTransforms.end(); }
  
  TransformType &GetInverseTransform(unsigned int i)
  { return m_InverseTransforms[i]; }
  
  TransformType &GetInverseTransform()
  {return m_InverseTransforms[0]; }

  TransformType &GetInversePrefixTransform(unsigned int i)
  { return m_InversePrefixTransforms[i]; }
  
  TransformType &GetInversePrefixTransform()
  {return m_InversePrefixTransforms[0]; }

private:
  ParticleSystem(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::vector<typename  PointContainerType::Pointer>  m_Positions;

  std::vector< typename DomainType::Pointer > m_Domains;

  unsigned int m_DomainsPerShape;

  std::vector< typename NeighborhoodType::Pointer > m_Neighborhoods;
               
  std::vector< TransformType > m_Transforms;

  std::vector< TransformType > m_InverseTransforms;

  std::vector< TransformType > m_PrefixTransforms;

  std::vector< TransformType > m_InversePrefixTransforms;

  std::vector< unsigned long int> m_IndexCounters;

  std::vector<bool> m_DomainFlags;

  std::vector< std::vector<bool> > m_FixedParticleFlags;

  std::mt19937 m_rand{42};
};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleSystem+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleSystem.txx"
#endif

#include "itkParticleSystem.txx"

#endif
```


-------------------------------

Updated on 2022-01-10 at 16:27:27 +0000
