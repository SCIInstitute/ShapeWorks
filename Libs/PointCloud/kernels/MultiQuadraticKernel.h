#pragma once

#include <kernels/RBFKernel.h>
#include <cmath>

class MultiQuadraticKernel : public RBFKernel {
 public:
  CUDA_ENABLED MultiQuadraticKernel() :
      RBFKernel(RBFKernelType::MultiQuadratic){}
  CUDA_ENABLED double compute(double r) const override {
    return sqrt(r*r + c*c);
  }
  CUDA_ENABLED float compute(float r) const override {
    float cf = (float)c;
    return sqrt(r*r + cf*cf);
  }
 private:
  const double c = 0.1;
};
