#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"
#include <torch/script.h> // One-stop header.

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
    // Get shape vector for this domain
    vnl_matrix<double> shape_vec_new = this->get_n_columns(d/this->m_DomainsPerShape, 1); // shape: dM X 1
    unsigned int dM = this->rows();
    torch::Tensor z0_particles;
    try
    {
      // Pass through invertible network
      torch::NoGradGuard no_grad;
      torch::Tensor shape_vec_new_tensor = torch::from_blob(shape_vec_new.data_block(), {1,dM});
      std::vector<torch::jit::IValue> inputs;
      inputs.push_back(shape_vec_new_tensor.to(this->m_device));
      auto outputs = this->m_module.forward(inputs).toTuple();
      // std::cout << "Update Particles, Forward pass done " << std::endl;
      z0_particles = outputs->elements()[0].toTensor();
      z0_particles = z0_particles.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
    }
    catch (const c10::Error& e) {
      std::cerr << "Error in Libtorch Operations in Particle Set Callback\n";
    }

    for (unsigned int i = 0; i < VDimension; i++)
      {
        double z0_value = z0_particles[0][i+k].item<double>();
        this->m_BaseShapeMatrix->put(i+k, d / this->m_DomainsPerShape, z0_value);
      }
  }
  
  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    // NEED TO IMPLEMENT THIS
  }
  
  void SetNonLinearTrainModelCallbackFunction(const std::function<void(void)> &f){
    this->m_NonLinearTrainModelCallback = f;
  }

  int LoadPytorchModel(const std::string model_path, int gpu_id){
    try {
      if (gpu_id == -1){
        this->m_module = torch::jit::load(model_path);
        this->m_device = torch::Device(torch::kCPU);
      }
      else{
        this->m_module = torch::jit::load(model_path);
        this->m_device = torch::Device(torch::kCUDA, gpu_id);
      }
      this->m_gpu_id = gpu_id;
      this->m_module.to(this->m_device);
    }
    catch (const c10::Error& e) {
      std::cerr << "Error loading the model \n";
      return -1;
    }
    std::cout << "***** Pytorch Model Loaded successfully in SW *****" << std::endl;
    return 0;
  }

  torch::jit::script::Module GetInvertibleNetwork()
  {
    return this->m_module;
  }

  torch::Device GetDeviceType(){
    return this->m_device;
  }

  int GetGpuId()
  {
    return this->m_gpu_id;
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
    std::cout << "Training Update counter value = " << m_UpdateCounter << std::endl;
    m_UpdateCounter ++;
    if (m_UpdateCounter >= m_NonLinearTrainingInterval)
      {
      m_UpdateCounter = 0;
      if (this->m_NonLinearTrainModelCallback)
      {
        std::cout << "Before Training Callback C++ 0" << std::endl;
        this->m_NonLinearTrainModelCallback();
      }
      else{
        std::cout << "callback not initialized yet" << std::endl;
      }
      std::cout << "After Training Callback C++ 1" << std::endl;
      }

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
    m_BaseShapeMatrix = std::make_shared<vnl_matrix<double>>();
  }

  virtual ~ParticleShapeMatrixAttributeNonLinear() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent);  }

private:
  ParticleShapeMatrixAttributeNonLinear(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_UpdateCounter;
  int m_NonLinearTrainingInterval;
  int m_gpu_id;

  // Callbacks from Python
  std::function<void(void)> m_NonLinearTrainModelCallback;

  // Z_0 --> Base Distribution
  std::shared_ptr<vnl_matrix<double>> m_BaseShapeMatrix;
  torch::jit::script::Module m_module;
  torch::Device m_device = torch::kCPU;
};

} // end namespace
