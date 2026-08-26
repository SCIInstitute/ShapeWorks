#include "RegistrationInitializer.h"

#include <algorithm>
#include <limits>
#include <optional>
#include <sstream>

#include <boost/filesystem.hpp>

#include <Logging.h>
#include <Mesh/MeshUtils.h>

#include "Libs/Optimize/Domain/MeshDomain.h"
#include "Optimize.h"
#include "ShapeworksUtils.h"

namespace shapeworks {

//! Width of the retained band around the surface, as a fraction of the shape's largest dimension,
//! when it is not set explicitly.  The band is what the similarity metric can actually see, so it
//! must be a physical width independent of the rasterization grid: tying it to the grid would
//! silently narrow it as the grid is raised and starve the metric of context on dissimilar shapes.
//! ~5% of the shape size was the best compromise across similar and dissimilar pairs in testing --
//! wide enough to bridge shape differences, not so wide that it loses surface detail.
constexpr double REGISTRATION_BAND_FRACTION = 0.05;

//! Width of the image-domain band, in voxels of the (fixed) distance transform.  Image inputs already
//! carry their own resolution, so there is no grid to decouple from.
constexpr double REGISTRATION_BAND_VOXELS = 4.0;

//! A transferred particle further than this fraction of the shape's size from the surface is
//! considered mislanded, whatever units the data is in
constexpr double TRANSFER_FAR_FRACTION = 0.05;

//! Warn that a registration may have failed once this fraction of a shape's particles are mislanded.
//! Keyed off a fraction rather than the single worst particle so a stray outlier is not a false alarm.
constexpr double TRANSFER_FAR_FRACTION_THRESHOLD = 0.25;

//---------------------------------------------------------------------------
Mesh RegistrationInitializer::GetDomainSurface(int domain) {
  auto* particle_domain = optimize_.m_sampler->GetParticleSystem()->GetDomain(domain);

  if (auto* mesh_domain = dynamic_cast<MeshDomain*>(particle_domain)) {
    return Mesh(mesh_domain->get_surface()->get_polydata());
  }

  // an image domain keeps only a narrow band of its distance transform, so read the groomed input
  // back from disk to recover the full surface
  if (domain >= static_cast<int>(optimize_.m_domain_paths.size()) || optimize_.m_domain_paths[domain].empty()) {
    throw std::runtime_error(
        "Registration based initialization needs the path of each groomed input, but none was given for domain " +
        std::to_string(domain));
  }
  return Image(optimize_.m_domain_paths[domain]).toMesh(0.0);
}

//---------------------------------------------------------------------------
Image RegistrationInitializer::GetRegistrationImage(int domain) {
  auto* particle_domain = optimize_.m_sampler->GetParticleSystem()->GetDomain(domain);

  if (particle_domain->GetDomainType() == DomainType::Image) {
    if (domain >= static_cast<int>(optimize_.m_domain_paths.size()) || optimize_.m_domain_paths[domain].empty()) {
      throw std::runtime_error(
          "Registration based initialization needs the path of each groomed input, but none was given for domain " +
          std::to_string(domain));
    }
    // the groomed input is already a distance transform, so take its resolution as given
    Image dt(optimize_.m_domain_paths[domain]);
    const auto spacing = dt.spacing();
    const double largest_spacing = std::max({spacing[0], spacing[1], spacing[2]});
    return ImageRegistration::make_registration_image(dt, GetRegistrationBand(largest_spacing));
  }

  // A mesh carries no resolution of its own, so pick one from how large it actually is.  Assuming a
  // spacing (and a band) in millimeters would break on data stored in other units, where a fixed
  // spacing yields either an unusable grid or one too coarse to register.
  Mesh mesh = GetDomainSurface(domain);
  auto region = mesh.boundingBox();
  const auto extent = region.size();
  const double largest_extent = std::max({extent[0], extent[1], extent[2]});
  const double spacing = largest_extent / optimize_.m_registration_grid_size;

  // Band is a physical width derived from the shape's size, independent of the chosen grid, so the
  // grid controls only resolution and the two do not confound each other.
  const double band =
      optimize_.m_registration_band > 0.0 ? optimize_.m_registration_band : REGISTRATION_BAND_FRACTION * largest_extent;

  // pad far enough that the whole band around the surface stays inside the grid
  region.pad(band * 2.0);
  return ImageRegistration::make_registration_image(
      mesh.toDistanceTransform(region, Point3({spacing, spacing, spacing})), band);
}

//---------------------------------------------------------------------------
double RegistrationInitializer::GetRegistrationBand(double spacing) const {
  if (optimize_.m_registration_band > 0.0) {
    return optimize_.m_registration_band;
  }
  // default to a band a few voxels wide, which keeps the metric focused near the surface without
  // making it so thin that it spans less than a voxel
  return REGISTRATION_BAND_VOXELS * spacing;
}

//---------------------------------------------------------------------------
int RegistrationInitializer::ResolveRegistrationReference() {
  const int num_subjects = optimize_.GetNumberOfSubjects();
  if (num_subjects < 1) {
    throw std::runtime_error("No shapes to initialize");
  }

  if (optimize_.m_registration_reference >= 0) {
    if (optimize_.m_registration_reference >= num_subjects) {
      throw std::runtime_error("Requested registration reference " +
                               std::to_string(optimize_.m_registration_reference) +
                               " is out of range, there are only " + std::to_string(num_subjects) + " shapes");
    }
    return optimize_.m_registration_reference;
  }

  if (num_subjects == 1) {
    return 0;
  }

  // combine each subject's domains into one mesh so that the template is representative of the whole
  // anatomy rather than of a single domain
  optimize_.PrintStartMessage("Choosing a registration reference...");
  std::vector<Mesh> meshes;
  for (int s = 0; s < num_subjects; s++) {
    Mesh mesh = GetDomainSurface(s * optimize_.m_domains_per_shape);
    for (int d = 1; d < static_cast<int>(optimize_.m_domains_per_shape); d++) {
      mesh += GetDomainSurface(s * optimize_.m_domains_per_shape + d);
    }
    meshes.push_back(mesh);
  }

  const int reference = MeshUtils::findReferenceMesh(meshes);
  if (reference < 0 || reference >= num_subjects) {
    throw std::runtime_error("Could not choose a registration reference");
  }
  optimize_.PrintDoneMessage();
  return reference;
}

//---------------------------------------------------------------------------
void RegistrationInitializer::SpreadParticlesOnReference(int reference_shape) {
  auto* system = optimize_.m_sampler->GetParticleSystem();
  const int first_domain = reference_shape * optimize_.m_domains_per_shape;

  // only the reference carries particles for the moment, so the correspondence matrices cannot keep
  // a consistent layout; they are brought back once every shape has been populated
  optimize_.m_sampler->SetCorrespondenceMatricesSuspended(true);

  // seed a single particle on each of the reference's domains
  for (int d = 0; d < static_cast<int>(optimize_.m_domains_per_shape); d++) {
    const int domain = first_domain + d;
    if (system->GetNumberOfParticles(domain) == 0) {
      auto* particle_domain = system->GetDomain(domain);
      system->AddPosition(particle_domain->GetValidLocationNear(particle_domain->GetZeroCrossingPoint()), domain);
    }
  }
  system->SynchronizePositions();

  // allocate everything without optimizing, so that turning correspondence off below sticks; Execute
  // forces it back on the first time it runs
  optimize_.m_sampler->Initialize();

  // only one shape carries particles at this point, so there is no correspondence to establish yet
  optimize_.m_sampler->SetCorrespondenceOff();

  // report progress against the reference rather than the first shape, which is still empty
  optimize_.m_progress_domain_offset = first_domain;

  const double epsilon = optimize_.m_spacing;

  auto needs_split = [&]() {
    for (int d = 0; d < static_cast<int>(optimize_.m_domains_per_shape); d++) {
      if (system->GetNumberOfParticles(first_domain + d) < optimize_.m_number_of_particles[d]) {
        return true;
      }
    }
    return false;
  };

  while (needs_split() && !optimize_.m_aborted) {
    optimize_.OptimizerStop();

    for (int d = 0; d < static_cast<int>(optimize_.m_domains_per_shape); d++) {
      const int domain = first_domain + d;
      if (system->GetNumberOfParticles(domain) < optimize_.m_number_of_particles[d]) {
        system->SplitAllParticlesInDomain(epsilon, domain);
      }
    }
    system->SynchronizePositions();

    optimize_.m_split_number++;
    if (optimize_.m_verbosity_level > 0) {
      std::string counts;
      for (int d = 0; d < static_cast<int>(optimize_.m_domains_per_shape); d++) {
        counts += " " + std::to_string(system->GetNumberOfParticles(first_domain + d));
      }
      SW_LOG("Reference split {}, particle count:{}", optimize_.m_split_number, counts);
    }

    optimize_.m_energy_a.clear();
    optimize_.m_energy_b.clear();
    optimize_.m_total_energy.clear();
    optimize_.m_str_energy = "split" + std::to_string(optimize_.m_split_number) + "pts_init";

    optimize_.m_sampler->GetOptimizer()->set_maximum_number_of_iterations(optimize_.m_iterations_per_split);
    optimize_.m_sampler->GetOptimizer()->set_number_of_iterations(0);
    optimize_.m_sampler->Execute();
  }

  optimize_.m_progress_domain_offset = 0;
}

//---------------------------------------------------------------------------
std::string RegistrationInitializer::GetRegistrationCachePath(const ImageRegistration& registration,
                                                              int reference_domain, int domain) const {
  if (optimize_.m_registration_cache_dir.empty()) {
    return "";
  }

  auto describe = [&](int d) {
    std::string description =
        d < static_cast<int>(optimize_.m_domain_paths.size()) ? optimize_.m_domain_paths[d] : std::to_string(d);
    // a groomed input that has been rewritten must not read back as a hit
    if (ShapeWorksUtils::file_exists(description)) {
      try {
        description += ":" + std::to_string(boost::filesystem::file_size(description));
        description += ":" + std::to_string(boost::filesystem::last_write_time(description));
      } catch (const std::exception&) {
      }
    }
    return description;
  };

  // Everything the transform depends on, and nothing that it does not.  How the registration itself
  // was performed is the registration's own account of it, so that changing the algorithm cannot
  // leave a description here saying otherwise; what is left is how these two images were prepared
  // for it, which is this class's doing.
  std::string key = describe(reference_domain) + "->" + describe(domain);
  key += "|" + registration.settings_description();
  key += "|band=" + std::to_string(optimize_.m_registration_band);
  key += "|grid=" + std::to_string(optimize_.m_registration_grid_size);

  const auto hash = std::hash<std::string>{}(key);
  std::stringstream name;
  name << optimize_.m_registration_cache_dir << "/registration_" << std::hex << hash << ".tfm";
  return name.str();
}

//---------------------------------------------------------------------------
void RegistrationInitializer::TransferParticlesFromReference(int reference_shape) {
  auto* system = optimize_.m_sampler->GetParticleSystem();
  const int num_subjects = optimize_.GetNumberOfSubjects();

  // how each shape ended up landing, so that the run can close by saying what is normal for this
  // cohort and which shape sits furthest from it
  std::vector<std::pair<std::string, double>> landings;

  for (int d = 0; d < static_cast<int>(optimize_.m_domains_per_shape) && !optimize_.m_aborted; d++) {
    const int reference_domain = reference_shape * optimize_.m_domains_per_shape + d;

    std::vector<Point3> reference_points;
    for (auto k = 0; k < system->GetNumberOfParticles(reference_domain); k++) {
      reference_points.push_back(system->GetPosition(k, reference_domain));
    }

    // The reference image is only needed to run a registration, so it is built lazily on the first
    // cache miss.  Rasterizing a large mesh takes several seconds, and when every transfer is a cache
    // hit (e.g. re-running with different optimization parameters) it is never needed at all.
    std::optional<Image> reference_image;

    for (int s = 0; s < num_subjects && !optimize_.m_aborted; s++) {
      if (s == reference_shape) {
        continue;
      }
      const int domain = s * optimize_.m_domains_per_shape + d;

      optimize_.UpdateProgress(fmt::format("Registering shape {} of {}", s + 1, num_subjects));
      optimize_.RefreshDuringTransfer();

      ImageRegistration registration;
      registration.set_transform_type(optimize_.m_registration_transform_type);
      registration.set_gradient_step(optimize_.m_registration_gradient_step);
      registration.set_update_field_variance(optimize_.m_registration_flow_sigma);

      const auto cache_path = GetRegistrationCachePath(registration, reference_domain, domain);
      // check for the file first: asking the reader for one that is not there is a normal cache
      // miss, but it makes the HDF5 layer print an alarming block of text
      const bool cached =
          !cache_path.empty() && ShapeWorksUtils::file_exists(cache_path) && registration.load_transform(cache_path);
      if (cached) {
        SW_DEBUG("Reusing cached registration: {}", cache_path);
      } else {
        if (!reference_image) {
          // only now, on a genuine miss, is the reference image worth building
          optimize_.UpdateProgress(optimize_.m_domains_per_shape > 1
                                       ? fmt::format("Preparing registration (domain {})", d + 1)
                                       : std::string("Preparing registration"));
          optimize_.RefreshDuringTransfer();
          reference_image = GetRegistrationImage(reference_domain);
        }
        registration.run(*reference_image, GetRegistrationImage(domain));
        if (!cache_path.empty()) {
          try {
            registration.save_transform(cache_path);
          } catch (const std::exception& e) {
            SW_WARN("Unable to cache registration: {}", e.what());
          }
        }
      }

      auto transferred = registration.transform_points(reference_points);

      // A registration can leave a particle somewhere this shape cannot be sampled: outside its
      // image, or too far from its surface for the narrow band to reach.  Settle that here, where
      // the shape can be named, rather than leaving it to surface as an unplaceable particle from
      // deep inside the sampler.
      RescueTransferredParticles(domain, reference_points, transferred);

      // AddPositionList applies the domain constraints, which pulls each point onto the surface
      system->AddPositionList(transferred, domain);

      landings.emplace_back(GetDomainName(domain), ReportTransferQuality(domain, reference_points, transferred));

      // registrations do not run particle iterations, so charge each one its share of the budget
      // reserved for them; without this the bar would sit still through the longest phase
      optimize_.current_particle_iterations_ += optimize_.m_transfer_iteration_weight;
      optimize_.UpdateProgress(fmt::format("Registered shape {} of {}", s + 1, num_subjects));
      // the shape now has its particles; let the GUI redraw them (and the status) between
      // registrations rather than only when the whole phase ends
      optimize_.RefreshDuringTransfer();
    }
  }

  ReportTransferSummary(landings);
}

//---------------------------------------------------------------------------
std::string RegistrationInitializer::GetDomainName(int domain) const {
  return domain < static_cast<int>(optimize_.m_filenames.size()) ? optimize_.m_filenames[domain]
                                                                 : std::to_string(domain);
}

//---------------------------------------------------------------------------
void RegistrationInitializer::ReportTransferSummary(const std::vector<std::pair<std::string, double>>& landings) const {
  if (landings.empty()) {
    return;
  }

  std::vector<double> distances;
  distances.reserve(landings.size());
  for (const auto& landing : landings) {
    distances.push_back(landing.second);
  }
  const auto middle = distances.begin() + distances.size() / 2;
  std::nth_element(distances.begin(), middle, distances.end());

  const auto furthest = std::max_element(landings.begin(), landings.end(),
                                         [](const auto& a, const auto& b) { return a.second < b.second; });

  // One shape landing further out than the rest is what a failed registration looks like from here,
  // so give the typical figure and the outlier together: either number alone says nothing.
  // one entry per domain of every registered subject, so say registrations rather than shapes: with
  // more than one domain per shape these are not the same count, and on different anatomies they are
  // not even the same scale
  SW_LOG(
      "Registration based initialization: across {} registrations, particles landed a median of {:.3g} from the "
      "surface, and furthest on {} at {:.3g}",
      landings.size(), *middle, furthest->first, furthest->second);
}

//---------------------------------------------------------------------------
void RegistrationInitializer::RescueTransferredParticles(int domain,
                                                        const std::vector<Point3>& reference_points,
                                                        std::vector<Point3>& transferred) const {
  auto* particle_domain = optimize_.m_sampler->GetParticleSystem()->GetDomain(domain);

  const auto& lower = particle_domain->GetLowerBound();
  const auto& upper = particle_domain->GetUpperBound();

  std::vector<bool> placed(transferred.size(), true);
  std::vector<size_t> misplaced;
  int off_the_image = 0;
  for (size_t i = 0; i < transferred.size(); i++) {
    placed[i] = particle_domain->IsValidLocation(transferred[i]);
    if (placed[i]) {
      continue;
    }
    misplaced.push_back(i);

    // Whether a particle can be sampled depends on how wide a narrow band was asked for, which is a
    // setting rather than a verdict on the registration; whether it is inside the image at all does
    // not.  Count the two apart so that a tighter band cannot masquerade as a failed registration.
    for (unsigned int c = 0; c < 3; c++) {
      if (transferred[i][c] < lower[c] || transferred[i][c] > upper[c]) {
        off_the_image++;
        break;
      }
    }
  }

  if (misplaced.empty()) {
    return;
  }

  const std::string name = GetDomainName(domain);

  // A registration that worked puts every particle somewhere on the shape, so more than a stray
  // handful landing outside the image altogether means it did not work at all.  Nothing downstream
  // can make a shape model out of that, and stopping here is what lets the failure name the shape it
  // happened to.
  if (off_the_image > TRANSFER_FAR_FRACTION_THRESHOLD * transferred.size()) {
    throw std::runtime_error(fmt::format(
        "Registration based initialization failed for {}: {} of its {} particles were mapped outside its image "
        "altogether. The registration from the reference shape did not converge, so there is nothing here to start "
        "the optimization from. Check that this shape's groomed input covers the same anatomy as the rest, or "
        "choose a different reference shape with the initialization reference setting.",
        name, off_the_image, transferred.size()));
  }

  // The rescue walks each stray towards a particle that did land, so there has to be one.  Every
  // particle being unplaceable means the band is too narrow to hold this registration at all, which
  // no amount of walking will fix, and walking towards a stray would leave them where they were.
  if (misplaced.size() == transferred.size()) {
    throw std::runtime_error(fmt::format(
        "Registration based initialization failed for {}: none of its {} particles landed within the narrow band of "
        "the surface, so there is nowhere to move them back to. Registration based initialization starts particles "
        "wherever the reference shape maps to, which can be much further from the surface than the split based "
        "initialization the default narrow band is sized for; increase the narrow band optimization parameter.",
        name, transferred.size()));
  }

  // Aim each stray at where its neighbours ended up rather than at some arbitrary point on the
  // shape.  Particles are transferred in correspondence, so the one beside it on the reference is
  // the best evidence there is of where this one belonged.
  constexpr int RESCUE_STEPS = 40;

  double furthest = 0.0;

  for (const auto index : misplaced) {
    // the nearest particle that was placed, measured on the reference, where the correspondence
    // between the two shapes is defined
    size_t nearest = 0;
    double nearest_distance = std::numeric_limits<double>::max();
    for (size_t i = 0; i < transferred.size(); i++) {
      const double distance = reference_points[index].SquaredEuclideanDistanceTo(reference_points[i]);
      if (placed[i] && distance < nearest_distance) {
        nearest_distance = distance;
        nearest = i;
      }
    }

    const Point3 stray = transferred[index];
    const Point3 target = transferred[nearest];
    auto along = [&](double fraction) {
      Point3 point;
      for (unsigned int c = 0; c < 3; c++) {
        point[c] = stray[c] + fraction * (target[c] - stray[c]);
      }
      return point;
    };

    // Walk in from the stray until the shape can be sampled again, then carry on halfway to the
    // target and settle there.  The first location that can be sampled is the far edge of the narrow
    // band, where there is too little distance transform left around the particle to project it onto
    // the surface; going halfway on towards a particle that did land keeps the direction the
    // registration chose while leaving this one somewhere the domain can work with.  The last step
    // of the walk is the target itself, so somewhere is always found.
    for (int step = 1; step <= RESCUE_STEPS; step++) {
      const double fraction = static_cast<double>(step) / RESCUE_STEPS;
      if (!particle_domain->IsValidLocation(along(fraction))) {
        continue;
      }
      const Point3 deeper = along((fraction + 1.0) / 2.0);
      transferred[index] = particle_domain->IsValidLocation(deeper) ? deeper : along(fraction);
      break;
    }

    furthest = std::max(furthest, stray.EuclideanDistanceTo(transferred[index]));
  }

  // Telling someone that something is worth checking is no use without telling them how.  Which
  // remedy to point at depends on where the particles went: outside the image is the registration's
  // doing, whereas inside it but beyond the narrow band only means the band is too tight to hold
  // them, which says nothing about the registration and has an entirely different answer.
  if (off_the_image > 0) {
    SW_WARN(
        "{}: {} of {} transferred particles landed off the shape, by as much as {:.3g}, and were moved back onto it. "
        "The registration from the reference shape did not fit this one closely (the distance a well registered shape "
        "lands within is logged at the end of initialization). Look at this shape in the viewer, or in the "
        "correspondence quality panel, once the run finishes. If its particles do not correspond to the others, "
        "either register from a different reference shape with the initialization reference setting, or check that "
        "grooming left this shape aligned with the rest of the cohort.",
        name, misplaced.size(), transferred.size(), furthest);
  } else {
    SW_WARN(
        "{}: {} of {} transferred particles registered within the image but outside the narrow band, by as much as "
        "{:.3g}, and were moved back onto the shape. Increasing the narrow band optimization parameter may help.",
        name, misplaced.size(), transferred.size(), furthest);
  }
}

//---------------------------------------------------------------------------
double RegistrationInitializer::ReportTransferQuality(int domain, const std::vector<Point3>& reference_points,
                                                      const std::vector<Point3>& transferred) {
  if (transferred.empty()) {
    return 0.0;
  }

  auto* system = optimize_.m_sampler->GetParticleSystem();

  // Judge the transfer against the size of the shape rather than an absolute distance, so the same
  // thresholds work whatever units and resolution the data is in.  Take that size from the domain
  // itself, not from the particles: particles that have collapsed together would otherwise shrink
  // the scale in step with the distances being judged, and always look acceptable.
  const auto* particle_domain = system->GetDomain(domain);
  const double shape_scale = particle_domain->GetLowerBound().EuclideanDistanceTo(particle_domain->GetUpperBound());

  // A particle beyond this fraction of the shape's size is clearly in the wrong place
  const double far_distance = TRANSFER_FAR_FRACTION * shape_scale;

  double total_snap = 0.0;
  double worst_snap = 0.0;
  int far_count = 0;
  int unmoved = 0;

  for (size_t i = 0; i < transferred.size(); i++) {
    // how far the point had to travel to reach the surface is how far off the surface it landed
    const double snap = transferred[i].EuclideanDistanceTo(system->GetPosition(i, domain));
    total_snap += snap;
    worst_snap = std::max(worst_snap, snap);
    if (shape_scale > 0.0 && snap > far_distance) {
      far_count++;
    }

    // a point outside the displacement field is returned unchanged rather than reported as an error,
    // so an unmoved point means the field did not cover it
    if (transferred[i] == reference_points[i]) {
      unmoved++;
    }
  }

  const double mean_snap = total_snap / transferred.size();
  const double far_fraction = static_cast<double>(far_count) / transferred.size();
  // a registration that failed leaves many particles far from the surface, not just one outlier
  const bool suspect = far_fraction > TRANSFER_FAR_FRACTION_THRESHOLD || unmoved > 0;

  const std::string name = GetDomainName(domain);

  if (optimize_.m_verbosity_level > 0 || suspect) {
    SW_LOG("{}: transferred particles landed {:.3g} from the surface on average (worst {:.3g})", name, mean_snap,
           worst_snap);
  }

  if (unmoved > 0) {
    SW_WARN("{}: {} of {} transferred particles fell outside the registration field and did not move", name, unmoved,
            transferred.size());
  }

  if (far_fraction > TRANSFER_FAR_FRACTION_THRESHOLD) {
    SW_WARN("{}: {:.0f}% of transferred particles landed far from the surface, the registration may have failed", name,
            100.0 * far_fraction);
  }

  return mean_snap;
}

//---------------------------------------------------------------------------
void RegistrationInitializer::Run() {
  const int reference = ResolveRegistrationReference();
  optimize_.m_registration_reference_chosen = reference;

  SW_LOG("Spreading particles on reference shape {} ({})", reference,
         GetDomainName(reference * optimize_.m_domains_per_shape));

  SpreadParticlesOnReference(reference);

  // each shape is populated with a full set below, so the matrices can track them again
  optimize_.m_sampler->SetCorrespondenceMatricesSuspended(false);

  if (!optimize_.m_aborted) {
    TransferParticlesFromReference(reference);
  }

  // Only now does every shape hold its particles.  The matrices size themselves from the first
  // shape's domains, so they cannot be brought up to date any earlier: while particles were being
  // spread, only the reference (which is usually not the first shape) had any.
  auto* system = optimize_.m_sampler->GetParticleSystem();
  system->ResyncObservers();
  system->SynchronizePositions();

  // every shape now carries a full set of corresponding particles
  optimize_.m_sampler->SetCorrespondenceOn();

  optimize_.WritePointFiles();
  optimize_.WritePointFilesWithFeatures();
  optimize_.WriteTransformFile();
  optimize_.WriteTransformFiles();
  optimize_.WriteCuttingPlanePoints();

  if (optimize_.m_verbosity_level > 0) {
    SW_LOG("Finished registration based initialization");
  }
}

}  // namespace shapeworks
