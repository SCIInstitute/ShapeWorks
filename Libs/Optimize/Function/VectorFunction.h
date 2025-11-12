#pragma once

#include <memory>
#include "Libs/Optimize/ParticleSystem.h"
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
class VectorFunction {
 public:
  constexpr static unsigned int Dimension = 3;

  /// Type of vectors
  using VectorType = vnl_vector_fixed<double, Dimension>;

  /// The first argument is a pointer to the particle system.  The second
  /// argument is the index of the domain within that particle system.  The
  /// third argument is the index of the particle location within the given domain.
  virtual VectorType evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep) const = 0;
  virtual VectorType evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep,
                              double& energy) const = 0;
  virtual double energy(unsigned int, unsigned int, const ParticleSystem*) const = 0;

  /// May be called by the solver class
  virtual void reset_buffers() {}

  /// This method is called by a solver after each iteration
  virtual void after_iteration() {}

  /// This method is called by a solver before each iteration
  virtual void before_iteration() {}

  /// This method may be called to set up the state of the function object
  /// before a call to evaluate.  It is necessary in order to initialize
  /// certain constants and variables that may be used for calculating the
  /// energy as well as the gradients.  Typically this is only necessary for
  /// the adaptive gradient descent algorithm.
  virtual void before_evaluate(unsigned int, unsigned int, const ParticleSystem*) {}

  /// Some subclasses may require a pointer to the particle system and its domain number
  virtual void set_particle_system(ParticleSystem* p) { particle_system_ = p; }
  virtual ParticleSystem* get_particle_system() const { return particle_system_; }
  virtual void set_domain_number(unsigned int i) { domain_number_ = i; }
  virtual int get_domain_number() const { return domain_number_; }

  virtual std::shared_ptr<VectorFunction> clone() {
    std::cerr << "Error: base class VectorFunction clone method called!\n";
    std::cerr << "Threaded run of current parameters not supported!\n";
    return nullptr;
  }

  virtual double get_relative_energy_scaling() const { return 1.0; }
  virtual void set_relative_energy_scaling(double r) { return; }

  /// Virtual destructor for proper cleanup of derived classes
  virtual ~VectorFunction() = default;

 protected:
  VectorFunction() : particle_system_(nullptr), domain_number_(0) {}
  VectorFunction(const VectorFunction&) = delete;
  VectorFunction& operator=(const VectorFunction&) = delete;

  ParticleSystem* particle_system_;
  unsigned int domain_number_;
};

}  // namespace shapeworks
