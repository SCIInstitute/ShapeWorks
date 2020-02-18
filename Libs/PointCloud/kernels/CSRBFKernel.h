#pragma once

#include <kernels/RBFKernel.h>

/**
 * A compact support kernel base class.
 * These kernels have a limited radius.
 */
class CSRBFKernel : public RBFKernel{
public:
  CUDA_ENABLED CSRBFKernel(RBFKernelType type, double radius) :
      RBFKernel(type), m_radius(radius){}
  CUDA_ENABLED virtual ~CSRBFKernel(){}
  CUDA_ENABLED virtual double compute(double r) const = 0;
  CUDA_ENABLED virtual float compute(float r) const = 0;
  double getRadius() const { return m_radius; }
  void setRadius(double r) { m_radius = r; }
protected:
  double m_radius;
};
