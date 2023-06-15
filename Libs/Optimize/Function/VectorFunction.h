#pragma once

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "Libs/Optimize/ParticleSystem.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

/**
 * \class VectorFunction
 *
 * This is the base class defining the API for a function that takes a particle
 * system, domain, and location index as arguments and returns a vector-valued
 * result.
 *
 */
class VectorFunction : public itk::LightObject {
 public:
  constexpr static unsigned int VDimension = 3;
  /** Standard class typedefs. */
  typedef VectorFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::LightObject Superclass;
  itkTypeMacro(VectorFunction, LightObject);

  /** Type of vectors. */
  typedef vnl_vector_fixed<double, VDimension> VectorType;

  /** Method for object allocation through the factory. */
  //  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep,
                              double& energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystem*) const = 0;

  // Pure Virtual Definitions for Robust SSM Particle and Offset Updates, implemented in selected base classes only
  virtual double EvaluateOffsetGradientMode(unsigned int, unsigned int, const ParticleSystem*, double&, double&) {}
  virtual VectorType EvaluateParticleGradientMode(unsigned int, unsigned int, const ParticleSystem*, double&, double&) {}
  virtual double EvaluateOffsetGradientMode(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const {}
  virtual VectorType EvaluateParticleGradientMode(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) {}
  virtual double EnergyOffsetGradientMode(unsigned int a, unsigned int b, const ParticleSystem* c) {}
  virtual double EnergyParticleGradientMode(unsigned int a, unsigned int b, const ParticleSystem* c) {}
  
  /** May be called by the solver class. */
  virtual void ResetBuffers() {}

  /** This method is called by a solver after each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void AfterIteration() {}

  /** This method is called by a solver before each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void BeforeIteration() {}

  /** This method may be called to set up the state of the function object
      before a call to Evaluate.  It is necessary in order to initialize
      certain constants and variables that may be used for calculating the
      Energy as well as the Gradients.  Typically this is only necessary for
      the adaptive gradient descent algorithm.*/
  virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystem*) {}

  /** Some subclasses may require a pointer to the particle system and its
      domain number.  These methods set/get those values. */
  virtual void SetParticleSystem(ParticleSystem* p) { m_ParticleSystem = p; }
  virtual ParticleSystem* GetParticleSystem() const { return m_ParticleSystem; }
  virtual void SetDomainNumber(unsigned int i) { m_DomainNumber = i; }
  virtual int GetDomainNumber() const { return m_DomainNumber; }

  virtual VectorFunction::Pointer Clone() {
    std::cerr << "Error: base class VectorFunction Clone method called!\n";
    std::cerr << "Threaded run of current parameters not supported!\n";
    return nullptr;
  }

  virtual double GetRelativeEnergyScaling() const { return 1.0; }
  virtual void SetRelativeEnergyScaling(double r) { return; }

 protected:
  VectorFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~VectorFunction() {}
  void operator=(const VectorFunction&);
  VectorFunction(const VectorFunction&);

  ParticleSystem* m_ParticleSystem;
  unsigned int m_DomainNumber;
};

}  // namespace shapeworks
