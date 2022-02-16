---
title: Libs/Optimize/ParticleSystem/itkParticleVectorFunction.h

---

# Libs/Optimize/ParticleSystem/itkParticleVectorFunction.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleVectorFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleVectorFunction_h
#define __itkParticleVectorFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

template <unsigned int VDimension>
class ParticleVectorFunction : public LightObject
{
public:
  typedef ParticleVectorFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( ParticleVectorFunction, LightObject);

  typedef ParticleSystem<VDimension> ParticleSystemType;

  typedef vnl_vector_fixed<double, VDimension> VectorType;

  //  itkNewMacro(Self);
  
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep, double &energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const =0;


  virtual void ResetBuffers() { }
  
  virtual void AfterIteration() { }

  virtual void BeforeIteration()  {}

  virtual void BeforeEvaluate(unsigned int , unsigned int, const ParticleSystemType *) {}
  
  virtual void SetParticleSystem( ParticleSystemType *p)
  {    m_ParticleSystem = p;  }
  virtual ParticleSystemType *GetParticleSystem() const
  { return m_ParticleSystem; }
  virtual void SetDomainNumber( unsigned int i)
  {    m_DomainNumber = i;  }
  virtual int GetDomainNumber() const
  { return m_DomainNumber; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    std::cerr << "Error: base class ParticleVectorFunction Clone method called!\n";
    std::cerr << "Threaded run of current parameters not supported!\n";
    return nullptr;
  }

protected:
  ParticleVectorFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~ParticleVectorFunction() {}
  void operator=(const ParticleVectorFunction &);
  ParticleVectorFunction(const ParticleVectorFunction &);

  ParticleSystemType *m_ParticleSystem;
  unsigned int m_DomainNumber;  
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleVectorFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleVectorFunction.txx"
#endif

#endif
```


-------------------------------

Updated on 2022-02-16 at 06:47:28 +0000
