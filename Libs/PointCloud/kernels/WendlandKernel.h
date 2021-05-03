#pragma once

#include <kernels/CSRBFKernel.h>
#include <cmath>

class WendlandKernel : public CSRBFKernel {
 public:
  CUDA_ENABLED WendlandKernel(double radius = 1.0) :
      CSRBFKernel(RBFKernelType::Wendland, radius) {}
  CUDA_ENABLED double compute(double r) const override {
    // shireen: for wendland compact basis
    double a = 3.0 * sqrt(3.14 / 2.0) * m_radius;
    double nr = (double)r / a; // the support of the basis is only defined till 2.5*kernelRadius
    double nr1 = 1 - nr;
    return nr > 1 ? 0.0 : ((nr1)*(nr1) *(nr1) *(nr1) * (4.0*nr + 1));
  }
  CUDA_ENABLED float compute(float r) const override {
    // shireen: for wendland compact basis
    float a = 3.0 * sqrt(3.14 / 2.0) * m_radius;
    float nr = r / a; // the support of the basis is only defined till 2.5*kernelRadius
    float nri = 1-nr;
    return nr > 1 ? 0.0 : ((nri)*(nri) *(nri) *(nri) * (4.0*nr + 1));
  }
};
