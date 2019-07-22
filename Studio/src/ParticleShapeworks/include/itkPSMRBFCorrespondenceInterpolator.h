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
#ifndef __itkPSMRBFCorrespondenceInterpolator_h
#define __itkPSMRBFCorrespondenceInterpolator_h

#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

#include "itkFunctionBase.h"
#include "itkPoint.h"
#include "itkMacro.h"

namespace itk
{

/** \class PSMRBFCorrespondenceInterpolator
 * \brief 
 *
 *
 * \ingroup 
 */
template <unsigned int VDimension>
class ITK_EXPORT PSMRBFCorrespondenceInterpolator :
  public FunctionBase<Point<double,VDimension>, Point<double,VDimension> >
{
public:
  /** Dimensionality of the points. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Standard class typedefs. */
  typedef PSMRBFCorrespondenceInterpolator    Self;
  typedef Point<double,VDimension>            PointType;
  typedef FunctionBase <PointType, PointType> Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Method to create through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( PSMRBFCorrespondenceInterpolator, FunctionBase );

  /** Set/Get the first point set "A".  This is the point set that
      defines the domain from which points will be mapped by the
      function. */
  void SetPointSetA(const std::vector<PointType> &v)
    {      
      m_PointSetA = v;
    }
  const std::vector<PointType> & GetPointSetA() const
  {
    return m_PointSetA;
  }

  /** Set/Get the second point set "B".  This is the point set surface
      that defines the domain into which points will be mapped by the
      function. */
  void SetPointSetB(const std::vector<PointType> &v)
    {      
      m_PointSetB = v;
    }
  const std::vector<PointType> & GetPointSetB() const
  {
    return m_PointSetB;
  }

  /** Compute the interpolation function.*/
  void Initialize();

  /** Returns true if the interpolation function has been computed.*/
  itkGetMacro(Initialized, bool);

  /** Map a point from the surface domain defined by PointSetA to the
      domain defined by PointSetB. */
  virtual PointType Evaluate( const PointType& pt ) const;

protected:
  PSMRBFCorrespondenceInterpolator();
  virtual ~PSMRBFCorrespondenceInterpolator() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }

private:
  PSMRBFCorrespondenceInterpolator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_Initialized;

  std::vector<PointType> m_PointSetA;
  std::vector<PointType> m_PointSetB;

  /** Parameter matrices */
  vnl_matrix<double> m_P;
  vnl_matrix<double> m_C;
  
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMRBFCorrespondenceInterpolator.hxx"
#endif

#endif
