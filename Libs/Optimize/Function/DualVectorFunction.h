#pragma once

#include "ParticleSystemEvaluation.h"
#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

/**
 * \class DualVectorFunction
 *
 * This class combines the results of evaluating 2 VectorFunction and
 * presents the interface of a single function evaluation. Optionally, only the
 * first function can be used by calling SetLinkOff().
 *
 */
class DualVectorFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  constexpr static unsigned int Dimension = VDimension;

  /// Type of vectors
  using VectorType = VectorFunction::VectorType;

  /// Factory method for creating instances
  static std::shared_ptr<DualVectorFunction> New() { return std::make_shared<DualVectorFunction>(); }

  /// The first argument is a pointer to the particle system.  The second
  /// argument is the index of the domain within that particle system.  The
  /// third argument is the index of the particle location within the given domain.
  VectorType evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxmove) const override {
    double maxA = 0;
    double maxB = 0;
    VectorType ansA;
    ansA.fill(0.0);
    VectorType ansB;
    ansB.fill(0.0);

    const_cast<DualVectorFunction*>(this)->counter_ = counter_ + 1.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (a_on_) {
      ansA = function_a_->evaluate(idx, d, system, maxA);
      const_cast<DualVectorFunction*>(this)->average_grad_mag_a_ = average_grad_mag_a_ + ansA.magnitude();
    }

    if (b_on_) {
      ansB = function_b_->evaluate(idx, d, system, maxB);
      const_cast<DualVectorFunction*>(this)->average_grad_mag_b_ = average_grad_mag_b_ + ansB.magnitude();
    }

    if (relative_gradient_scaling_ == 0.0) {
      ansB.fill(0.0);
      maxB = 0.0;
    }

    // get maxmove and predicted move for current configuration
    VectorType predictedMove;
    predictedMove.fill(0.0);

    // both A and B are active
    if (a_on_ && b_on_) {
      maxmove = maxA;  // always driven by the sampling to decrease the sensitivity to covariance regularization
      predictedMove = ansA + relative_gradient_scaling_ * ansB;
      return predictedMove;
    }

    // B is active, A is not active
    if (b_on_) {
      maxmove = maxB;
      predictedMove = ansB;
      return predictedMove;
    }

    // else only A is active (or if both are off, 0 will be return)
    maxmove = maxA;
    return ansA;
  }

  double EnergyA(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    function_a_->before_evaluate(idx, d, system);
    double ansA = 0.0;
    if (a_on_) {
      ansA = function_a_->energy(idx, d, system);
    }
    return ansA;
  }

  virtual double EnergyB(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    function_b_->before_evaluate(idx, d, system);
    double ansB = 0.0;
    if (b_on_ == true) {
      ansB = function_b_->energy(idx, d, system);
    }
    ansB *= relative_energy_scaling_;
    return ansB;
  }

  virtual double energy(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    double ansA = 0.0;
    double ansB = 0.0;
    double ansC = 0.0;
    double finalEnergy = 0.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (a_on_ == true) {
      ansA = function_a_->energy(idx, d, system);
    }

    if (b_on_ == true) {
      ansB = function_b_->energy(idx, d, system);
    }

    if (relative_energy_scaling_ == 0) {
      ansB = 0.0;
    }

    // compute final energy for current configuration
    if (b_on_ == true) {
      if (a_on_ == true)  // both A and B are active
      {
        finalEnergy = ansA + relative_energy_scaling_ * ansB;
        return (finalEnergy);
      } else  // B is active, A is not active
      {
        finalEnergy = ansB;
        return finalEnergy;
      }
    } else  // only A is active
    {
      return ansA;
    }

    return 0.0;
  }

  virtual VectorType evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxmove,
                              double& energy) const {
    double maxA = 0.0;
    double maxB = 0.0;
    double energyA = 0.0;
    double energyB = 0.0;
    VectorType ansA;
    ansA.fill(0.0);
    VectorType ansB;
    ansB.fill(0.0);

    const_cast<DualVectorFunction*>(this)->counter_ = counter_ + 1.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (a_on_ == true) {
      ansA = function_a_->evaluate(idx, d, system, maxA, energyA);

      const_cast<DualVectorFunction*>(this)->average_grad_mag_a_ = average_grad_mag_a_ + ansA.magnitude();
      const_cast<DualVectorFunction*>(this)->average_energy_a_ = average_energy_a_ + energyA;
    }

    if (b_on_ == true) {
      ansB = function_b_->evaluate(idx, d, system, maxB, energyB);

      const_cast<DualVectorFunction*>(this)->average_grad_mag_b_ = average_grad_mag_b_ + ansB.magnitude();
      const_cast<DualVectorFunction*>(this)->average_energy_b_ = average_energy_b_ + energyB;
    }

    if (relative_energy_scaling_ == 0.0) {
      energyB = 0.0;
      ansB.fill(0.0);
    }

    if (relative_gradient_scaling_ == 0.0) {
      maxB = 0.0;
      ansB.fill(0.0);
    }

    // compute final energy, maxmove and predicted move based on current configuration
    VectorType predictedMove;
    predictedMove.fill(0.0);
    if (b_on_ == true) {
      if (a_on_ == true)  // both A and B are active
      {
        if (maxB > maxA) {
          maxmove = maxB;
        } else {
          maxmove = maxA;
        }

        energy = energyA + relative_energy_scaling_ * energyB;

        maxmove = maxA;  // always driven by the sampling to decrease the senstivity to covariance regularization

        predictedMove = ansA + relative_gradient_scaling_ * ansB;

        return (predictedMove);
      } else  // only B is active, A is not active
      {
        maxmove = maxB;
        energy = energyB;
        predictedMove = ansB;

        return (predictedMove);
      }
    } else  // only A is active
    {
      maxmove = maxA;
      energy = energyA;
      return ansA;
    }
    maxmove = 0.0;
    return ansA;
  }

  virtual void before_evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system) {
    if (a_on_ == true) {
      function_a_->before_evaluate(idx, d, system);
    }

    if (b_on_ == true) {
      function_b_->before_evaluate(idx, d, system);
    }
  }

  /// This method is called by a solver after each iteration
  void after_iteration() override {
    if (a_on_) {
      function_a_->after_iteration();
    }
    if (b_on_) {
      function_b_->after_iteration();
    }
  }

  /// This method is called by a solver before each iteration
  void before_iteration() override {
    if (a_on_) {
      function_a_->before_iteration();
    }
    if (b_on_) {
      function_b_->before_iteration();
    }
    average_grad_mag_a_ = 0.0;
    average_grad_mag_b_ = 0.0;
    average_energy_a_ = 0.0;
    counter_ = 0.0;
  }

  /// Some subclasses may require a pointer to the particle system and its domain number
  void set_particle_system(ParticleSystem* p) override {
    VectorFunction::set_particle_system(p);
    if (function_a_ != nullptr) {
      function_a_->set_particle_system(p);
    }
    if (function_b_ != nullptr) {
      function_b_->set_particle_system(p);
    }
  }

  void set_domain_number(unsigned int i) override {
    VectorFunction::set_domain_number(i);
    if (function_a_ != nullptr) {
      function_a_->set_domain_number(i);
    }
    if (function_b_ != nullptr) {
      function_b_->set_domain_number(i);
    }
  }

  void set_function_a(std::shared_ptr<VectorFunction> o) {
    function_a_ = o;
    function_a_->set_domain_number(this->get_domain_number());
    function_a_->set_particle_system(this->get_particle_system());
  }

  std::shared_ptr<VectorFunction> get_function_a() { return function_a_; }

  std::shared_ptr<VectorFunction> get_function_b() { return function_b_; }

  void set_function_b(std::shared_ptr<VectorFunction> o) {
    function_b_ = o;
    function_b_->set_domain_number(this->get_domain_number());
    function_b_->set_particle_system(this->get_particle_system());
  }

  /// Turn each term on and off
  void set_a_on() { a_on_ = true; }
  void set_a_off() { a_on_ = false; }
  void set_a_on(bool s) { a_on_ = s; }
  bool get_a_on() const { return a_on_; }
  void set_b_on() { b_on_ = true; }
  void set_b_off() { b_on_ = false; }
  void set_b_on(bool s) { b_on_ = s; }
  bool get_b_on() const { return b_on_; }

  /// The relative scaling scales the gradient B relative to A. By default this value is 1.0.
  void set_relative_energy_scaling(double r) override { relative_energy_scaling_ = r; }
  double get_relative_energy_scaling() const override { return relative_energy_scaling_; }

  void set_relative_gradient_scaling(double r) { relative_gradient_scaling_ = r; }
  double get_relative_gradient_scaling() const { return relative_gradient_scaling_; }

  double get_average_grad_mag_a() const {
    if (counter_ != 0.0) {
      return average_grad_mag_a_ / counter_;
    } else {
      return 0.0;
    }
  }
  double get_average_grad_mag_b() const {
    if (counter_ != 0.0) {
      return average_grad_mag_b_ / counter_;
    } else {
      return 0.0;
    }
  }

  double get_average_energy_a() const {
    if (counter_ != 0.0) {
      return average_energy_a_ / counter_;
    } else {
      return 0.0;
    }
  }
  double get_average_energy_b() const {
    if (counter_ != 0.0) {
      return average_energy_b_ / counter_;
    } else {
      return 0.0;
    }
  }

  std::shared_ptr<VectorFunction> clone() override {
    auto copy = DualVectorFunction::New();
    copy->a_on_ = this->a_on_;
    copy->b_on_ = this->b_on_;

    copy->relative_gradient_scaling_ = this->relative_gradient_scaling_;
    copy->relative_energy_scaling_ = this->relative_energy_scaling_;
    copy->average_grad_mag_a_ = this->average_grad_mag_a_;
    copy->average_grad_mag_b_ = this->average_grad_mag_b_;
    copy->average_energy_a_ = this->average_energy_a_;
    copy->average_energy_b_ = this->average_energy_b_;
    copy->counter_ = this->counter_;

    if (this->function_a_) {
      copy->function_a_ = this->function_a_->clone();
    }
    if (this->function_b_) {
      copy->function_b_ = this->function_b_->clone();
    }

    if (!copy->function_a_) {
      copy->a_on_ = false;
    }
    if (!copy->function_b_) {
      copy->b_on_ = false;
    }

    copy->domain_number_ = this->domain_number_;
    copy->particle_system_ = this->particle_system_;

    return copy;
  }

  DualVectorFunction() : a_on_(true), b_on_(false), relative_gradient_scaling_(1.0), relative_energy_scaling_(1.0) {}

  ~DualVectorFunction() override = default;

 protected:
  DualVectorFunction(const DualVectorFunction&) = delete;
  DualVectorFunction& operator=(const DualVectorFunction&) = delete;

  bool a_on_;
  bool b_on_;
  double relative_gradient_scaling_;
  double relative_energy_scaling_;
  double average_grad_mag_a_;
  double average_grad_mag_b_;
  double average_energy_a_;
  double average_energy_b_;
  double counter_;

  std::shared_ptr<VectorFunction> function_a_;
  std::shared_ptr<VectorFunction> function_b_;
};

}  // namespace shapeworks
