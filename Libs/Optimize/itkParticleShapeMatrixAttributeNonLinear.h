#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkParticleSystem.h"
#include "InvertibleNetwork.h"
#include <torch/script.h>
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include <c10/cuda/CUDACachingAllocator.h>

#define MSG(x) do { std::cerr << x << std::endl; } while (0)
#define DEBUG(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)

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
      MSG("Position Update, Setting Base");
      try
      {
        auto ten_options = torch::TensorOptions().dtype(torch::kDouble);
        torch::Tensor z = torch::from_blob(shape_vec_new.data_block(), {1,dM}, ten_options);
        z.to(torch::kFloat);
        auto l_dim = this->m_inv_net->GetLatentDimensions();
        torch::Tensor u = torch.zeros({1, l_dim});
        this->m_inv_net->ForwardPass(z, u);
        unsigned int l_dim = u.sizes()[1]; // 1 X L
        for (unsigned int i = 0; i < l_dim; i++)
        {
          double z0_value = u[0][l].item<double>();
          this->m_BaseShapeMatrix->put(l, d / this->m_DomainsPerShape, z0_value);
        }
      }
      catch (const c10::Error& e) {
        std::cerr << "Error in LibTorch Operations in Particle Set Callback | " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
      }
      // c10::cuda::CUDACachingAllocator::emptyCache();
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
    MSG("Before Iteration | Init. Latent Space"); DEBUG(m_UpdateCounter);
    m_UpdateCounter ++;
    {
      torch::Tensor sm;
      vnl_matrix<T> tmp(*this); // copy existing  matrix
      unsigned int dM = this->rows();
      unsigned int N = this->cols();
      tmp = tmp.transpose(); // N X dM
      try{
        auto ten_options = torch::TensorOptions().dtype(torch::kDouble);
        auto x = torch::from_blob(tmp.data_array(), {N, dM}, ten_options);
        sm = x.to(torch::kFloat); // make model compatible float
        auto l_dim = this->m_inv_net->GetLatentDimensions();
        torch::Tensor u = torch.zeros({N, l_dim});
        // Forward pass
        this->m_inv_net->ForwardPass(sm, u);
        u = u.transpose();
        double* u_data_ptr = u.data_ptr<double>();
        m_BaseShapeMatrix->set_size(l_dim, N);
        m_BaseShapeMatrix->clear();
        m_BaseShapeMatrix->set(u_data_ptr);
        MSG("Latent Space initialized");
        } 
        catch (const c10::Error& e){ 
          std::cerr << "Errors in Shape Matrix initialization in Before Iteration | " << e.what() << "\n";
          std::exit(EXIT_FAILURE); 
        }
    }
    // c10::cuda::CUDACachingAllocator::emptyCache();
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


  void RunForwardPassWithJacbian(double& log_prob_u, double& log_det_g, double& log_det_j, torch::Tensor& input_tensor, torch::tensor& jacobian_matrix)
  {
    // Forward pass to be called from entropy function
    this->m_inv_net->ForwardPassGrad(input_tensor, jacobian_matrix, log_prob_u, log_det_g, log_det_j);
  }

  torch::Device GetDevice()
  {
    this->m_inv_net->GetDevice();
  };

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
