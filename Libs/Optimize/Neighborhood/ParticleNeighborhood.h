#pragma once

#include <vector>

#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "ParticlePointIndexPair.h"

namespace shapeworks {
class ParticleSystem;

/** \class ParticleNeighborhood
 *
 * A ParticleNeighborhood is responsible for computing neighborhoods of
 * particles.  Given a point position in a domain, and a neighborhood radius,
 * the ParticleNeighborhood returns a list of points that are neighbors of that
 * point.
 */
class ParticleNeighborhood {
 public:
  explicit ParticleNeighborhood(ParticleSystem* ps, int domain_id = -1) : ps_(ps), domain_id_(domain_id) {}

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights,
                                                               std::vector<double>& distances, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               double radius);

  void set_weighting_enabled(bool is_enabled) { weighting_enabled_ = is_enabled; }

  bool is_weighting_enabled() const { return weighting_enabled_; }

  void set_force_euclidean(bool is_enabled) { force_euclidean_ = is_enabled; }

  bool is_force_euclidean() const { return force_euclidean_; }

  void set_domain(ParticleDomain::Pointer domain) { domain_ = domain; };
  ParticleDomain::Pointer get_domain() const { return domain_; };

  void set_domain_id(int id) { domain_id_ = id; }

 private:
  std::vector<ParticlePointIndexPair> get_points_in_sphere(const itk::Point<double, 3>& position, int id,
                                                           double radius);

  ParticleSystem* ps_;
  ParticleDomain::Pointer domain_;
  int domain_id_{-1};
  double flat_cutoff_{0.3};
  bool weighting_enabled_{true};
  bool force_euclidean_{false};
};

}  // end namespace shapeworks
