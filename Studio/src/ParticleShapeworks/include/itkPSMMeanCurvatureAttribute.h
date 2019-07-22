/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMMeanCurvatureAttribute_h
#define __itkPSMMeanCurvatureAttribute_h

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkPSMContainer.h"
#include "itkPSMContainerArrayAttribute.h"
#include "itkPSMImageDomainWithCurvature.h"
#include "itkPSMParticleSystem.h"

namespace itk
{
/** \class PSMMeanCurvatureAttribute
 *  \brief
 */
template <class TNumericType, unsigned int VDimension>
class ITK_EXPORT PSMMeanCurvatureAttribute
  : public PSMContainerArrayAttribute<TNumericType,VDimension>
{
public:
  /** Standard class typedefs */
  typedef TNumericType NumericType;
  typedef PSMMeanCurvatureAttribute Self;
  typedef PSMContainerArrayAttribute<TNumericType,VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Numeric types. */
  typedef PSMParticleSystem<VDimension> ParticleSystemType; 
  typedef typename ParticleSystemType::PointType PointType;
  typedef  vnl_vector_fixed<TNumericType, VDimension> VnlVectorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMMeanCurvatureAttribute, PSMContainerArrayAttribute);

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
    this->ComputeCurvatureStatistics(ps, event.GetDomainIndex());
  }

  /** */
  inline void ComputeMeanCurvature(const ParticleSystemType *system,
                                   unsigned int idx, unsigned int dom)
  {
    // First we need a pointer to the domain.  Assume we have
    // Curvature information.
    const PSMImageDomainWithCurvature<TNumericType, VDimension> *domain
      = static_cast<const PSMImageDomainWithCurvature<TNumericType, VDimension> *>(
                                                               system->GetDomain(dom) ); 
    //  Get the position and index.
    PointType pos = system->GetPosition(idx, dom);
    this->operator[](dom)->operator[](idx) = domain->GetCurvature(pos);
  }
  
  /** Compute the mean and std deviation of the curvature on the image
      surface. */
  virtual void ComputeCurvatureStatistics(const ParticleSystemType *, unsigned int d);

  double GetMeanCurvature(int d)
  { return m_MeanCurvatureList[d]; }
  double GetCurvatureStandardDeviation(int d)
  { return m_CurvatureStandardDeviationList[d];}
  
protected:
  PSMMeanCurvatureAttribute()
  {
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.DomainAddEvent = true;
  }
  virtual ~PSMMeanCurvatureAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  PSMMeanCurvatureAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<double> m_MeanCurvatureList;
  std::vector<double> m_CurvatureStandardDeviationList;
  
};

} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMMeanCurvatureAttribute.hxx"
#endif

#endif
