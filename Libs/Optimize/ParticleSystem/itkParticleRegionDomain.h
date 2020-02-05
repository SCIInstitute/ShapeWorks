/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleRegionDomain.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleRegionDomain_h
#define __itkParticleRegionDomain_h

#include "itkDataObject.h"
#include "itkParticleDomain.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkExceptionObject.h"

namespace itk
{
/** \class ParticleRegionDomain
 *  \brief 
 */
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleRegionDomain : public ParticleDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleRegionDomain Self;
  typedef ParticleDomain<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleRegionDomain, ParticleDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface. This function will clip the point to the boundaries. */
  virtual bool ApplyConstraints(PointType &p) const
  {
    bool changed = false;
    for (unsigned int i = 0; i < VDimension; i++)
    {
        if (p[i] < GetLowerBound()[i]) {
            changed = true;
            p[i] = GetLowerBound()[i];
        }
        else if (p[i] > GetUpperBound()[i]) {
            changed = true;
            p[i] = GetUpperBound()[i];
        }
    }
    return changed;
  }

  /** Set the lower/upper bound of the bounded region. */
  itkSetMacro(LowerBound, PointType);
  itkSetMacro(UpperBound, PointType);
  virtual const PointType &GetUpperBound() const { return m_UpperBound; }
  virtual const PointType &GetLowerBound() const { return m_LowerBound; }
  
  /** Specify the lower and upper bounds (1st and 2nd parameters, respectively)
      of the region. */
  void SetRegion(const PointType &lowerBound, const PointType &upperBound)
  {
    SetLowerBound(lowerBound);
    SetUpperBound(upperBound);
  }
  
protected:
  ParticleRegionDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << "LowerBound = " << GetLowerBound() << std::endl;
    os << "UpperBound = " << GetUpperBound() << std::endl;
  }
  virtual ~ParticleRegionDomain() {};
  
private:
  ParticleRegionDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PointType m_LowerBound;
  PointType m_UpperBound;

};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleRegionDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleRegionDomain.txx"
#endif

#endif
