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
  using Tensor = torch::Tensor;

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
   //pass
  }
  
  virtual void BeforeIteration()
  {
    m_UpdateCounter ++;
    //pass
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

  void ForwardPass(Tensor& input_tensor, Tensor& output_tensor, Tensor& jacobian_matrix, double& log_prob_u, double& log_det_g, double& log_det_j)
  {
    this->m_inv_net->ForwardPass(input_tensor, output_tensor, 
                                                    jacobian_matrix, log_prob_u, 
                                                    log_det_g, log_det_j);
  };

  void ComputeEnergy(Tensor& input_tensor, double& energy_in_data_space)
  {
    this->m_inv_net->ComputeEnergy(input_tensor, energy_in_data_space);
  };

  torch::Device GetDevice()
  {
    return this->m_inv_net->GetDevice();
  };

  int GetLatentDimensions()
  {
    return this->m_inv_net->GetLatentDimensions();
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
  InvertibleNet::Model::Pointer m_inv_net;
};

} // end namespace
