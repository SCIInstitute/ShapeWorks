/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleQualifierEntropyGradientFunction.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleQualifierEntropyGradientFunction_h

#define __itkParticleQualifierEntropyGradientFunction_h



#include "itkParticleVectorFunction.h"

#include "itkParticleContainerArrayAttribute.h"

#include "itkParticleImageDomainWithGradients.h"

#include <vector>



namespace itk

{



/**

 */

template <class TGradientNumericType, unsigned int VDimension>

class ParticleQualifierEntropyGradientFunction : public ParticleVectorFunction<VDimension>

{

public:

 /** Standard class typedefs. */

  typedef ParticleQualifierEntropyGradientFunction Self;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self>  ConstPointer;

  typedef ParticleVectorFunction<VDimension> Superclass;

  itkTypeMacro( ParticleQualifierEntropyGradientFunction, ParticleVectorFunction);



  /** Data type representing individual gradient components. */

  typedef TGradientNumericType GradientNumericType;



  /** Type of particle system. */

  typedef typename Superclass::ParticleSystemType ParticleSystemType;



  /** Cache type for the sigma values. */

  typedef ParticleContainerArrayAttribute<double, VDimension> SigmaCacheType;



  /** Vector & Point types. */

  typedef typename Superclass::VectorType VectorType;

  typedef typename ParticleSystemType::PointType PointType;

  typedef  vnl_vector_fixed<TGradientNumericType, VDimension> GradientVectorType;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);



  /** Dimensionality of the domain of the particle system. */

  itkStaticConstMacro(Dimension, unsigned int, VDimension);



  /** The first argument is a pointer to the particle system.  The second

      argument is the index of the domain within that particle system.  The

      third argument is the index of the particle location within the given

      domain. */

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,

                              double &) const;



  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,

                              double &, double &) const

  {

    itkExceptionMacro("This method not implemented");

    return VectorType();

  }

  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const

  {

    itkExceptionMacro("This method not implemented");

    return 0.0;

  }



  virtual void ResetBuffers()

  {

    m_SpatialSigmaCache->ZeroAllValues();

  }

    

  

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.

      The best sigma is the sigma that maximizes probability at the given point  */

  virtual double EstimateSigma(unsigned int, const typename ParticleSystemType::PointVectorType &,

                                const std::vector<double> &,

                                    const PointType &, double,  double,  int &err) const;



  /** Returns a weighting coefficient based on the angle between two

     vectors. Weights smoothly approach zero as the angle between two

     normals approaches 90 degrees. */

  TGradientNumericType AngleCoefficient(const GradientVectorType&,

                                        const GradientVectorType&) const;

  

  /** Minimum radius of the neighborhood of points that are considered in the

      calculation. The neighborhood is a spherical radius in 3D space. The

      actual radius used in a calculation may exceed this value, but will not

      exceed the MaximumNeighborhoodRadius. */

  void SetMinimumNeighborhoodRadius( double s)

  { m_MinimumNeighborhoodRadius = s; }

  double GetMinimumNeighborhoodRadius() const

  { return m_MinimumNeighborhoodRadius; }



  /** Maximum radius of the neighborhood of points that are considered in the

      calculation. The neighborhood is a spherical radius in 3D space. */

  void SetMaximumNeighborhoodRadius( double s)

  { m_MaximumNeighborhoodRadius = s; }

  double GetMaximumNeighborhoodRadius() const

  { return m_MaximumNeighborhoodRadius; }



  /** Numerical parameters*/

  void SetFlatCutoff(double s)

  { m_FlatCutoff = s; }

  double GetFlatCutoff() const

  { return m_FlatCutoff; }

  void SetNeighborhoodToSigmaRatio(double s)

  { m_NeighborhoodToSigmaRatio = s; }

  double GetNeighborhoodToSigmaRatio() const

  { return m_NeighborhoodToSigmaRatio; }



  /**Access the cache of sigma values for each particle position.  This cache

     is populated by registering this object as an observer of the correct

     particle system (see SetParticleSystem).*/

  void SetSpatialSigmaCache( SigmaCacheType *s)

  {    m_SpatialSigmaCache = s;  }

  SigmaCacheType *GetSpatialSigmaCache()

  {   return  m_SpatialSigmaCache.GetPointer();  }

  const SigmaCacheType *GetSpatialSigmaCache() const

  {   return  m_SpatialSigmaCache.GetPointer();  }



  /** Compute a set of weights based on the difference in the normals of a

      central point and each of its neighbors.  Difference of > 90 degrees

      results in a weight of 0. */

  void ComputeAngularWeights(const PointType &,

                             const typename ParticleSystemType::PointVectorType &,

                             const ParticleImageDomainWithGradients<TGradientNumericType, VDimension> *,

                             std::vector<double> &) const;





  //  void ComputeNeighborho0d();

  

protected:

  

  ParticleQualifierEntropyGradientFunction() : m_FlatCutoff(0.05), m_NeighborhoodToSigmaRatio(3.0) {}

  virtual ~ParticleQualifierEntropyGradientFunction() {}

  void operator=(const ParticleQualifierEntropyGradientFunction &);

  ParticleQualifierEntropyGradientFunction(const ParticleQualifierEntropyGradientFunction &);



  double m_MinimumNeighborhoodRadius;

  double m_MaximumNeighborhoodRadius;

  double m_FlatCutoff;

  double m_NeighborhoodToSigmaRatio;

  typename SigmaCacheType::Pointer m_SpatialSigmaCache;

};





} //end namespace



#if ITK_TEMPLATE_EXPLICIT

# include "Templates/itkParticleQualifierEntropyGradientFunction+-.h"

#endif



#if ITK_TEMPLATE_TXX

# include "itkParticleQualifierEntropyGradientFunction.txx"

#endif



#endif





