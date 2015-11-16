/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleNonparametricEntropyFunction.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleNonparametricEntropyFunction_h
#define __itkParticleEnsembleNonparametricEntropyFunction_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{

/**
 * \class ParticleEnsembleNonparametricEntropyFunction
 *
 * NOTE: This class may only be used in one thread at a time.
 * 
 */
template <class TGradientNumericType, unsigned int VDimension>
class ParticleEnsembleNonparametricEntropyFunction
  : public ParticleEntropyGradientFunction<TGradientNumericType,VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleNonparametricEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleEntropyGradientFunctionFunction<TGradientNumericType,VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleNonparametricEntropyFunction,
                ParticleEntropyGradientFunction);

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
                              const ParticleSystemType *, double &) const;

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
    if (m_Counter == 0)
      {
      this->ComputeProjectedShapeMatrix();
      }
  }

  /** Called after each iteration of the solver. */
  virtual void AfterIteration()
  {
    if (m_Counter >=  m_RecomputeProjectedShapeMatrixInterval)
      {
      m_Counter = 0;
      //        m_MinimumVariance *= m_MinimumVarianceBase;
      }
    else
      {
      m_Counter++;
      }
  }

  /** Set/Get the percentage of the total variation that the subspace should contain.*/
  void SetVarianceThreshold( double d)
  { m_VarianceThreshold = d;}
  double GetVarianceThreshold() const
  { return m_VarianceTrheshold; }
  
  void SetRecomputeProjectedShapeMatrixInterval(int i)
  { m_RecomputeProjectedShapeMatrixInterval = i; }
  int GetRecomputeProjectedShapeMatrixInterval() const
  { return m_RecomputeProjectedShapeMatrixInterval; }

protected:
  ParticleEnsembleNonparametricEntropyFunction() : m_VarianceThreshold(1.0),
                                                   m_RecomputeProjectedShapeMatrixInterval(5),
                                                   m_Counter(0)
  {  }
  virtual ~ParticleEnsembleNonparametricEntropyFunction() {}
  void operator=(const ParticleEnsembleNonparametricEntropyFunction &);
  ParticleEnsembleNonparametricEntropyFunction(const ParticleEnsembleNonparametricEntropyFunction &);

  typename ShapeMatrixType::Pointer m_ShapeMatrix;
  vnl_matrix_type m_ProjectedShapeMatrix;

  virtual void ComputeProjectedShapeMatrix();

  double m_VarianceThreshold;
  int m_RecomputeProjectedShapeMatrixInterval;
  int m_Counter;

};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleNonparametricEntropyFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleNonparametricEntropyFunction.txx"
#endif

#endif
