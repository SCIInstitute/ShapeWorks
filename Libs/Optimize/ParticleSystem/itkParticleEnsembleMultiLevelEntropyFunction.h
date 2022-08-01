#ifndef __itkParticleEnsembleMultiLevelEntropyFunction_h
#define __itkParticleEnsembleMultiLevelEntropyFunction_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkParticleVectorFunction.h"
#include <vector>
#include <Eigen/Eigen>


namespace itk
{

/**
 * \class ParticleEnsembleMultiLevelEntropyFunction
 *
 */
template <unsigned int VDimension>
class ParticleEnsembleMultiLevelEntropyFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleMultiLevelEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleMultiLevelEntropyFunction, ParticleVectorFunction);

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

  void SetDomainsPerShapeInfo(int dps)
  {
    m_total_organs = m_ShapeMatrix->GetDomainsPerShape();
    std::cout << "Setting DPS Info...\ndps = " << dps << " m_total_organs = " << m_total_organs << std::endl;
    m_MinimumEigenValue_rel_pose = 0.0;
    m_MinimumEigenValue_shape_dev_ar.resize(m_total_organs, 0.0);
    m_MinimumVarianceDecayConstant_rel_pose = 1.0;
    m_MinimumVarianceDecayConstant_shape_dev_ar.resize(m_total_organs);
    m_MinimumVariance_rel_pose = 1.0e-5;
    m_MinimumVariance_shape_dev_ar.resize(m_total_organs);
    std::fill(m_MinimumVarianceDecayConstant_shape_dev_ar.begin(), m_MinimumVarianceDecayConstant_shape_dev_ar.end(), 1.0);
    std::fill(m_MinimumVariance_shape_dev_ar.begin(), m_MinimumVariance_shape_dev_ar.end(),  1.0e-5);
  }

  /** Called before each iteration of a solver. */
  virtual void BeforeIteration()
  {
    m_ShapeMatrix->BeforeIteration();

    if (m_Counter == 0)
      {
      // this->ComputeCovarianceMatrix();
      this->ComputeShapeRelPoseDeviations();
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

  void SetMinimumVariance_rel_pose( double d)
  { m_MinimumVariance_rel_pose = d;}
  double GetMinimumVariance_rel_pose() const
  { return m_MinimumVariance_rel_pose; }

  void SetMinimumVariance_shape_dev_ar( std::vector<double> d)
  { 
    for (unsigned int i = 0; i < d.size(); i++){ m_MinimumVariance_shape_dev_ar[i] = d[i]; }
  }
  std::vector<double> GetMinimumVariance_shape_dev_ar() const
  { return m_MinimumVariance_shape_dev_ar; }
  

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

  void SetMinimumVarianceDecay_rel_pose(double initial_value, double final_value, double time_period)
  {
    m_MinimumVarianceDecayConstant_rel_pose =  exp(log(final_value / initial_value) / time_period);
    m_MinimumVariance_rel_pose = initial_value;
    m_HoldMinimumVariance_rel_pose = false;
  }

  bool GetMinimumVarianceDecayConstant_rel_pose() const
  {
    return m_MinimumVarianceDecayConstant_rel_pose;
  }
  
  void SetMinimumVarianceDecay_shape_dev_ar(std::vector <double> initial_values, std::vector <double> final_values, double time_period)
  {
    unsigned int n = initial_values.size();
    for (unsigned int i = 0; i < n; i++)
    {
      m_MinimumVarianceDecayConstant_shape_dev_ar[i] = exp(log(final_values[i] / initial_values[i]) / time_period);
      m_MinimumVariance_shape_dev_ar[i] = initial_values[i];
    }
    m_HoldMinimumVariance_shape_dev = false;
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

  bool GetHoldMinimumVariance_shape_dev() const
  { return m_HoldMinimumVariance_shape_dev; }
  void SetHoldMinimumVariance_shape_dev(bool b)
  { m_HoldMinimumVariance_shape_dev = b; }


  bool GetHoldMinimumVariance_rel_pose() const
  { return m_HoldMinimumVariance_rel_pose; }
  void SetHoldMinimumVariance_rel_pose(bool b)
  { m_HoldMinimumVariance_rel_pose = b; }

  void SetRecomputeCovarianceInterval(int i)
  { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const
  { return m_RecomputeCovarianceInterval; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEnsembleMultiLevelEntropyFunction<VDimension>::Pointer copy = ParticleEnsembleMultiLevelEntropyFunction<VDimension>::New();

    copy->m_PointsUpdate_shape_dev = this->m_PointsUpdate_shape_dev;
    copy->m_PointsUpdate_rel_pose = this->m_PointsUpdate_rel_pose;

    copy->m_MinimumVariance = this->m_MinimumVariance;
    copy->m_MinimumVariance_shape_dev_ar = this->m_MinimumVariance_shape_dev_ar;
    copy->m_MinimumVariance_rel_pose = this->m_MinimumVariance_rel_pose;

    copy->m_MinimumEigenValue = this->m_MinimumEigenValue;
    copy->m_MinimumEigenValue_shape_dev_ar = this->m_MinimumEigenValue_shape_dev_ar;
    copy->m_MinimumEigenValue_rel_pose = this->m_MinimumEigenValue_rel_pose;

    copy->m_CurrentEnergy = this->m_CurrentEnergy;
    copy->m_CurrentEnergy_shape_dev_ar = this->m_CurrentEnergy_shape_dev_ar;
    copy->m_CurrentEnergy_rel_pose = this->m_CurrentEnergy_rel_pose;

    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_HoldMinimumVariance_shape_dev = this->m_HoldMinimumVariance_shape_dev;
    copy->m_HoldMinimumVariance_rel_pose = this->m_HoldMinimumVariance_rel_pose;

    

    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    copy->m_ShapeMatrix = this->m_ShapeMatrix;

    copy->m_super_matrix = this->m_super_matrix;

    copy->m_InverseCovMatrix_shape_dev = this->m_InverseCovMatrix_shape_dev;
    copy->m_InverseCovMatrix_rel_pose = this->m_InverseCovMatrix_rel_pose;

    copy->m_points_mean_rel_pose = this->m_points_mean_rel_pose;
    copy->m_points_mean_shape_dev = this->m_points_mean_shape_dev;

    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }

protected:
  ParticleEnsembleMultiLevelEntropyFunction()
  {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_HoldMinimumVariance_rel_pose = true;
    m_HoldMinimumVariance_shape_dev = true;

    m_MinimumVariance = 1.0e-5;
    m_MinimumEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;//log(2.0) / 50000.0;


    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;

    m_super_matrix = std::make_shared<vnl_matrix_type>(10, 10); 

    m_PointsUpdate_shape_dev = std::make_shared<std::vector<vnl_matrix_type>>();
    m_PointsUpdate_rel_pose = std::make_shared<vnl_matrix_type>(10, 10);

    m_InverseCovMatrix_shape_dev = std::make_shared<std::vector<vnl_matrix_type>>();
    m_InverseCovMatrix_rel_pose = std::make_shared<vnl_matrix_type>(10, 10);

    m_points_mean_shape_dev = std::make_shared<std::vector<vnl_matrix_type>>();
    m_points_mean_rel_pose= std::make_shared<vnl_matrix_type>(10, 10);

  }
  virtual ~ParticleEnsembleMultiLevelEntropyFunction() {}
  void operator=(const ParticleEnsembleMultiLevelEntropyFunction &);
  ParticleEnsembleMultiLevelEntropyFunction(const ParticleEnsembleMultiLevelEntropyFunction &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;



  // virtual void ComputeCovarianceMatrix();
  virtual void ComputeShapeRelPoseDeviations();
  virtual void ComputeCentroidForShapeVector(const vnl_matrix_type &shape_vector, vnl_matrix_type &centroid_results) const;

  std::shared_ptr<vnl_matrix_type> m_PointsUpdate_rel_pose;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_PointsUpdate_shape_dev;

  double m_MinimumVariance;
  double m_MinimumVariance_rel_pose;
  std::vector<double> m_MinimumVariance_shape_dev_ar;

  double m_MinimumEigenValue;
  double m_MinimumEigenValue_rel_pose;
  std::vector<double> m_MinimumEigenValue_shape_dev_ar;

  double m_CurrentEnergy;
  double m_CurrentEnergy_rel_pose;
  std::vector<double> m_CurrentEnergy_shape_dev_ar;
  
  bool m_HoldMinimumVariance;
  bool m_HoldMinimumVariance_shape_dev;
  bool m_HoldMinimumVariance_rel_pose;

  double m_MinimumVarianceDecayConstant;
  double m_MinimumVarianceDecayConstant_rel_pose;
  std::vector<double> m_MinimumVarianceDecayConstant_shape_dev_ar;
  
  int m_RecomputeCovarianceInterval;
  int m_Counter;
  bool m_UseMeanEnergy;

  unsigned int m_total_organs;
  std::vector<int> m_num_particles_ar;

  std::shared_ptr<vnl_matrix_type> m_super_matrix;

  std::shared_ptr<vnl_matrix_type> m_points_mean_rel_pose; 
  std::shared_ptr<vnl_matrix_type> m_InverseCovMatrix_rel_pose; 

  std::shared_ptr<std::vector<vnl_matrix_type>> m_points_mean_shape_dev; 
  std::shared_ptr<std::vector<vnl_matrix_type>> m_InverseCovMatrix_shape_dev; 

};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleMultiLevelEntropyFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleMultiLevelEntropyFunction.txx"
#endif

#include "itkParticleEnsembleMultiLevelEntropyFunction.txx"

#endif