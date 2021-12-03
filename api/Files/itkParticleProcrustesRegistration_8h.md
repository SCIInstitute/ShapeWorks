---
title: Libs/Optimize/ParticleSystem/itkParticleProcrustesRegistration.h

---

# Libs/Optimize/ParticleSystem/itkParticleProcrustesRegistration.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md)** <br>Class for replacing bad particles during optimization Author - Praful Agrawal Date - June 05, 2017.  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleProcrustesRegistration.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleProcrustesRegistration_h
#define __itkParticleProcrustesRegistration_h

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"
#include "itkParticleSystem.h"

namespace itk
{
template <unsigned int VDimension>
class ITK_EXPORT ParticleProcrustesRegistration: public DataObject
{
public:
  //  typedef TDataType DataType;
  typedef ParticleProcrustesRegistration Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef ParticleSystem<VDimension> ParticleSystemType;
  typedef typename ParticleSystemType::PointType PointType;
  
  itkNewMacro(Self);
  
  itkTypeMacro(ParticleProcrustesRegistration, DataObject);

  void SetParticleSystem(ParticleSystemType *p)
  { m_ParticleSystem = p;  }
  const ParticleSystemType *GetParticleSystem() const
  { return m_ParticleSystem; }
  ParticleSystemType *GetParticleSystem()
  { return m_ParticleSystem; }

  void IterationCallback(itk::Object *, const itk::EventObject &)
  {
    this->RunRegistration();
  }

  void RunRegistration(int i);
  void RunRegistration()
  {
    for (int i =0; i < m_DomainsPerShape; i++)
      {
      this->RunRegistration(i);
      }
  }

  void SetDomainsPerShape(int i)
  { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return m_DomainsPerShape; }

  bool GetScaling() const
  { return m_Scaling; }
  void ScalingOn()
  { m_Scaling = true; }
  void ScalingOff()
  { m_Scaling = false; }
  
  bool GetRotationTranslation() const
  { return m_RotationTranslation; }
  void RotationTranslationOn()
  { m_RotationTranslation = true; }
  void RotationTranslationOff()
  { m_RotationTranslation = false; }
  
  void SetFixedScales( const std::vector<double> v)
  { m_FixedScales = v; }

protected:
  ParticleProcrustesRegistration() : m_DomainsPerShape(1), m_Scaling(true), m_RotationTranslation(true) {  }
  virtual ~ParticleProcrustesRegistration() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  ParticleProcrustesRegistration(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<double> m_FixedScales;
  
  int m_DomainsPerShape;
  bool m_Scaling;
  bool m_RotationTranslation;
  bool m_ComputeTransformation;
  ParticleSystemType *m_ParticleSystem;
};

} // end namespace

#endif
```


-------------------------------

Updated on 2021-12-03 at 02:18:11 +0000
