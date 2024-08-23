#include "ParticleNeighborhood.h"

#include "ParticleSystem.h"

namespace shapeworks {

//--------------------------------------------------------------------------------------------------
std::vector<ParticlePointIndexPair> ParticleNeighborhoodTwo::get_points_in_sphere(const itk::Point<double, 3>& position,
                                                                                  int id, double radius) {
  // iterate over all particles in the system for this domain, return those within radius
  std::vector<ParticlePointIndexPair> neighbors;

  auto num_particles = ps_->GetNumberOfParticles(domain_id_);

  for (unsigned int i = 0; i < num_particles; i++) {
    if (i == id) {
      continue;
    }
    auto p = ps_->GetPosition(i, domain_id_);
    double distance = (p - position).GetNorm();
    if (distance < radius) {
      neighbors.push_back(ParticlePointIndexPair(p, i));
    }
  }

  return neighbors;
}

//--------------------------------------------------------------------------------------------------

std::vector<ParticlePointIndexPair> ParticleNeighborhoodTwo::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, std::vector<double>& weights, std::vector<double>& distances,
    double radius) {
  auto neighbors = get_points_in_sphere(position, id, radius);

  using GradientVectorType = vnl_vector_fixed<float, 3>;
  using PointType = itk::Point<double, 3>;
  const int Dimension = 3;

  GradientVectorType posnormal;
  if (weighting_enabled_) {  // uninitialized otherwise, but we're trying to avoid looking up the normal if we can
    posnormal = domain_->SampleNormalAtPoint(position, id);
  }

  weights.clear();
  distances.clear();

  // Grab the list of points in this bounding box.
  auto pointlist = get_points_in_sphere(position, id, radius);

  // Allocate return vector.  Reserve ensures no extra copies occur.
  std::vector<ParticlePointIndexPair> ret;
  ret.reserve(pointlist.size());  // todo investigate performance here: does it actually help?
  weights.reserve(pointlist.size());

  // Add any point whose distance from center is less than radius to the return list
  for (auto it = pointlist.begin(); it != pointlist.end(); it++) {
    const auto& pt_b = it->Point;
    const auto& idx_b = it->Index;

    double distance;
    bool is_within_distance;

    if (force_euclidean_) {
      distance = position.EuclideanDistanceTo(pt_b);
      is_within_distance = distance < radius;
    } else {
      is_within_distance = domain_->IsWithinDistance(position, id, pt_b, idx_b, radius, distance);
    }

    if (is_within_distance) {
      ret.push_back(*it);
      distances.push_back(distance);

      // todo change the APIs so don't have to pass a std::vector<double> of 1s whenever weighting is disabled
      if (!weighting_enabled_) {
        weights.push_back(1.0);
        continue;
      }

      const GradientVectorType pn = domain_->SampleNormalAtPoint(it->Point, it->Index);
      const double cosine = dot_product(posnormal, pn);  // normals already normalized
      if (cosine >= flat_cutoff_) {
        weights.push_back(1.0);
      } else {
        // Drop to zero influence over 90 degrees.
        weights.push_back(cos((flat_cutoff_ - cosine) / (1.0 + flat_cutoff_) * 1.5708));
      }
    }
  }
  return ret;

  return neighbors;
}

//--------------------------------------------------------------------------------------------------
std::vector<ParticlePointIndexPair> ParticleNeighborhoodTwo::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, std::vector<double>& weights, double radius) {
  std::vector<double> distances;
  return find_neighborhood_points(position, id, weights, distances, radius);
}

//--------------------------------------------------------------------------------------------------
std::vector<ParticlePointIndexPair> ParticleNeighborhoodTwo::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, double radius) {
  std::vector<double> weights;
  std::vector<double> distances;
  return find_neighborhood_points(position, id, weights, distances, radius);
}

}  // namespace shapeworks
