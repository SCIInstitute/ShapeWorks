#pragma once

#include <kernels/CSRBFKernel.h>
#include <cmath>

class TruncatedGaussianKernel : public CSRBFKernel {
 public:
  CUDA_ENABLED TruncatedGaussianKernel(double radius = 1.0) :
      CSRBFKernel(RBFKernelType::TruncatedGaussian, radius) {}
  CUDA_ENABLED double compute(double r) const override{
    // TODO: Restructure logic below to avoid conditional.
    return r > 10.0*m_radius ? 0.0 : std::exp(-r*r / (m_radius*m_radius));
  }
  CUDA_ENABLED float compute(float r) const override{
    // TODO: Restructure logic below to avoid conditional.
    float m_radiusf = (float)m_radius;
    return r > 10.0*m_radiusf ? 0.0 : std::exp(-r*r / (m_radiusf*m_radiusf));
  }
};
