#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"
#include "InvertibleNetwork.h"
#include <torch/script.h>

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
      // std::cout << "Before Position Set Callback 0" <<  std::endl;
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
      }
    
    if (this->m_optimizing_check && this->m_inv_net->GetModelInitialized())
    {
      vnl_matrix<double> shape_vec_new = this->get_n_columns(d/this->m_DomainsPerShape, 1); // shape: dM X 1
      unsigned int dM = this->rows();
      std::cout << "Setting Z0 New " << dM << "shape vec size " << shape_vec_new.rows() << shape_vec_new.cols() << std::endl;

      try
      {
        torch::Tensor shape_vec_new_tensor = torch::from_blob(shape_vec_new.data_block(), {1,dM});
        std::cout << "Tensor input size " << shape_vec_new_tensor.sizes() << std::endl;
        torch::Tensor z0_particles = this->m_inv_net->ForwardPass(shape_vec_new_tensor);
        for (unsigned int i = 0; i < VDimension; i++)
        {
          double z0_value = z0_particles[0][i+k].item<double>();
          this->m_BaseShapeMatrix->put(i+k, d / this->m_DomainsPerShape, z0_value);
        }
      }
      catch (const c10::Error& e) {
        std::cerr << "Error in LibTorch Operations in Particle Set Callback | " << e.what() << "\n";
      }
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

  void SetOptimizingValue(bool value)
  {
    this->m_optimizing_check = value;
  }

  bool GetOptimizingValue()
  {
    return this->m_optimizing_check;
  }


  void Initialize()
  {
    m_BaseShapeMatrix->fill(0.0);
  }
  
  virtual void BeforeIteration()
  {
    std::cout << "Training Update counter value = " << m_UpdateCounter << std::endl;
    m_UpdateCounter ++;
    if (m_UpdateCounter >= m_NonLinearTrainingInterval)
      {
        m_UpdateCounter = 0;
        if (this->m_inv_net->GetModelInitialized())
        {
          vnl_matrix<T> tmp(*this); // copy existing  matrix
          unsigned int dM = this->rows();
          torch::Tensor sm;
          try{ sm = torch::from_blob(tmp.data_block(), {dM,dM});} catch (const c10::Error& e){ std::cerr << "Errors in SM init | " << e.what() << "\n";}
          this->m_inv_net->TrainModel(sm);
        }
      }
  }

  void SetNonLinearTrainingInterval( int i)
  {    m_NonLinearTrainingInterval = i;  }
  int GetNonLinearTrainingInterval() const
  { return m_NonLinearTrainingInterval; }

  void LoadInvNet( std::string filename)
  {    
    if (!filename.empty())
    {
      this->m_inv_net->LoadParams(filename);
    }
  }

  void SetBaseDistParams( std::shared_ptr<vnl_matrix<double>>& vnl_mean, std::shared_ptr<vnl_matrix<double>>& vnl_cov)
  {
    try
    {
      unsigned int dM = vnl_cov->rows();
      torch::Tensor mean = torch::from_blob(vnl_mean->data_block(), {1,dM});
      torch::Tensor cov = torch::from_blob(vnl_cov->data_block(), {dM,dM});
      this->m_inv_net->SetBaseDistMean(mean);
      this->m_inv_net->SetBaseDistVar(cov);
    }
    catch (const c10::Error& e) {
      std::cerr << "Errors in Libtorch operations while updating base distribution params | " << e.what() << "\n";
    }
  }

  void DoForwardPass(torch::Tensor& input_tensor, double& log_det_jacobian_val, double& p_z_0_val)
  {
    this->m_inv_net->ForwardPass(input_tensor, log_det_jacobian_val, p_z_0_val);
  }


protected:
  ParticleShapeMatrixAttributeNonLinear() 
  {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
    
    m_UpdateCounter = 0;
    m_NonLinearTrainingInterval = 10;
    m_BaseShapeMatrix = std::make_shared<vnl_matrix<double>>();
    m_inv_net = std::make_shared<InvertibleNet::Model>();
    m_optimizing_check = false;
  }

  virtual ~ParticleShapeMatrixAttributeNonLinear() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent);  }

private:
  ParticleShapeMatrixAttributeNonLinear(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_UpdateCounter;
  int m_NonLinearTrainingInterval;
  bool m_optimizing_check = true;
  std::shared_ptr<vnl_matrix<double>> m_BaseShapeMatrix;
  InvertibleNet::Model::Pointer m_inv_net;
};

} // end namespace
