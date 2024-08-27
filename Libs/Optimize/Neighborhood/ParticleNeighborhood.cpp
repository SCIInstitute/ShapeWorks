#include "ParticleNeighborhood.h"

#include <Libs/Optimize/Domain/MeshDomain.h>

#include "ParticleSystem.h"

namespace shapeworks {

//--------------------------------------------------------------------------------------------------
std::pair<std::vector<ParticlePointIndexPair>, std::vector<double>> ParticleNeighborhood::get_points_in_sphere(
    const itk::Point<double, 3>& position, int id, double radius) {
  // iterate over all particles in the system for this domain, return those within radius
  std::vector<ParticlePointIndexPair> neighbors;
  std::vector<double> distances;

  auto num_particles = ps_->GetNumberOfParticles(domain_id_);

  for (unsigned int i = 0; i < num_particles; i++) {
    if (i == id) {
      continue;
    }
    auto p = ps_->GetPosition(i, domain_id_);
    double distance = (p - position).GetNorm();
    if (distance < radius) {
      neighbors.push_back(ParticlePointIndexPair(p, i));
      distances.push_back(distance);
    }
  }

  return {neighbors, distances};
}

//--------------------------------------------------------------------------------------------------

std::vector<ParticlePointIndexPair> ParticleNeighborhood::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, std::vector<double>& weights, std::vector<double>& distances,
    double radius) {
  auto [pointlist, neighbor_distance] = get_points_in_sphere(position, id, radius);

  using GradientVectorType = vnl_vector_fixed<float, 3>;
  using PointType = itk::Point<double, 3>;

  GradientVectorType normal;
  if (weighting_enabled_) {  // uninitialized otherwise, but we're trying to avoid looking up the normal if we can
    normal = domain_->SampleNormalAtPoint(position, id);
  }

  weights.clear();
  distances.clear();

  bool use_euclidean = true;

  if (domain_->GetDomainType() == DomainType::Mesh) {
    // cast to MeshDomain to ask if geodesics are enabled
    auto mesh_domain = std::dynamic_pointer_cast<MeshDomain>(domain_);
    use_euclidean = !mesh_domain->GetMeshWrapper()->IsGeodesicsEnabled();
  } else if (domain_->GetDomainType() == DomainType::Contour) {
    use_euclidean = false;
  }

  if (force_euclidean_) {
    use_euclidean = true;
  }

  std::vector<ParticlePointIndexPair> ret;

  for (int i = 0; i < pointlist.size(); i++) {
    const auto& pt_b = pointlist[i].Point;
    const auto& idx_b = pointlist[i].Index;

    double distance = neighbor_distance[i];
    bool is_within_distance = true;

    if (!use_euclidean) {
      is_within_distance = domain_->IsWithinDistance(position, id, pt_b, idx_b, radius, distance);
    }

    if (is_within_distance) {
      ret.push_back(pointlist[i]);
      distances.push_back(distance);

      // todo change the APIs so don't have to pass a std::vector<double> of 1s whenever weighting is disabled
      if (!weighting_enabled_) {
        weights.push_back(1.0);
        continue;
      }

      const GradientVectorType pn = domain_->SampleNormalAtPoint(pointlist[i].Point, pointlist[i].Index);
      const double cosine = dot_product(normal, pn);  // normals already normalized
      if (cosine >= flat_cutoff_) {
        weights.push_back(1.0);
      } else {
        // Drop to zero influence over 90 degrees.
        weights.push_back(cos((flat_cutoff_ - cosine) / (1.0 + flat_cutoff_) * 1.5708));
      }
    }
  }
  return ret;
}

//--------------------------------------------------------------------------------------------------
std::vector<ParticlePointIndexPair> ParticleNeighborhood::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, std::vector<double>& weights, double radius) {
  std::vector<double> distances;
  return find_neighborhood_points(position, id, weights, distances, radius);
}

//--------------------------------------------------------------------------------------------------
std::vector<ParticlePointIndexPair> ParticleNeighborhood::find_neighborhood_points(
    const itk::Point<double, 3>& position, int id, double radius) {
  std::vector<double> weights;
  std::vector<double> distances;
  return find_neighborhood_points(position, id, weights, distances, radius);
}

}  // namespace shapeworks
