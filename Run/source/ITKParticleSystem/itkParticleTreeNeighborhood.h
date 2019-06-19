/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleTreeNeighborhood.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleTreeNeighborhood_h
#define __itkParticleTreeNeighborhood_h

#include "itkParticleNeighborhood.h"

namespace itk
{
/** \class ParticleTreeNeighborhood
 *
 * This class implements a ParticleNeighborhood tree that caches point values
 * in a power-of-two tree, e.g. a quad-tree in 2D, octree in 3D, etc.  Space is
 * partitioned in bounding boxes that decrease in size by powers of 2^D, where
 * D is dimensionality, as one descends into the tree.  FindNeighborhoodPoints
 * simply descends into this tree to find the correct nodes which must be
 * considered for a given point and radius.
 */
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleTreeNeighborhood : public ParticleNeighborhood<VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleTreeNeighborhood Self;
  typedef ParticleNeighborhood Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleTreeNeighborhood, ParticleNeighborhood);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Type of the tree used to store data. */
  itkPowersOfTwoTree<Dimension> TreeType;
  
  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Domain type.  The Domain object provides bounds and distance
      information. */
  typedef ParticleDomain<VDimension> DomainType;
  
  /** Container type for points.  This matches the itkParticleSystem container
      type. */
  typedef ParticleContainer<PointType> PointContainerType;

  /** Point list (vector) type.  This is the type of list returned by FindNeighborhoodPoints. */
  typedef std::vector<PointType> PointVectorType;

  /** Set/Get the point container.  These are the points parsed by the
      Neighborhood class when FindNeighborhoodPoints is called. */
  itkSetMacro(PointContainer, PointContainerType);
  itkGetConstMacro(PointContainer, PointContainerType);

  /** Compile a list of points that are within a specified radius of a given
      point.  The default implementation will throw an exception. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, double)
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
  itkSetMacro(Domain, DomainType);
  itkGetConstMacro(Domain, DomainType);
  
protected:
  ParticleTreeNeighborhood() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    m_PointContainer->PrintSelf(os, indent);
  }
  virtual ~ParticleTreeNeighborhood() {};

private:
  ParticleTreeNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PointContainerType m_PointContainer;
  DomainType m_Domain;
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleTreeNeighborhood+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleTreeNeighborhood.txx"
#endif

#endif
