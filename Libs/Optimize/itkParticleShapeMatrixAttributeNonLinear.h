#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"

namespace itk
{
/** \class ParticleShapeMatrixAttributeNonLinear
 *
 *
 *
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT ParticleShapeMatrixAttributeNonLinear
  : public ParticleShapeMatrixAttribute<T,VDimension>
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef ParticleShapeMatrixAttributeNonLinear Self;
  typedef ParticleShapeMatrixAttribute<T,VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleShapeMatrixAttributeNonLinear, ParticleShapeMatrixAttribute);
  
  std::shared_ptr<vnl_matrix<double>> GetBaseShapeMatrix(){
    return this->m_BaseShapeMatrix;
  }
  
  std::shared_ptr<vnl_matrix<double>> GetJacobianMatrix(){
    return this->m_JacobianMatrix_det;
  }

  std::shared_ptr<vnl_matrix<double>> GetDifferenceMatrix(){
    return this->m_DifferenceMatrix;
  }
  
  virtual void ResizeBaseShapeMatrix(int rs, int cs)
  {
    vnl_matrix<T> tmp(*m_BaseShapeMatrix); // copy existing  matrix 
    // Create new column (shape)
    m_BaseShapeMatrix->set_size(rs, cs);
    m_BaseShapeMatrix->fill(0.0);
    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++)
      {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
        m_BaseShapeMatrix->put(r,c,  tmp(r,c));
        }
      } 
  }

  virtual void ResizeDifferenceMatrix(int rs, int cs)
  {
    vnl_matrix<T> tmp(*m_BaseShapeMatrix); // copy existing  matrix 
    // Create new column (shape)
    m_DifferenceMatrix->set_size(rs, cs);
    m_DifferenceMatrix->fill(0.0);
    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++)
      {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
        m_DifferenceMatrix->put(r,c,  tmp(r,c));
        }
      } 
  }

  virtual void ResizeJacobianMatrix(int cs)
  {
    vnl_matrix<T> tmp(*m_JacobianMatrix_det); // copy existing  matrix
    // Create new column (shape)
    m_JacobianMatrix_det->set_size(0, cs);
    m_JacobianMatrix_det->fill(0.0);    
    // Copy old data into new matrix.
    for (unsigned int c = 0; c < tmp.cols(); c++)
        {
        m_JacobianMatrix_det->put(0,c, tmp(0, c));
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
      this->ResizeMatrix(this->rows(), this->cols()+1);
      this->ResizeBaseShapeMatrix(this->rows(), this->cols()+1);
      this->ResizeDifferenceMatrix(this->rows(), this->cols()+1);
      this->ResizeJacobianMatrix(this->cols()+1);                      

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
      this->ResizeMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                         this->cols());
      this->ResizeBaseShapeMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                             this->cols());
      this->ResizeDifferenceMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                             this->cols());
      this->ResizeJacobianMatrix(this->cols());                      
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
      this->operator()(i+k, d / this->m_DomainsPerShape) = pos[i];
        // pos[i] - m_MeanMatrix(i+k, d/ this->m_DomainsPerShape);

      }
      // Feed Z space to network and get new base particles
      this->m_UpdateBaseParticlesCallback();
  }
  
  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    // NEED TO IMPLEMENT THIS
  }
  
  void SetNonLinearTrainModelCallbackFunction(const std::function<void(void)> &f){
    this->m_NonLinearTrainModelCallback = f;
  }

  void SetBeforeGradientUpdatesCallbackFunction(const std::function<void(void)> &f){
    this->m_BeforeGradientUpdatesCallback = f;
  }

  void SetUpdateBaseParticlesCallbackFunction(const std::function<void(void)> &f){
    this->m_UpdateBaseParticlesCallback = f;
  }

  /** Set/Get the number of domains per shape.  This can only be safely done
      before shapes are initialized with points! */
  void SetDomainsPerShape(int i)
  { this->m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return this->m_DomainsPerShape; }

  void Initialize()
  {
    m_BaseShapeMatrix->fill(0.0);
  }
  
  virtual void BeforeIteration()
  {
    m_UpdateCounter ++;
    if (m_UpdateCounter >= m_NonLinearTrainingInterval)
      {
      m_UpdateCounter = 0;
      this->m_NonLinearTrainModelCallback();
      }
    this->m_BeforeGradientUpdatesCallback();
  }

  void SetNonLinearTrainingInterval( int i)
  {    m_NonLinearTrainingInterval = i;  }
  int GetNonLinearTrainingInterval() const
  { return m_NonLinearTrainingInterval; }
  
protected:
  ParticleShapeMatrixAttributeNonLinear() 
  {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
    m_UpdateCounter = 0;
    m_NonLinearTrainingInterval = 50;
  }

  virtual ~ParticleShapeMatrixAttributeNonLinear() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent);  }

private:
  ParticleShapeMatrixAttributeNonLinear(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_UpdateCounter;
  int m_NonLinearTrainingInterval;

  // Callbacks from Python
  std::function<void(void)> m_BeforeGradientUpdatesCallback;
  std::function<void(void)> m_NonLinearTrainModelCallback;
  std::function<void(void)> m_UpdateBaseParticlesCallback;

  // Z_0 --> Base Distribution
  std::shared_ptr<vnl_matrix<double>> m_BaseShapeMatrix;
  std::shared_ptr<vnl_matrix<double>> m_JacobianMatrix_det;
  std::shared_ptr<vnl_matrix<double>> m_DifferenceMatrix;



};

} // end namespace