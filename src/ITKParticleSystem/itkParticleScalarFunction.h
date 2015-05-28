/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleScalarFunction.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleScalarFunction_h
#define __itkParticleScalarFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"

namespace itk
{

/**
 * \class ParticleScalarFunction
 *
 * This is the base class defining the API for a function that takes a particle
 * system, domain, and location index as arguments and returns a scalar-valued
 * result. 
 *
 */
template <unsigned int VDimension>
class ParticleScalarFunction : public LightObject
{
public:
 /** Standard class typedefs. */
  typedef ParticleScalarFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( ParticleScalarFunction, LightObject);

  /** Type of particle system. */
  typedef ParticleSystem<VDimension> ParticleSystemType;
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

 /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual double Evaluate(unsigned int, unsigned int, typename ParticleSystemType::Pointer &) const = 0;
protected:
  ParticleScalarFunction() {}
  virtual ~ParticleScalarFunction() {}
  void operator=(const ParticleScalarFunction &);
  ParticleScalarFunction(const ParticleScalarFunction &);
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleScalarFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleScalarFunction.txx"
#endif

#endif


