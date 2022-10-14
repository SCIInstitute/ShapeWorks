#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

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
    // for each sample
    //TODO: Sanity Check
    for (unsigned int i = 0; i < m_MeanMatrix.cols(); i++)
      {
      // compute the mean
      vnl_vector<double> vec;
      for (t = 0 ; t < T; t++){
        vec += pow(m_Expl(i), t) * m_betas.col(i)
      }
      m_MeanMatrix.set_column(i, vec);
      }
  }
  
  
  void ResizeParameters(unsigned int n)
  {
    // Resize after change in particle size
    vnl_matrix<double> tmp = m_Betas; // copy existing  matrix
    unsigned int T = m_Betas.cols();
    // Create new 
    m_Betas.set_size(n, T);
    // Copy old data
    for (unsigned int r = 0; r < tmp.rows(); r++)
    {
      for (unsigned int c = 0; c < tmp.cols(); c++)
      {
          m_Betas(r, c) = tmp(r, c);
      }
    }
  }
  
  virtual void ResizeMeanMatrix(int rs, int cs)
  {
    // TODO: sanity check
    vnl_matrix<T> tmp = m_MeanMatrix; // copy existing  matrix
    
    // Create new column (shape)
    m_MeanMatrix.set_size(rs, cs);
    
    m_MeanMatrix.fill(0.0);
    
    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++)
      {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
        m_MeanMatrix(r,c) = tmp(r,c);
        }
      } 
  }

  void ResizeExplanatory(unsigned int n)
  {
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
  }  
  
  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object *, const EventObject &e)
  {
    const itk::ParticleDomainAddEvent &event
      = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);
    unsigned int d = event.GetDomainIndex();
    
    if ( d % this->m_DomainsPerShape  == 0 )
      {
      this->ResizeMatrix(this->rows(), this->cols()+1); // for shapeMatrixAttribute
      this->ResizeMeanMatrix(this->rows(), this->cols()+1);
      this->ResizeExplanatory(this->cols());
      }    
  }
  
  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 
  {
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
      this->ResizeParameters(PointsPerDomain * VDimension * this->m_DomainsPerShape);
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
    
    //   std::cout << "Row " << k << " Col " << d / this->m_DomainsPerShape << " = " << pos << std::endl;
  }
  
  virtual void PositionSetEventCallback(Object *o, const EventObject &e) 
  {
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
        pos[i] - m_MeanMatrix(i+k, d/ this->m_DomainsPerShape);
      }
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
  }
  void SetExplanatory(unsigned int i, double q)
  { m_Expl[i] = q; }
  const double &GetExplanatory(unsigned int i) const
  { return m_Expl[i]; }
  double &GetExplanatory(unsigned int i)
  { return m_Expl[i]; }

  const vnl_matrix<double> &GetBetas() const
  { return m_Betas; }

  void SetBetas(const vnl_matrix<double> &m)
  {
    ResizeParameters(m.rows());
    for (unsigned int r = 0; r < m.rows(); ++r)
    {
      for (unsigned int c = 0; c < m.cols(); ++c)
      {
        m_Betas(r, c) = m(r, c);
      }
    }
  }

  void GetOptimumAlphaValue()
  {
    py::module sw = py::module::import("shapeworks");
    py::object compute = sw.attr("polynomial_regression").attr("get_optimum_alpha");
    vnl_matrix<double> X = *this + m_MeanMatrix;
    // x to eigen 
    // m_exp tp eig vec
    double val = compute(X, X).cast<Eigen::MatrixXd>();
    m_alpha_value = val;
    m_alpha_estimated = true;
  }

    void EstimateParameters()
  {
    py::module sw = py::module::import("shapeworks");
    py::object compute = sw.attr("polynomial_regression").attr("estimate_parameters");
    vnl_matrix<double> X = *this + m_MeanMatrix;
    // x to eigen 
    // m_exp tp eig vec
    Eigen::MatrixXd betas_eigen = compute(X, X, alpha_value).cast<Eigen::MatrixXd>();
  }
  
  // 
  void Initialize()
  {
    m_MeanMatrix.fill(0.0);
    m_Betas.fill(0.0);
  }
  
  virtual void BeforeIteration()
  {
    m_UpdateCounter ++;
    if (m_UpdateCounter >= m_RegressionInterval)
      {
      m_UpdateCounter = 0;
      if (!m_alpha_estimated){
        this->GetOptimumAlphaValue();
      }
      this->EstimateParameters();
      this->UpdateMeanMatrix();
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
    m_alpha_value = 0.000005;
    m_alpha_estimated = false;
  }
  virtual ~ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent);  }

private:
  ParticleShapeSpatiotemporalPolynomialRegressionMatrixAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_UpdateCounter;
  int m_RegressionInterval;

  // Parameters for the polynomial regression model using LASSO
  vnl_matrix<double> m_betas;
  double m_alpha_value;
  bool m_alpha_estimated;

  // The explanatory variable value for each sample (matrix column)
  vnl_vector<double> m_Expl;

  // A matrix to store the mean estimated for each explanatory variable (each sample)
  vnl_matrix<double> m_MeanMatrix; // g(r)
};

} // end namespace
