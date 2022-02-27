#ifndef __itkParticleEnsembleMlpcaEntropyFunction_h
#define __itkParticleEnsembleMlpcaEntropyFunction_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{

/**
 * \class ParticleEnsembleMlpcaEntropyFunction
 *
 */
template <unsigned int VDimension>
class ParticleEnsembleMlpcaEntropyFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleMlpcaEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleMlpcaEntropyFunction, ParticleVectorFunction);

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

  
  virtual VectorType Evaluate(unsigned int, unsigned int,
                              const ParticleSystemType *, double &, double&) const
  
  {
    itkExceptionMacro("This method not implemented");
    return VectorType();
  }
                        
  virtual VectorType Evaluate(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    itkExceptionMacro("This method not implemented");
    return VectorType();
  }
  
  virtual double Energy(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    itkExceptionMacro("This method not implemented");
    return 0.0;
  }
  
  virtual VectorType EvaluateFromWithinResiduals(unsigned int, unsigned int,
                              const ParticleSystemType *, double &, double&) const;
  virtual VectorType EvaluateFromWithinResiduals(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->EvaluateFromWithinResiduals(a,b,c,d,e);
  }
  
  virtual VectorType EvaluateFromBetweenResiduals(unsigned int, unsigned int,
                              const ParticleSystemType *, double &, double&) const;
  virtual VectorType EvaluateFromBetweenResiduals(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->EvaluateFromBetweenResiduals(a,b,c,d,e);
  }
  
  virtual double EnergyFromWithinResiduals(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    /*
      This function computes the Energy in Between Subspace 
      which is computed based on the covariance matrix built from within residuals[Y_betweenSubspace = P - grand mean - withinTerms]
    */
    double e, d;
    this->EvaluateFromWithinResiduals(a,b,c,d,e);
    return e;
  }

  virtual double EnergyFromBetweenResiduals(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    /*
      This function computes the Energy in Within Subspace 
      which is computed based on the covariance matrix built from Between residuals[Y_withinSubspace = P - grand mean - betweenTerms]
    */
    double e, d;
    this->EvaluateFromBetweenResiduals(a,b,c,d,e);
    return e;
  }
  
  /** Write the first n modes to +- 3 std dev and the mean of the model
      described by the covariance matrix.  The string argument is a prefix to
      the file names. */
  void WriteModes(const std::string &, int) const;
  
  /**Access the shape matrix. */
  void SetShapeMatrix( ShapeMatrixType *s)
  {    
    m_ShapeMatrix = s;
  }

  void SetDomainsPerShapeInfo(int dps)
  {
    m_dps = m_ShapeMatrix->GetDomainsPerShape();
    std::cout << " m_dps is = " << m_dps << std::endl; 
    m_MinimumWithinResidualEigenValues.resize(m_dps, 0.0);
    m_MinimumBetweenResidualEigenValues.resize(m_dps, 0.0);
    
    m_MinimumVarianceDecayConstant_between_ar.resize(m_dps);
    m_MinimumVarianceDecayConstant_within_ar.resize(m_dps);
    m_MinimumVariance_within_ar.resize(m_dps);
    m_MinimumVariance_between_ar.resize(m_dps);
    std::fill(m_MinimumVarianceDecayConstant_between_ar.begin(), m_MinimumVarianceDecayConstant_between_ar.end(), 1.0);
    std::fill(m_MinimumVarianceDecayConstant_within_ar.begin(), m_MinimumVarianceDecayConstant_within_ar.end(), 1.0);
    std::fill(m_MinimumVariance_between_ar.begin(), m_MinimumVariance_between_ar.end(), 1.0e-5);
    std::fill(m_MinimumVariance_within_ar.begin(), m_MinimumVariance_within_ar.end(),  1.0e-5);
  }

  ShapeMatrixType *GetShapeMatrix()
  {   return  m_ShapeMatrix.GetPointer();  }
  const ShapeMatrixType *GetShapeMatrix() const
  {   return  m_ShapeMatrix.GetPointer();  }

  /** Called before each iteration of a solver. */
  virtual void BeforeIteration()
  {
    /*  
      Before Each Optimization iteration - Compute the MLPCA terms, and build covariance matrices from within and between residuals
    */
    m_ShapeMatrix->BeforeIteration();
    if (m_Counter == 0)
      {
      this->ComputeMlpcaTerms();
      this->ComputeCovarianceMatricesFromWithinResiduals();
      this->ComputeCovarianceMatricesFromBetweenResiduals();
      }
  }

  /** Called after each iteration of the solver. */
  virtual void AfterIteration()
  {
    m_ShapeMatrix->AfterIteration();
    // Update the annealing parameter.
    // if (m_HoldMinimumVariance != true && !m_UseMeanEnergy)
    //   {
    //   m_Counter ++;
    //   if (m_Counter >=  m_RecomputeCovarianceInterval)
    //     {
    //     m_Counter = 0;
    //     m_MinimumVariance *= m_MinimumVarianceDecayConstant;
    //     }
    //   }

    // 1. Within part
    if ((m_HoldMinimumVariance!= true || m_HoldMinimumVariance_within != true || m_HoldMinimumVariance_between != true) && !m_UseMeanEnergy)
    {
      m_Counter++;
      if (m_Counter >= m_RecomputeCovarianceInterval) //TODO: ensure proper usage here
      {
        m_Counter = 0;
        for (unsigned int k = 0; k < m_MinimumVariance_within_ar.size(); k++)
        {
          m_MinimumVariance_within_ar[k] *= m_MinimumVarianceDecayConstant_within_ar[k];
          m_MinimumVariance_between_ar[k] *= m_MinimumVarianceDecayConstant_between_ar[k];
          m_MinimumVariance *= m_MinimumVarianceDecayConstant;

        }
      }
    }
  }

  /** */
  void SetMinimumVariance( double d)
  { m_MinimumVariance = d;}
  double GetMinimumVariance() const
  { return m_MinimumVariance; }
  // Set Regularization parameters for multi-level optimization
   void SetMinimumVarianceBetween( double d)
  { 
    // m_MinimumVariance_between = d;
    std::fill(m_MinimumVariance_between_ar.begin(), m_MinimumVariance_between_ar.end(), d);
  }
  std::vector<double> GetMinimumVarianceBetween() const
  { return m_MinimumVariance_between_ar; }
  
  void SetMinimumVarianceWithin( std::vector<double> d)
  { 
    std::cout << "setting minimum variance within " << std::endl;

    for (unsigned int i = 0; i < d.size(); i++){
      m_MinimumVariance_within_ar[i] = d[i];
      std::cout << d[i] << " ";
    }
    std::cout << std::endl;
  }
  std::vector<double> GetMinimumVarianceWithin() const
  { return m_MinimumVariance_within_ar; }
  
  void SetMinimumVarianceDecayWithin(std::vector <double> initial_values, std::vector <double> final_values, double time_period)
  {
    unsigned int n = initial_values.size();
    for (unsigned int i = 0; i < n; i++)
    {
      m_MinimumVarianceDecayConstant_within_ar[i] = exp(log(final_values[i] / initial_values[i]) / time_period);
      m_MinimumVariance_within_ar[i] = initial_values[i];
      m_HoldMinimumVariance_within = false;
    }
  } 
   void SetMinimumVarianceDecayBetween(double initial_value, double final_value, double time_period)
  {
    double minimumVarianceDecayConstant_val =  exp(log(final_value / initial_value) / time_period);
    std::fill(m_MinimumVarianceDecayConstant_between_ar.begin(), m_MinimumVarianceDecayConstant_between_ar.end(), minimumVarianceDecayConstant_val);
    std::fill(m_MinimumVariance_between_ar.begin(), m_MinimumVariance_between_ar.end(), initial_value);
    m_HoldMinimumVariance_between = false;
  } 
 

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
  bool GetHoldMinimumVarianceBetween() const
  { return m_HoldMinimumVariance_between; }
  void SetHoldMinimumVarianceBetween(bool b)
  { m_HoldMinimumVariance_between = b; }
  bool GetHoldMinimumVarianceWithin() const
  { return m_HoldMinimumVariance_within; }
  void SetHoldMinimumVarianceWithin(bool b)
  { m_HoldMinimumVariance_within = b; }

  void SetRecomputeCovarianceInterval(int i)
  { m_RecomputeCovarianceInterval = i; }
  int GetRecomputeCovarianceInterval() const
  { return m_RecomputeCovarianceInterval; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::Pointer copy = ParticleEnsembleMlpcaEntropyFunction<VDimension>::New();

    copy->m_PointsUpdateAllBetween = this->m_PointsUpdateAllBetween;
    copy->m_PointsUpdateAllWithin = this->m_PointsUpdateAllWithin;
    copy->m_MinimumVariance = this->m_MinimumVariance;

    copy->m_MinimumVariance_between_ar = this->m_MinimumVariance_between_ar;
    copy->m_MinimumVarianceDecayConstant_between_ar = this->m_MinimumVarianceDecayConstant_between_ar;
    copy->m_MinimumVariance_within_ar = this->m_MinimumVariance_within_ar;
    copy->m_MinimumVarianceDecayConstant_within_ar = this->m_MinimumVarianceDecayConstant_within_ar;
    copy->m_HoldMinimumVariance_within = this->m_HoldMinimumVariance_within;
    copy->m_HoldMinimumVariance_between = this->m_HoldMinimumVariance_between;

    copy->m_MinimumWithinEigenValue = this->m_MinimumWithinEigenValue;
    copy->m_MinimumBetweenEigenValue = this->m_MinimumBetweenEigenValue;
    copy->m_CurrentBetweenEnergy = this->m_CurrentBetweenEnergy;
    copy->m_CurrentWithinEnergy = this->m_CurrentWithinEnergy;

    copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
    copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
    copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
    copy->m_Counter = m_Counter;
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;
    copy->m_ShapeMatrix = this->m_ShapeMatrix;
    copy->m_dps = this->m_dps;
    copy->m_UseMeanEnergy = this->m_UseMeanEnergy;


    copy->m_InverseCovMatricesAllBetween = this->m_InverseCovMatricesAllBetween;
    copy->m_points_meanAllBetween = this->m_points_meanAllBetween;
    copy->m_InverseCovMatricesAllWithin = this->m_InverseCovMatricesAllWithin;
    copy->m_points_meanAllWithin = this->m_points_meanAllWithin;
    copy->m_within_loadings = this->m_within_loadings;
    copy->m_within_scores = this->m_within_scores;
    copy->m_between_scores = this->m_between_scores;
    copy->m_between_loadings= this->m_between_loadings;
    copy->m_grand_mean = this->m_grand_mean;
    copy->m_super_matrix = this->m_super_matrix;
    copy->m_within_space_mean = this->m_within_space_mean;
    copy->m_between_space_mean = this->m_between_space_mean;
    copy->m_CurrentWithinResidualEnergies = this->m_CurrentWithinResidualEnergies;
    copy->m_CurrentBetweenResidualEnergies = this->m_CurrentBetweenResidualEnergies;
    copy->m_MinimumWithinResidualEigenValues = this->m_MinimumWithinResidualEigenValues;
    copy->m_MinimumBetweenResidualEigenValues = this->m_MinimumBetweenResidualEigenValues;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }

protected:
  ParticleEnsembleMlpcaEntropyFunction()
  {
    // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
    m_HoldMinimumVariance = true;
    m_HoldMinimumVariance_within = true;
    m_HoldMinimumVariance_between = true;
    m_MinimumVariance = 1.0e-5;
    m_MinimumWithinEigenValue = 0.0;
    m_MinimumBetweenEigenValue = 0.0;
    m_MinimumVarianceDecayConstant = 1.0;//log(2.0) / 50000.0;
    m_RecomputeCovarianceInterval = 1;
    m_Counter = 0;
    m_UseMeanEnergy = true;

    m_super_matrix = std::make_shared<vnl_matrix_type>(10, 10); 
    m_within_loadings = std::make_shared<std::vector<vnl_matrix_type>>();
    m_within_scores = std::make_shared<std::vector<vnl_matrix_type>>();
    m_between_scores = std::make_shared<std::vector<vnl_matrix_type>>();
    m_between_loadings = std::make_shared<vnl_matrix_type>(10, 10);
    m_grand_mean = std::make_shared<vnl_matrix_type>(10, 1);
    m_PointsUpdateAllWithin = std::make_shared<std::vector<vnl_matrix_type>>();
    m_InverseCovMatricesAllWithin = std::make_shared<std::vector<vnl_matrix_type>>();
    m_points_meanAllWithin = std::make_shared<std::vector<vnl_matrix_type>>();
    m_PointsUpdateAllBetween = std::make_shared<std::vector<vnl_matrix_type>>();
    m_InverseCovMatricesAllBetween = std::make_shared<std::vector<vnl_matrix_type>>();
    m_points_meanAllBetween = std::make_shared<std::vector<vnl_matrix_type>>();
    m_within_space_mean = std::make_shared<std::vector<vnl_matrix_type>>();
    m_between_space_mean =  std::make_shared<vnl_matrix_type>(10, 10);

    // m_CurrentWithinResidualEnergies = std::make_shared<std::vector<double>>();
    // m_CurrentBetweenResidualEnergies = std::make_shared<std::vector<double>>();
    // m_MinimumWithinResidualEigenValues = std::make_shared<std::vector<double>>();
    // m_MinimumBetweenResidualEigenValues = std::make_shared<std::vector<double>>();

  }
  virtual ~ParticleEnsembleMlpcaEntropyFunction() {}
  void operator=(const ParticleEnsembleMlpcaEntropyFunction &);
  ParticleEnsembleMlpcaEntropyFunction(const ParticleEnsembleMlpcaEntropyFunction &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  // std::shared_ptr<std::vector<vnl_matrix_type>> m_shape_matrices;

  virtual void ComputeCovarianceMatricesFromWithinResiduals();
  virtual void ComputeCovarianceMatricesFromBetweenResiduals();
  virtual void ComputeMlpcaTerms();
  virtual void ComputeWithinTerms(std::vector<int>& num_particles, unsigned int& dps, unsigned int& M, unsigned int& N);
  virtual void ComputeBetweenTerms(std::vector<int>& num_particles, unsigned int& dps, unsigned int& M, unsigned int& N);
  // std::shared_ptr<vnl_matrix_type> m_PointsUpdate;
  double m_MinimumVariance; // TODO: change for each domain
  // double m_MinimumEigenValue;
  double m_CurrentEnergy;
  bool m_HoldMinimumVariance;
  bool m_HoldMinimumVariance_within;
  bool m_HoldMinimumVariance_between;

  double m_MinimumVarianceDecayConstant;

  std::vector<double> m_MinimumVariance_within_ar;
  std::vector<double> m_MinimumVarianceDecayConstant_within_ar;
  std::vector<double> m_MinimumVariance_between_ar;
  std::vector<double> m_MinimumVarianceDecayConstant_between_ar;
  int m_RecomputeCovarianceInterval;
  int m_Counter;
  int m_dps;
  bool m_UseMeanEnergy;

  double m_MinimumWithinEigenValue;
  double m_MinimumBetweenEigenValue;
  double m_CurrentWithinEnergy;
  double m_CurrentBetweenEnergy;

  // std::shared_ptr<std::vector<double>> m_MinimumWithinResidualEigenValues;
  // std::shared_ptr<std::vector<double>> m_MinimumBetweenResidualEigenValues;
  // std::shared_ptr<std::vector<double>> m_CurrentWithinResidualEnergies;
  // std::shared_ptr<std::vector<double>> m_CurrentBetweenResidualEnergies;
  std::vector<double> m_MinimumWithinResidualEigenValues;
  std::vector<double> m_MinimumBetweenResidualEigenValues;
  std::vector<double> m_CurrentWithinResidualEnergies;
  std::vector<double> m_CurrentBetweenResidualEnergies;

  std::shared_ptr<std::vector<vnl_matrix_type>> m_InverseCovMatricesAllWithin;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_points_meanAllWithin;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_PointsUpdateAllWithin; // gradient of Cov matrix of all organs
  std::shared_ptr<std::vector<vnl_matrix_type>> m_InverseCovMatricesAllBetween;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_points_meanAllBetween;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_PointsUpdateAllBetween; // stores gradient for between optimization
  std::shared_ptr<vnl_matrix_type> m_super_matrix; // M X 3N matrix
  std::shared_ptr<vnl_matrix_type> m_between_loadings;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_within_loadings;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_within_scores;
  std::shared_ptr<std::vector<vnl_matrix_type>> m_between_scores; // each idx points to each organ params
  std::shared_ptr<std::vector<vnl_matrix_type>> m_within_space_mean;
  std::shared_ptr<vnl_matrix_type> m_between_space_mean;
  std::shared_ptr<vnl_matrix_type> m_grand_mean;
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleMlpcaEntropyFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleMlpcaEntropyFunction.txx"
#endif

#include "itkParticleEnsembleMlpcaEntropyFunction.txx"

#endif
