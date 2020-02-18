#pragma once

#include <kernels/CSRBFKernel.h>
#include <cmath>

class GaussianKernel : public CSRBFKernel {
 public:
  CUDA_ENABLED GaussianKernel(double radius = 1.0) :
      CSRBFKernel(RBFKernelType::Gaussian, radius) {}
  CUDA_ENABLED double compute(double r) const {
    return std::exp(-r*r / (m_radius*m_radius));
  }
  CUDA_ENABLED float compute(float r) const {
    float m_radiusf = (float)m_radius;
    return std::exp(-r*r / (m_radiusf*m_radiusf));
  }
};
