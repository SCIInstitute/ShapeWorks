/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleEntropyFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleEntropyFunction_h
#define __itkParticleEnsembleEntropyFunction_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{

/**
 * \class ParticleEnsembleEntropyFunction
 *
 */
template <unsigned int VDimension>
class ParticleEnsembleEntropyFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleEntropyFunction, ParticleVectorFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  typedef ParticleShapeMatrixAttribute<double, VDimension> ShapeMatrixType;

  typedef typename ShapeMatrixType::DataType DataType;
  
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
  
  /** Write the first n modes to +- 3 std dev and the mean of the model
      described by the covariance matrix.  The string argument is a prefix to
      the file names. */
  void WriteModes(const std::string &, int) const;
  
  /**Access the shape matrix. */
  void SetShapeMatrix( ShapeMatrixType *s)
  {    m_ShapeMatrix = s;  }
  ShapeMatrixType *GetShapeMatrix()
  {   return  m_ShapeMatrix.GetPointer();  }
  const ShapeMatrixType *GetShapeMatrix() const
  {   return  m_ShapeMatrix.GetPointer();  }

  /** Called before each iteration of a solver. */
  virtual void BeforeIteration()
  {
    m_ShapeMatrix->BeforeIteration();

    if (m_Counter == 0)
      {
      this->ComputeCovarianceMatrix();
      }
  }

  /** Called after each iteration of the solver. */
  virtual void AfterIteration()
  {
    m_ShapeMatrix->AfterIteration();
    // Update the annealing parameter.
    if (m_HoldMinimumVariance != true && !m_UseMeanEnergy)
      {
      m_Counter ++;
      if (m_Counter >=  m_RecomputeCovarianceInterval)
        {
        m_Counter = 0;
        m_MinimumVariance *= m_MinimumVarianceDecayConstant;
        }
      }
  }

  /** */
  void SetMinimumVariance( double d)
  { m_MinimumVariance = d;}
  double GetMinimumVariance() const
  { return m_MinimumVariance; }
  
  void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period)
  {
    m_MinimumVarianceDecayConstant =  exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance = initial_value;
    m_HoldMinimumVariance = false;
  } 
  bool GetMinimumVarianceDecayConstant() const
  {
    return m_MinimumVarianceDecayConstant;
  }
  
  void PrintShapeMatrix()
  {
      m_ShapeMatrix->PrintMatrix();
  }

  void UseMeanEnergy()
  { m_UseMeanEnergy = true; }
  void UseEntropy()
  { m_UseMeanEnergy = false; }

  /** */
  bool GetHoldMinimumVariance() const
  { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b)
  { m_HoldMinimumVariance = b; }

  void SetRecomputeCovarianceInterval(int i)
  { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const
  { return m_RecomputeCovarianceInterval; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEnsembleEntropyFunction<VDimension>::Pointer copy = ParticleEnsembleEntropyFunction<VDimension>::New();

    copy->m_PointsUpdate = this->m_PointsUpdate;
    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumEigenValue = this->m_MinimumEigenValue;
    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;
    copy->m_ShapeMatrix = this->m_ShapeMatrix;

    copy->m_InverseCovMatrix = this->m_InverseCovMatrix;
    copy->m_points_mean = this->m_points_mean;
    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }

protected:
  ParticleEnsembleEntropyFunction()
  {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;//log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;
    m_PointsUpdate = new vnl_matrix_type(10,10);
    m_InverseCovMatrix = new vnl_matrix_type(10,10);
    m_points_mean = new vnl_matrix_type(10,10);
  }
  virtual ~ParticleEnsembleEntropyFunction() {}
  void operator=(const ParticleEnsembleEntropyFunction &);
  ParticleEnsembleEntropyFunction(const ParticleEnsembleEntropyFunction &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  virtual void ComputeCovarianceMatrix();
  vnl_matrix_type * m_PointsUpdate;
  double m_MinimumVariance;
  double m_MinimumEigenValue;
  double m_CurrentEnergy;
  bool m_HoldMinimumVariance;
  double m_MinimumVarianceDecayConstant;
  int m_RecomputeCovarianceInterval;
  int m_Counter;
  bool m_UseMeanEnergy;

  vnl_matrix_type * m_InverseCovMatrix; // 3Nx3N - used for energy computation
  vnl_matrix_type * m_points_mean; //3NxM - used for energy computation

};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleEntropyFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleEntropyFunction.txx"
#endif

#endif
