#pragma once

#include <string>
#include <utility>
#include <vector>

#include <Image/Image.h>
#include <Image/ImageRegistration.h>
#include <Mesh/Mesh.h>

#include "Libs/Common/Shapeworks.h"

namespace shapeworks {

class Optimize;

/**
 * \class RegistrationInitializer
 * \ingroup Group-Optimize
 *
 * Establishes the initial correspondence by a different route than splitting.  Particles are spread
 * over one reference shape alone, that shape is registered to each of the others, and its particles
 * are carried across, so that every shape starts from a set that already corresponds rather than
 * from one that has to be brought into correspondence by optimization.
 *
 * Holds no state of its own beyond the run in progress: the particles, the domains and the settings
 * all belong to the Optimize it is initializing, which it works on directly.
 */
class RegistrationInitializer {
 public:
  explicit RegistrationInitializer(Optimize& optimize) : optimize_(optimize) {}

  //! Spread particles over the reference shape and carry them to every other shape
  void Run();

 private:
  //! Pick the shape used as the registration template, honoring an explicitly requested one
  int ResolveRegistrationReference();

  //! Split and optimize particles on the reference shape alone, until it holds the requested counts
  void SpreadParticlesOnReference(int reference_shape);

  //! Register the reference to every other shape and carry its particles across
  void TransferParticlesFromReference(int reference_shape);

  //! Move transferred particles that the domain cannot sample back to somewhere it can, and refuse
  //! a registration that has misplaced more than a stray handful of them
  void RescueTransferredParticles(int domain, const std::vector<Point3>& reference_points,
                                  std::vector<Point3>& transferred) const;

  //! Log how well a shape's transferred particles landed on its surface, and return how far from it
  //! they landed on average, so that the shapes can be compared with one another
  double ReportTransferQuality(int domain, const std::vector<Point3>& reference_points,
                               const std::vector<Point3>& transferred);

  //! Log how the whole cohort landed, which is what gives any one shape's figure a scale to be read
  //! against
  void ReportTransferSummary(const std::vector<std::pair<std::string, double>>& landings) const;

  //! The groomed file a domain came from, for messages that need to name it
  std::string GetDomainName(int domain) const;

  //! Path the given registration's transform is cached at, or "" when caching is off.  The name
  //! covers everything the transform depends on, so a stale one is never mistaken for a hit.
  std::string GetRegistrationCachePath(const ImageRegistration& registration, int reference_domain,
                                       int domain) const;

  //! Build the image used to register the given domain.  Mesh domains are rasterized to a distance
  //! transform; image domains are read back from their groomed path.
  Image GetRegistrationImage(int domain);

  //! Return the band to retain around the surface, defaulting to a few voxels of the given spacing
  double GetRegistrationBand(double spacing) const;

  //! Return the surface of the given domain, reading it back from disk for image domains
  Mesh GetDomainSurface(int domain);

  Optimize& optimize_;
};

}  // namespace shapeworks
