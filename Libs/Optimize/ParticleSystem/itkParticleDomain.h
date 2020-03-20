/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleDomain.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleDomain_h
#define __itkParticleDomain_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk
{
/** \class ParticleDomain
 *  \brief 
 */
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleDomain : public DataObject
{
public:
  /** Standard class typedefs */
  typedef ParticleDomain Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleDomain, DataObject);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  virtual bool ApplyConstraints(PointType &p) const
  {  return false; }

  /** A Domain may define a distance calculation.  This is useful in cases
      such as geodesic distance, where distance depends on some information
      contained in the Domain.  The default implementation is Euclidean
      distance. */
  virtual double Distance(const PointType &a, const PointType &b) const
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {      sum += (b[i]-a[i]) * (b[i]-a[i]);      }
    return sqrt(sum);
  }
  
  /** A Domain may optionally return a bounding box.  The lower bound method
      gives the upper-left-hand corner of the domain.  The upper bound method
      gives the lower-right-hand-corner of the domain.  If a domain does not
      define boundaries, these methods will throw an exceptions  */
  virtual const PointType &GetLowerBound() const
  {
    itkExceptionMacro("This Domain does not define a lower bound.");
  }
  virtual const PointType &GetUpperBound() const
  {
    itkExceptionMacro("This Domain does not define an upper bound.");
  }

  virtual unsigned long GetMemUsage() const {
      itkExceptionMacro("GetMemUsage() not implemented!!");
  }

 /** Enable/Disable constraints on particle positions imposed by a domain. */
  void DisableConstraints()  { m_ConstraintsEnabled = false; }
  void EnableConstraints()   { m_ConstraintsEnabled = true; }    
  bool GetConstraintsEnabled() const  { return m_ConstraintsEnabled; }
  void SetConstraintsEnabled( bool g )  { m_ConstraintsEnabled = g; }

  
protected:
  ParticleDomain();
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~ParticleDomain() {}

  bool m_ConstraintsEnabled;
  
private:
  ParticleDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleDomain.txx"
#endif

#include "itkParticleDomain.txx"

#endif
