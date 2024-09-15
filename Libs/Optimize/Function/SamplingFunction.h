#pragma once

#include <vector>

#include "Libs/Optimize/Container/GenericContainerArray.h"
#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "VectorFunction.h"

namespace shapeworks {

class SamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;

  /** Standard class typedefs. */
  typedef SamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(SamplingFunction, VectorFunction);

  /** Data type representing individual gradient components. */
  typedef float GradientNumericType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector_fixed<float, 3> GradientVectorType;

  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxdt) const {
    return {};
  };

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxdt,
                              double& energy) const {
    itkExceptionMacro("This method not implemented");
    return VectorType();
  }
  virtual double Energy(unsigned int, unsigned int, const ParticleSystem*) const {
    itkExceptionMacro("This method not implemented");
    return 0.0;
  }

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma(unsigned int idx, const typename ParticleSystem::PointVectorType& neighborhood,
                               const shapeworks::ParticleDomain* domain, const std::vector<double>& weights,
                               const PointType& pos, double initial_sigma, double precision, int& err) const {
    return 0.0;
  };

  virtual VectorFunction::Pointer Clone() {}

 protected:
  SamplingFunction() {}
  virtual ~SamplingFunction() {}
  void operator=(const SamplingFunction&);
  SamplingFunction(const SamplingFunction&);
};

}  // namespace shapeworks
