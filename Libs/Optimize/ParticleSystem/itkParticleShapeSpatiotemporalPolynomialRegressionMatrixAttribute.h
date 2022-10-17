#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"


namespace itk
{
/** \class itkParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute
 *
 *
 *
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute
  : public ParticleShapeMatrixAttribute<T,VDimension>
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute Self;
  typedef ParticleShapeMatrixAttribute<T,VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(itkParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute, ParticleShapeMatrixAttribute);
  
  
  void UpdateMeanMatrix()
  {
    // New g(r)
    //TODO: Sanity Check
    int beta_cols_size = m_polynomial_degree + 1;
    std::cout << "Updating mean matrix m_polynomial_degree = " << m_polynomial_degree << std::endl;
    // b_0, b_1, b_2 ... each vec of size dM
    unsigned int N = m_MeanMatrix->cols();
    for (unsigned int i = 0; i < N; ++i)
      {
      // compute the mean
      vnl_vector<double> vec; // dM
      for (unsigned int p = 0 ; p < beta_cols_size; ++p){
        vec += (std::pow(m_Expl(i), p) * m_Betas->get_column(p));
      }
      m_MeanMatrix->set_column(i, vec);
      }
      std::cout << "-----Mean Matrix Updated in ShapeMatrixAttribute ----- " << std::endl;
  }

  std::shared_ptr<vnl_matrix<double>> GetMeanMatrix(){
    return this->m_MeanMatrix;
  }

  std::shared_ptr<vnl_matrix<double>> GetRegressionParametersMatrix(){
    return this->m_Betas;
  }
  
  void ResizeParameters(unsigned int n, int degree)
  {
    // Resize after change in particle size
    // std::cout << "Inside ResizeParameters 0 n = " << n << std::endl;
    
    vnl_matrix<double> tmp(*m_Betas); // copy existing  matrix
    // std::cout << "Inside ResizeParameters 0.1 " << std::endl;
    
    int beta_cols = m_Betas->cols(); // P + 1
    if (beta_cols != (m_polynomial_degree + 1)){
      std::cerr << "Polynomial Degree doesn't match with Beta Matrix Colsize" << std::endl;
    }
    // Create new 
    m_Betas->set_size(n, beta_cols);
    // Copy old data
    for (unsigned int r = 0; r < tmp.rows(); r++)
    {
      for (unsigned int c = 0; c < tmp.cols(); c++)
      {
          m_Betas->put(r, c, tmp(r, c));
      }
    }
    // std::cout << "Inside ResizeParameters 1 " << std::endl;
    
  }
  
  virtual void ResizeMeanMatrix(int rs, int cs)
  {
    // std::cout << "Inside Resize Mean Matrix 0 rs, cs = " << rs << " " << cs <<  std::endl;
    vnl_matrix<T> tmp(*m_MeanMatrix); // copy existing  matrix
    // std::cout << "Inside Resize Mean Matrix 0.1" << std::endl;
    
    // Create new column (shape)
    m_MeanMatrix->set_size(rs, cs);
    
    m_MeanMatrix->fill(0.0);
    
    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++)
      {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
        m_MeanMatrix->put(r,c, tmp(r,c)); 
        }
      }
    // std::cout << "Inside Resize Mean Matrix 1" << std::endl;
    
  }

  void ResizeExplanatory(unsigned int n)
  {
    // std::cout << "Inside ResizeExplanatory 0 n = " << n << std::endl;
    if (n > m_Expl.size())
      {
      vnl_vector<double> tmp = m_Expl; // copy existing  matrix
      
      // Create new 
      m_Expl.set_size(n);
      m_Expl.fill(0.0);
      
      // Copy old data into new vector.
      for (unsigned int r = 0; r < tmp.size(); r++)
        {
        m_Expl(r) = tmp(r);
        }
      }
    // std::cout << "Inside ResizeExplanatory 1" << std::endl;

  }  
  
  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object *, const EventObject &e)
  {
    // std::cout << "inside Domain add in ShapeMatrixAttribute 0 " << std::endl;
    const itk::ParticleDomainAddEvent &event
      = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);
    unsigned int d = event.GetDomainIndex();
    
    if ( d % this->m_DomainsPerShape  == 0 )
      {
      this->ResizeMatrix(this->rows(), this->cols()+1); // for shapeMatrixAttribute
      // std::cout << "ShapeMatrix Resized " << std::endl;

      this->ResizeMeanMatrix(this->rows(), this->cols()+1);
      this->ResizeExplanatory(this->cols());
      }
    // std::cout << "inside Domain add in ShapeMatrixAttribute 1" << std::endl;

  }
  
  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 
  {
    // std::cout << "inside position add in ShapeMatrixAttribute 0" << std::endl;
    const itk::ParticlePositionAddEvent &event
      = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
    const itk::ParticleSystem *ps
      = dynamic_cast<const itk::ParticleSystem *>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename itk::ParticleSystem::PointType pos
      = ps->GetTransformedPosition(idx, d);
    
    const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
    
    // Make sure we have enough rows.
    if ((ps->GetNumberOfParticles(d) * VDimension * this->m_DomainsPerShape)
        > this->rows())
      {
      this->ResizeParameters(PointsPerDomain * VDimension * this->m_DomainsPerShape, this->m_polynomial_degree);
      this->ResizeMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                         this->cols());
      this->ResizeMeanMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                             this->cols());
      }
    
    // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN
    // Add position info to the matrix
    unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
      + (idx * VDimension);
    for (unsigned int i = 0; i < VDimension; i++)
      {
      this->operator()(i+k, d / this->m_DomainsPerShape) = pos[i];
      }
    // std::cout << "inside position add in ShapeMatrixAttribute 1 " << std::endl;
    
    //   std::cout << "Row " << k << " Col " << d / this->m_DomainsPerShape << " = " << pos << std::endl;
  }
  
  virtual void PositionSetEventCallback(Object *o, const EventObject &e) 
  {
    // std::cout << "inside position set in ShapeMatrixAttribute 0" << std::endl;
    const itk::ParticlePositionSetEvent &event
      = dynamic_cast <const itk::ParticlePositionSetEvent &>(e);
  
    const itk::ParticleSystem *ps
      = dynamic_cast<const itk::ParticleSystem *>(o);
    const int d = event.GetDomainIndex();
    const unsigned int idx = event.GetPositionIndex();
    const typename itk::ParticleSystem::PointType pos = ps->GetTransformedPosition(idx, d);
    const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
    
    // Modify matrix info
    //    unsigned int k = VDimension * idx;
    unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
      + (idx * VDimension);    
    
    for (unsigned int i = 0; i < VDimension; i++)
      {
      this->operator()(i+k, d / this->m_DomainsPerShape) =
        pos[i] - m_MeanMatrix->get(i+k, d/ this->m_DomainsPerShape);
      }
    // std::cout << "inside position set in ShapeMatrixAttribute 1" << std::endl;

  }
  
  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    // NEED TO IMPLEMENT THIS
  }
  
  /** Set/Get the number of domains per shape.  This can only be safely done
      before shapes are initialized with points! */
  void SetDomainsPerShape(int i)
  { this->m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return this->m_DomainsPerShape; }

  void SetExplanatory(std::vector<double> v)
  {
    // take input from opt file and set time points for corresponding subjects
    //    std::cout << "Setting expl " << std::endl;
    ResizeExplanatory(v.size());
    for (unsigned int i = 0; i < v.size(); i++)
      {
      //      std::cout << v[i] << std::endl;
      m_Expl[i] = v[i];
      }
    m_N = v.size();
  }

  void SetPolynomialDegree(int degree){
    this->m_polynomial_degree = degree;
  }
  void SetExplanatory(unsigned int i, double q)
  { m_Expl[i] = q; }
  const double &GetExplanatory(unsigned int i) const
  { return m_Expl[i]; }
  double &GetExplanatory(unsigned int i)
  { return m_Expl[i]; }
 
  // 
  void Initialize()
  {
    //TODO: make shared ?
    std::cout << "Initializing shape matrix poly regression" << std::endl;
    m_MeanMatrix->fill(0.0);
    m_Betas->fill(0.0);
  }
  
  virtual void BeforeIteration()
  {
    // std::cout << "-----Before Iteration in ShapeMatrixAttribute 0 ------" << std::endl;
    m_UpdateCounter ++;
    if (m_UpdateCounter >= m_RegressionInterval)
      {
      m_UpdateCounter = 0;
      // this->EstimateParameters();
      //Get Parameters from python
      this->UpdateMeanMatrix();
      std::cout << "-----Before Iteration in ShapeMatrixAttribute 1 (MEAN UDPATED)------" << std::endl;
      }
  }

  void SetRegressionInterval( int i)
  {    m_RegressionInterval = i;  }
  int GetRegressionInterval() const
  { return m_RegressionInterval; }
  
protected:
  ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute() 
  {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
    m_UpdateCounter = 0;
    m_RegressionInterval = 1;
    m_polynomial_degree = 10;
    m_Betas = std::make_shared<vnl_matrix<double>>(); // dM X (P+1)
    m_MeanMatrix = std::make_shared<vnl_matrix<double>>(); // dM X N
    m_N = 1;
  }
  virtual ~ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent);  }

private:
  ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_UpdateCounter;
  int m_RegressionInterval;
  int m_polynomial_degree;
  unsigned int m_N;

  // Parameters for the polynomial regression model using LASSO
  std::shared_ptr<vnl_matrix<double>> m_Betas; // dM X (P+1)

  // The explanatory variable value for each sample (matrix column)
  vnl_vector<double> m_Expl; // N

  // A matrix to store the mean estimated for each explanatory variable (each sample)
  std::shared_ptr<vnl_matrix<double>> m_MeanMatrix; // dM X N

};

} // end namespace
