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
#ifndef __itkPSMRegionDomain_h
#define __itkPSMRegionDomain_h

#include "itkDataObject.h"
#include "itkPSMDomain.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkExceptionObject.h"

namespace itk
{
/** \class PSMRegionDomain
 *  \brief 
 */
template <unsigned int VDimension>
class ITK_EXPORT PSMRegionDomain : public PSMDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef PSMRegionDomain Self;
  typedef PSMDomain<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMRegionDomain, PSMDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  This class will throw an exception if the point is outside of
      the region.  To specify other behaviors, create a subclass and override
      this method. */
  virtual bool ApplyConstraints(PointType &p) const
  {
    for (unsigned int i = 0; i < VDimension; i++)
      {
        if ( p[i] < m_LowerBound[i]  || p[i] > m_UpperBound[i])
          {
            itkExceptionMacro(<< "Point " << p << " is outside of the specified Domain,  with bounding box "
                              <<  m_LowerBound << " - " << m_UpperBound
                              << std::endl;);
            
          }
      }
    return false;
  }

  /** Set the lower/upper bound of the bounded region. */
  itkSetMacro(LowerBound, PointType);
  itkSetMacro(UpperBound, PointType);
  virtual const PointType &GetUpperBound() const
  { return m_UpperBound; }
  virtual const PointType &GetLowerBound() const
  { return m_LowerBound; }
  
  /** Specify the lower and upper bounds (1st and 2nd parameters, respectively)
      of the region. */
  void SetRegion(const PointType &l, const PointType &u)
  {
    this->SetLowerBound(l);
    this->SetUpperBound(u);
  }
  
protected:
  PSMRegionDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << "LowerBound = " << m_LowerBound << std::endl;
    os << "UpperBound = " << m_UpperBound << std::endl;
  }
  virtual ~PSMRegionDomain() {};
  
private:
  PSMRegionDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PointType m_LowerBound;
  PointType m_UpperBound;

};

} // end namespace itk

#endif
