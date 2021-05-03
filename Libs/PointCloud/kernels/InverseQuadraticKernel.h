#pragma once

#include <kernels/RBFKernel.h>
#include <cmath>

class InverseQuadraticKernel : public RBFKernel {
 public:
  CUDA_ENABLED InverseQuadraticKernel() :
      RBFKernel(RBFKernelType::InverseQuadratic){}
  CUDA_ENABLED double compute(double r) const override {
    return 1.0/(r*r + c*c);
  }
  CUDA_ENABLED float compute(float r) const override {
    float cf = (float)c;
    return 1.0/(r*r + cf*cf);
  }
 private:
  const double c = 0.1;
};
