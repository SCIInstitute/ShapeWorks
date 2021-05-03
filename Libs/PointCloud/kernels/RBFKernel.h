#pragma once

#include "RBFKernelType.h"

#ifdef __CUDACC__
#define CUDA_ENABLED __host__ __device__
#else
#define CUDA_ENABLED
#endif

/**
 * The abstract base class of all rbf kernels. The RBFKernelType must
 * be provided by any inheriting kernel class so that CUDA device code
 * can inspect and instantiate a local kernel in device memory.
 */
class RBFKernel {
public:
  CUDA_ENABLED RBFKernel(RBFKernelType type) : m_type(type){}
  CUDA_ENABLED virtual ~RBFKernel(){}
  CUDA_ENABLED virtual double compute(double r) const {return 42;}
  CUDA_ENABLED virtual float compute(float r) const {return 42;}
  CUDA_ENABLED virtual double compute_gradient(double r) const {return 42;}
  CUDA_ENABLED RBFKernelType getType() const { return m_type; }
protected:
  RBFKernelType m_type;
};
