/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGeneralEntropyGradientFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGeneralEntropyGradientFunction_h
#define __itkParticleGeneralEntropyGradientFunction_h

#include "itkParticleFunctionBasedShapeSpaceData.h"
#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{

/**
 * \class ParticleGeneralEntropyGradientFunction
 *
 */
template <unsigned int VDimension>
class ParticleGeneralEntropyGradientFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleGeneralEntropyGradientFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleGeneralEntropyGradientFunction, ParticleVectorFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  typedef ParticleFunctionBasedShapeSpaceData<float, VDimension> ShapeDataType;

  typedef typename ShapeDataType::DataType DataType;
  
  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int, unsigned int,
                              const ParticleSystemType *, double &, double&) const;

  virtual VectorType Evaluate(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->Evaluate(a,b,c,d,e);
  }

  
  virtual double Energy(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    double e, d;
    this->Evaluate(a,b,c,d,e);
    return e;
  }
  
  /** Called before each iteration of a solver. */
  virtual void BeforeIteration()
  {
    //    if (m_Counter == 0)
    //      {
    this->ComputeUpdates();
    //      }
  }

  /** Called after each iteration of the solver. */
  virtual void AfterIteration()
  {
    // Update the annealing parameter.
    if (m_HoldMinimumVariance != true)
      {
      m_Counter++;
      if (m_Counter >=  m_RecomputeCovarianceInterval)
        {
        m_Counter = 0;
        m_MinimumVariance *= m_MinimumVarianceDecayConstant;
        }
      }
  }
  void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period)
  {
    m_MinimumVarianceDecayConstant =  exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance = initial_value;
    m_HoldMinimumVariance = false;
  } 
  
  /** */
  void SetMinimumVariance( double d)
  { m_MinimumVariance = d;}
  double GetMinimumVariance() const
  { return m_MinimumVariance; }
  
  /** */
  //  void SetMinimumVarianceBase( double d)
  //  {    m_MinimumVarianceBase = d;  }
  //  double GetMinimumVarianceBase() const
  //  {    return m_MinimumVarianceBase;  }

  /** */
  bool GetHoldMinimumVariance() const
  { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b)
  { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i)
  { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const
  { return m_RecomputeCovarianceInterval; }

  /**Access the shape matrix. */
  void SetShapeData( ShapeDataType *s)
  {    m_ShapeData = s;  }
  ShapeDataType *GetShapeData()
  {   return  m_ShapeData.GetPointer();  }
  const ShapeDataType *GetShapeData() const
  {   return  m_ShapeData.GetPointer();  }

  void SetAttributeScales( const std::vector<double> &s)
  {
    m_AttributeScales = s;
  }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleGeneralEntropyGradientFunction<VDimension>::Pointer copy =
      ParticleGeneralEntropyGradientFunction<VDimension>::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    // local
    copy->m_AttributeScales = this->m_AttributeScales;
    copy->m_Counter = this->m_Counter;
    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumEigenValue  = this->m_MinimumEigenValue;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_PointsUpdate = this->m_PointsUpdate;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_ShapeData = this->m_ShapeData;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }
  
protected:
  ParticleGeneralEntropyGradientFunction()
  {
 // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 5;
    m_Counter = 0;
    
  }
  virtual ~ParticleGeneralEntropyGradientFunction() {}
  void operator=(const ParticleGeneralEntropyGradientFunction &);
  ParticleGeneralEntropyGradientFunction(const ParticleGeneralEntropyGradientFunction &);
  typename ShapeDataType::Pointer m_ShapeData;

  virtual void ComputeUpdates();
  vnl_matrix_type m_PointsUpdate;
  double m_MinimumVariance;
  double m_MinimumEigenValue;
  //  double m_MinimumVarianceBase;
  bool m_HoldMinimumVariance;
  int m_RecomputeCovarianceInterval;
  double m_MinimumVarianceDecayConstant;
  int m_Counter;
  std::vector<double> m_AttributeScales;

  double m_CurrentEnergy;
  
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleGeneralEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleGeneralEntropyGradientFunction.txx"
#endif

#endif

