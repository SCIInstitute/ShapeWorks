/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleMeanCurvatureAttribute.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleMeanCurvatureAttribute_h
#define __itkParticleMeanCurvatureAttribute_h

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkParticleContainer.h"
#include "itkParticleContainerArrayAttribute.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleSystem.h"

namespace itk
{
/** \class ParticleMeanCurvatureAttribute
 *  \brief
 */
template <class TNumericType, unsigned int VDimension>
class ITK_EXPORT ParticleMeanCurvatureAttribute
  : public ParticleContainerArrayAttribute<TNumericType,VDimension>
{
public:
  /** Standard class typedefs */
  typedef TNumericType NumericType;
  typedef ParticleMeanCurvatureAttribute Self;
  typedef ParticleContainerArrayAttribute<TNumericType,VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Numeric types. */
  typedef ParticleSystem<VDimension> ParticleSystemType; 
  typedef typename ParticleSystemType::PointType PointType;
  typedef  vnl_vector_fixed<TNumericType, VDimension> VnlVectorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleMeanCurvatureAttribute, ParticleContainerArrayAttribute);

  void SetVerbosity(unsigned int val)
  {
      m_verbosity = val;
  }

  unsigned int GetVerbosity()
  { return m_verbosity; }

  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 
  {
    Superclass::PositionAddEventCallback(o, e);
    const ParticlePositionAddEvent &event = dynamic_cast<const ParticlePositionAddEvent &>(e);
    const ParticleSystemType *ps  = dynamic_cast<const ParticleSystemType *>(o);
    this->ComputeMeanCurvature(ps, event.GetPositionIndex(), event.GetDomainIndex());
  }

  virtual void PositionSetEventCallback(Object *o, const EventObject &e)
  {
    const ParticlePositionSetEvent &event = dynamic_cast<const ParticlePositionSetEvent &>(e);
    const ParticleSystemType *ps= dynamic_cast<const ParticleSystemType *>(o);
    this->ComputeMeanCurvature(ps, event.GetPositionIndex(), event.GetDomainIndex());
  }
  
  virtual void DomainAddEventCallback(Object *o, const EventObject &e)
  {
    Superclass::DomainAddEventCallback(o, e);
    m_MeanCurvatureList.push_back(0.0);
    m_CurvatureStandardDeviationList.push_back(0.0);
    const ParticleDomainAddEvent &event = dynamic_cast<const ParticleDomainAddEvent &>(e);
    const ParticleSystemType *ps= dynamic_cast<const ParticleSystemType *>(o);

    if (!ps->GetDomainFlag(event.GetDomainIndex())) {
      this->ComputeCurvatureStatistics(ps, event.GetDomainIndex());
    }
  }

  /** */
  inline void ComputeMeanCurvature(const ParticleSystemType *system,
                                   unsigned int idx, unsigned int dom)
  {
    //  Get the position and index.
    PointType pos = system->GetPosition(idx, dom);
    this->operator[](dom)->operator[](idx) = system->GetDomain(dom)->GetCurvature(pos);
  }
  
  /** Compute the mean and std deviation of the curvature on the image
      surface. */
  virtual void ComputeCurvatureStatistics(const ParticleSystemType *, unsigned int d);

  double GetMeanCurvature(int d)
  { return m_MeanCurvatureList[d]; }
  double GetCurvatureStandardDeviation(int d)
  { return m_CurvatureStandardDeviationList[d];}
  
protected:
  ParticleMeanCurvatureAttribute()
  {
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.DomainAddEvent = true;
  }
  virtual ~ParticleMeanCurvatureAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  ParticleMeanCurvatureAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<double> m_MeanCurvatureList;
  std::vector<double> m_CurvatureStandardDeviationList;
  unsigned int m_verbosity;
};

} // end namespace


#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleMeanCurvatureAttribute+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleMeanCurvatureAttribute.txx"
#endif

#include "itkParticleMeanCurvatureAttribute.txx"

#endif
