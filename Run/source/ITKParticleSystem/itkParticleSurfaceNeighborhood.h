/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSurfaceNeighborhood.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSurfaceNeighborhood_h
#define __itkParticleSurfaceNeighborhood_h

#include "itkParticleRegionNeighborhood.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{
/** \class ParticleSurfaceNeighborhood
 *
 * ParticleSurfaceNeighborhood is a general purpose neighborhood object that
 * computes neighborhoods based on distance from a point.  It requires a domain
 * that provides bounds information and a distance metric.  This class uses a
 * PowerOfTwoPointTree to cache point and index values so that
 * FindNeighborhoodPoints is somewhat optimized. 
 */
template <class TImage>
class ITK_EXPORT ParticleSurfaceNeighborhood : public ParticleRegionNeighborhood<TImage::ImageDimension>
{
public:
  /** Standard class typedefs */
  typedef TImage ImageType;
  typedef ParticleSurfaceNeighborhood Self;
  typedef ParticleRegionNeighborhood<TImage::ImageDimension> Superclass;
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
  itkTypeMacro(ParticleSurfaceNeighborhood, ParticleRegionNeighborhood);

  /** Inherited typedefs from parent class. */
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef ParticleImplicitSurfaceDomain<typename TImage::PixelType, Dimension> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  /** Compile a list of points that are within a specified radius of a given
      point.  This implementation uses a PowerOfTwoTree to sort points
      according to location. */
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, std::vector<double> &, double) const;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

protected:
  ParticleSurfaceNeighborhood() : m_FlatCutoff(0.30)  {  }
  virtual ~ParticleSurfaceNeighborhood() {};

private:
  ParticleSurfaceNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  double m_FlatCutoff;

};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleSurfaceNeighborhood+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleSurfaceNeighborhood.txx"
#endif

#include "itkParticleSurfaceNeighborhood.txx"

#endif
