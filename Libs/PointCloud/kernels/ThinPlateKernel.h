#pragma once

#include <kernels/RBFKernel.h>
#include <cmath>

class ThinPlateKernel : public RBFKernel {
 public:
  CUDA_ENABLED ThinPlateKernel() : RBFKernel(RBFKernelType::ThinPlate){}
  CUDA_ENABLED double compute(double r) const override {
    return r*r*log(r + c);
  }
  CUDA_ENABLED float compute(float r) const override {
    return r*r*log(r + (float)c);
  }
  CUDA_ENABLED double compute_gradient(double r) const override{
    double rc = r+c;
    return (r/rc + 2*log(rc));
  }
private:
  const double c = 0.1;
};
