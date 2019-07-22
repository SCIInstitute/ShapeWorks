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
#ifndef __itkPSMDomain_h
#define __itkPSMDomain_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk
{
/** \class PSMDomain 
 * 
 *  \brief Base class for defining the domain in which a particle
 *  system exists.
 *
 *  The PSMDomain class is an abstract base class for defining
 *  the domain in which a particle system exists.  It is intended to
 *  be subclassed to fully define the domain characteristics.
 *
 *  A domain defines a distance metric between two points.  By
 *  default, distance is defined in this base class as a Euclidean
 *  distance. PSMDomain subclasses may also define constraints on
 *  particle positions, such as constraints forcing particles to lie
 *  on a surface (submanifold of the domain).  PSMDomain
 *  subclasses may also define boundaries using the BoundingBox
 *  methods.
 * 
 *  \ingroup ParticleShapeModeling
 */
template <unsigned int VDimension>
class ITK_EXPORT PSMDomain : public DataObject
{
public:
  /** Standard class typedefs */
  typedef PSMDomain Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMDomain, DataObject);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  virtual bool ApplyConstraints(PointType &) const
  {  return false; }

  /** A Domain may define a distance calculation.  This is useful in cases
      such as geodesic distance, where distance depends on some information
      contained in the Domain.  The default implementation is Euclidean
      distance. */
  virtual double Distance(const PointType &a, const PointType &b) const
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {      
        sum += (b[i]-a[i]) * (b[i]-a[i]);      
      }
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

 /** Enable/Disable constraints on particle positions imposed by a domain. */
  void DisableConstraints()  { m_ConstraintsEnabled = false; }
  void EnableConstraints()   { m_ConstraintsEnabled = true; }    
  bool GetConstraintsEnabled() const  { return m_ConstraintsEnabled; }
  void SetConstraintsEnabled( bool g )  { m_ConstraintsEnabled = g; }

  
protected:
  PSMDomain();
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~PSMDomain() {}

  bool m_ConstraintsEnabled;
  
private:
  PSMDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMDomain.hxx"
#endif

#endif
