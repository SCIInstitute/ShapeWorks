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
#ifndef __itkPSMNeighborhood_h
#define __itkPSMNeighborhood_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkPSMDomain.h"
#include "itkPSMContainer.h"
#include "itkPSMPointIndexPair.h"
#include <vector>

namespace itk
{
/** \class PSMNeighborhood
 *
 * A PSMNeighborhood is responsible for computing neighborhoods of
 * particles.  Given a point position in a domain, and a neighborhood radius,
 * the PSMNeighborhood returns a list of points that are neighbors of that
 * point.  The base class, PSMNeighborhood, must be subclassed to provide
 * functionality; the base class will throw an exception when
 * FindNeighborhoodPoints is called.
 *
 * \ingroup PSM
 *
 */
template <unsigned int VDimension>
class ITK_EXPORT PSMNeighborhood : public DataObject
{
public:
  /** Standard class typedefs */
  typedef PSMNeighborhood Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMNeighborhood, DataObject);
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);
  
  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;
  
  /** Domain type.  The Domain object provides bounds and distance
      information. */
  typedef PSMDomain<VDimension> DomainType;
  
  /** Container type for points.  This matches the itkPSMParticleSystem container
      type. */
  typedef PSMContainer<PointType> PointContainerType;
  
  /** Point list (vector) type.  This is the type of list returned by
      FindNeighborhoodPoints. */
  typedef std::vector<PSMPointIndexPair<VDimension> > PointVectorType;
  
  /** Set/Get the point container.  These are the points parsed by the
      Neighborhood class when FindNeighborhoodPoints is called. */
  itkSetObjectMacro(PointContainer, PointContainerType);
  itkGetConstObjectMacro(PointContainer, PointContainerType);
  
  /** Compile a list of points that are within a specified radius of a
      given point.  The default implementation will throw an
      exception. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, double) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  /** This method finds neighborhood points as in the previous method,
      but also computes a vector of weights associated with each of
      those points. */
  virtual PointVectorType FindNeighborhoodPointsWithWeights(const PointType &, std::vector<double> &,
                                                 double) const
  {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
//  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const
//  {
//    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
//    return 0;
//  }
  
  /** Set the Domain that this neighborhood will use.  The Domain
      object is important because it defines bounds and distance
      measures. */
  itkSetObjectMacro(Domain, DomainType);
  itkGetConstObjectMacro(Domain, DomainType);
  
  /**  For efficiency, itkNeighborhoods are not necessarily observers
       of itkPSMParticleSystem, but have specific methods invoked for
       various events.  AddPosition is called by itkPSMParticleSystem
       when a particle location is added.  SetPosition is called when
       a particle location is set.  RemovePosition is called when a
       particle location is removed.*/
  virtual void AddPosition(const PointType &, unsigned int, int ) {}
  virtual void SetPosition(const PointType &, unsigned int, int) {}
  virtual void RemovePosition(unsigned int, int) {}
  
protected:
  PSMNeighborhood() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~PSMNeighborhood() {};
  
private:
  PSMNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename PointContainerType::Pointer m_PointContainer;
  typename DomainType::Pointer m_Domain;
};

} // end namespace itk

#endif
