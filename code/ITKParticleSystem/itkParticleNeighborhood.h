/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleNeighborhood.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleNeighborhood_h

#define __itkParticleNeighborhood_h



#include "itkDataObject.h"

#include "itkPoint.h"

#include "itkWeakPointer.h"

#include "itkParticleDomain.h"

#include "itkParticleContainer.h"

#include "itkParticlePointIndexPair.h"

#include <vector>



namespace itk

{

/** \class ParticleNeighborhood

 *

 * A ParticleNeighborhood is responsible for computing neighborhoods of

 * particles.  Given a point position in a domain, and a neighborhood radius,

 * the ParticleNeighborhood returns a list of points that are neighbors of that

 * point.  The base class, ParticleNeighborhood, must be subclassed to provide

 * functionality; the base class will throw an exception when

 * FindNeighborhoodPoints is called.

 */

template <unsigned int VDimension=3>

class ITK_EXPORT ParticleNeighborhood : public DataObject

{

public:

  /** Standard class typedefs */

  typedef ParticleNeighborhood Self;

  typedef DataObject Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self> ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;



  /** Method for creation through the object factory. */

  itkNewMacro(Self);



  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleNeighborhood, DataObject);



  /** Dimensionality of the domain of the particle system. */

  itkStaticConstMacro(Dimension, unsigned int, VDimension);



  /** Point type used to store particle locations. */

  typedef Point<double, VDimension> PointType;



  /** Domain type.  The Domain object provides bounds and distance

      information. */

  typedef ParticleDomain<VDimension> DomainType;

  

  /** Container type for points.  This matches the itkParticleSystem container

      type. */

  typedef ParticleContainer<PointType> PointContainerType;



  /** Point list (vector) type.  This is the type of list returned by FindNeighborhoodPoints. */

  typedef std::vector<ParticlePointIndexPair<VDimension> > PointVectorType;



  /** Set/Get the point container.  These are the points parsed by the

      Neighborhood class when FindNeighborhoodPoints is called. */

  itkSetObjectMacro(PointContainer, PointContainerType);

  itkGetConstObjectMacro(PointContainer, PointContainerType);



  /** Compile a list of points that are within a specified radius of a given

      point.  The default implementation will throw an exception. */

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, double) const

  {

    itkExceptionMacro("No algorithm for finding neighbors has been specified.");

  }

  /** This method finds neighborhood points as in the previous method, but also

      computes a vector of weights associated with each of those points. */

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, std::vector<double> &,

                                                 double) const

  {

    itkExceptionMacro("No algorithm for finding neighbors has been specified.");

  }

  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const

  {

    itkExceptionMacro("No algorithm for finding neighbors has been specified.");

    return 0;

  }



  /** Set the Domain that this neighborhood will use.  The Domain object is

      important because it defines bounds and distance measures. */

  itkSetObjectMacro(Domain, DomainType);

  itkGetConstObjectMacro(Domain, DomainType);



  /**  For efficiency, itkNeighborhoods are not necessarily observers of

      itkParticleSystem, but have specific methods invoked for various events.

      AddPosition is called by itkParticleSystem when a particle location is

      added.  SetPosition is called when a particle location is set.

      RemovePosition is called when a particle location is removed.*/

  virtual void AddPosition(const PointType &p, unsigned int idx, int threadId = 0) {}

  virtual void SetPosition(const PointType &p, unsigned int idx, int threadId = 0) {}

  virtual void RemovePosition(unsigned int idx, int threadId = 0) {}

  

protected:

  ParticleNeighborhood() {}

  void PrintSelf(std::ostream& os, Indent indent) const

  {

    Superclass::PrintSelf(os, indent);

  }

  virtual ~ParticleNeighborhood() {};



private:

  ParticleNeighborhood(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented



  typename PointContainerType::Pointer m_PointContainer;

  typename DomainType::Pointer m_Domain;

};



} // end namespace itk





#if ITK_TEMPLATE_EXPLICIT

//# include "Templates/itkParticleNeighborhood+-.h"

#endif



#if ITK_TEMPLATE_TXX

//# include "itkParticleNeighborhood.txx"

#endif



#endif

