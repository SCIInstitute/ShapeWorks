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
 *  Unless required by applicabcle law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMShapeEntropyFunction_h
#define __itkPSMShapeEntropyFunction_h

#include "itkPSMShapeMatrixAttribute.h"
#include "itkPSMCostFunction.h"
#include <vector>

namespace itk
{

/**
 * \class PSMShapeEntropyFunction
 *
 * This function estimates the entropy of a distribution of
 * particle-system-based shapes (itkPSMParticleSystem) in shape space.
 * It also computes the gradient of the entropy function.  This class
 * is used to optimize the correspondence of particle positions across
 * shapes samples through minimization of the entropy of their
 * distribution in shape space.
 * 
 * \ingroup PSM
 * \ingroup PSMOptimization
 * \ingroup PSMCostFunctions
 * \author Josh Cates
 */
template <unsigned int VDimension>
class PSMShapeEntropyFunction : public PSMCostFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef PSMShapeEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef PSMCostFunction<VDimension> Superclass;
  itkTypeMacro( PSMShapeEntropyFunction, PSMCostFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  typedef PSMShapeMatrixAttribute<double, VDimension> ShapeMatrixType;

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
  //  void WriteModes(const std::string &, int) const;
  
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
    if (m_HoldMinimumVariance != true)
      {
      m_Counter ++;
      if (m_Counter >=  m_RecomputeCovarianceInterval)
        {
        m_Counter = 0;
        m_MinimumVariance *= m_MinimumVarianceDecayConstant;
        }
      }
  }

  /** MinimumVariance is a regularization parameter that is added to
      the diagonal elements of the covariance matrix of the particle
      positions on shape.  Higher MinimumVariance produces larger
      gradients, thus allowing shapes to move larger distances in
      shape space during optimization of positions with this function.
      Higher MinimumVariance values minimize the effects of smaller
      modes of variation in the covariance matrix.  Conversely,
      smaller MinimumVariance values increase the significance of the
      smaller modes of variation on the gradient calculation.  This
      class allows for an "annealing" type optimization using the
      SetMinumumVarianceDecay function. */
  void SetMinimumVariance( double d)
  { m_MinimumVariance = d;}
  double GetMinimumVariance() const
  { return m_MinimumVariance; }
  
  /** Sets up an "annealing" type of optimization strategy when this
      method is used within a PSMOptimizer class.  This method
      specifies and initial value, a final value, and a time period
      (number of iterations) over which MinimumValue regularization
      parameter will decay.*/
  void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period)
  {
    m_MinimumVarianceDecayConstant =  exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance = initial_value;
    m_HoldMinimumVariance = false;
  } 

  /** Returns the decay constant value computed from SetMinimumVarianceDecay. */
  bool GetMinimumVarianceDecayConstant() const
  {
    return m_MinimumVarianceDecayConstant;
  }

  /** This method gets/sets the HoldMinimumVariance parameter, which
      turns on (true) or off (false) the gradual decrease in the
      m_MinumumVariance regularization parameter with successive
      invocations of the */
  bool GetHoldMinimumVariance() const
  { return m_HoldMinimumVariance; }
  void SetHoldMinimumVariance(bool b)
  { m_HoldMinimumVariance = b; }

  /** Set/Get the number of times this function can be called before
      the covariance matrix is recomputed.  In general, it may not be
      necessary to recompute the covariance matrix every time that
      particle positions are updated.  However, the default value of 1
      is the most accurate.  For small numbers of samples and
      reasonably small numbers of particles (e.g. < 2000 per shape), a
      default value of 1 should not incur much of a time penalty.*/
  void SetRecomputeCovarianceInterval(int i)
  { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const
  { return m_RecomputeCovarianceInterval; }

  /** Returns a vector of the variances associated with the PCA
      decomposition of the shape space.  There will be only
      number_of_samples - 1 total valid PCA modes.*/
  const std::vector<double> &GetShapePCAVariances() const
  {
    return m_ShapePCAVariances;
  }

protected:
  PSMShapeEntropyFunction()
  {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;//log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
  }
  virtual ~PSMShapeEntropyFunction() {}
  void operator=(const PSMShapeEntropyFunction &);
  PSMShapeEntropyFunction(const PSMShapeEntropyFunction &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  std::vector<double> m_ShapePCAVariances;

  virtual void ComputeCovarianceMatrix();
  vnl_matrix_type m_PointsUpdate;
  double m_MinimumVariance;
  double m_MinimumEigenValue;
  double m_CurrentEnergy;
  bool m_HoldMinimumVariance;
  double m_MinimumVarianceDecayConstant;
  int m_RecomputeCovarianceInterval;
  int m_Counter;

};


} //end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMShapeEntropyFunction.hxx"
#endif

#endif

