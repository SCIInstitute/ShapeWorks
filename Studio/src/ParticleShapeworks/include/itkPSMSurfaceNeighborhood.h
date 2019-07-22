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
#ifndef __itkPSMSurfaceNeighborhood_h
#define __itkPSMSurfaceNeighborhood_h

#include "itkPSMRegionNeighborhood.h"
#include "itkPSMImplicitSurfaceDomain.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{
/** \class PSMSurfaceNeighborhood
 *
 * \brief 
 * PSMSurfaceNeighborhood is a general purpose neighborhood object
 * that computes neighborhoods based on distance from a point.  It
 * requires a domain that provides bounds information and a distance
 * metric.  This class uses a PowerOfTwoPointTree to cache point and
 * index values so that FindNeighborhoodPoints is somewhat
 * optimized. PSMSurfaceNeighborhood adds the concept of distanes and
 * neighbors in the surface tangent plane to the concept of
 * PSMRegionNeighborhood, which has no knowledge of surfaces.
 * 
 * \ingroup PSM
 * \ingroup PSMContainers
 *
 * \author Josh Cates
 */
template <class TImage>
class ITK_EXPORT PSMSurfaceNeighborhood : public PSMRegionNeighborhood<TImage::ImageDimension>
{
public:
  /** Standard class typedefs */
  typedef TImage ImageType;
  typedef PSMSurfaceNeighborhood Self;
  typedef PSMRegionNeighborhood<TImage::ImageDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  typedef typename ImageType::PixelType NumericType;
  typedef typename Superclass::PointTreeType PointTreeType;

  typedef  vnl_vector_fixed<NumericType, TImage::ImageDimension> GradientVectorType;
  
  /**Expose the image dimension. */
  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMSurfaceNeighborhood, PSMRegionNeighborhood);

  /** Inherited typedefs from parent class. */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef PSMImplicitSurfaceDomain<typename TImage::PixelType, Dimension> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  /** Compile a list of points that are within a specified radius of a given
      point.  This implementation uses a PowerOfTwoTree to sort points
      according to location. */
  virtual PointVectorType FindNeighborhoodPointsWithWeights(const PointType &, std::vector<double> &, double) const;

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

protected:
  PSMSurfaceNeighborhood() : m_FlatCutoff(0.30)  {  }
  virtual ~PSMSurfaceNeighborhood() {};

private:
  PSMSurfaceNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  double m_FlatCutoff;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMSurfaceNeighborhood.hxx"
#endif

#endif
