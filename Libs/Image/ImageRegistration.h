#pragma once

#include <itkCompositeTransform.h>

#include <memory>
#include <vector>

#include "Image.h"
#include "Shapeworks.h"

namespace shapeworks {

/**
 * \class ImageRegistration
 * \ingroup Group-Image
 *
 * Intensity based registration of one image to another, following the same recipe as the ANTs
 * `SyNRA` pipeline: a multi-resolution rigid stage, then affine, then symmetric normalization
 * (SyN) with a neighborhood cross correlation metric.
 *
 * The resulting transform maps points from the fixed image space into the moving image space.
 * This is the ITK convention: resampling walks the fixed (output) grid and pulls samples from the
 * moving image.  So to carry a point set from image A onto image B, register with A as the fixed
 * image and B as the moving image, then call transform_points().
 *
 * Distance transforms rather than binary images are the expected input.  Use make_registration_image()
 * to clamp a distance transform to a band around the surface; unbounded far-field values otherwise
 * dominate the metric.
 */
class ImageRegistration {
 public:
  //! Which stages to run.  Each mode includes the stages before it.
  enum class TransformType {
    Rigid,   //!< rigid only
    Affine,  //!< rigid, then affine
    SyN      //!< rigid, then affine, then symmetric normalization
  };

  using CompositeTransformType = itk::CompositeTransform<double, 3>;

  ImageRegistration();
  ~ImageRegistration();

  //! set which registration stages to run (default SyN)
  void set_transform_type(TransformType type);

  //! set the SyN gradient step size (default 0.25, ANTs `grad_step`)
  void set_gradient_step(double step);

  //! set the variance for gaussian smoothing of the SyN update field (default 3.0, ANTs `flow_sigma`)
  void set_update_field_variance(double variance);

  //! set the variance for gaussian smoothing of the SyN total field (default 0.0, ANTs `total_sigma`)
  void set_total_field_variance(double variance);

  //! set the deformable stage's per-level iteration counts, coarsest level first (default {40, 20, 0})
  void set_iterations(const std::vector<unsigned int>& iterations);

  //! set the deformable shrink factors per level, coarsest first (default {4, 2, 1}); must match
  //! set_iterations() size
  void set_shrink_factors(const std::vector<unsigned int>& shrink_factors);

  //! set the deformable gaussian smoothing sigmas (in physical units) per level, coarsest first
  //! (default {2, 1, 0})
  void set_smoothing_sigmas(const std::vector<double>& sigmas);

  /**
   * @brief Set the schedule used by the rigid and affine stages.
   *
   * These stages need a coarser and much longer schedule than the deformable one.  Their coarse
   * levels are cheap, and an unconverged linear stage leaves the deformable stage with a starting
   * point it cannot recover from.  Defaults follow ANTs: iterations {2100, 1200, 1200, 10}, shrink
   * factors {6, 4, 2, 1}, sigmas {3, 2, 1, 0}.
   */
  void set_linear_iterations(const std::vector<unsigned int>& iterations);
  void set_linear_shrink_factors(const std::vector<unsigned int>& shrink_factors);
  void set_linear_smoothing_sigmas(const std::vector<double>& sigmas);

  //! set the radius of the neighborhood correlation metric window used by the SyN stage (default 4)
  void set_correlation_radius(unsigned int radius);

  //! run the registration.  Throws std::runtime_error if any stage fails.
  void run(const Image& fixed, const Image& moving);

  //! map points from fixed image space into moving image space.  run() must have been called.
  std::vector<Point3> transform_points(const std::vector<Point3>& points) const;

  //! the moving image resampled onto the fixed image grid.  Useful for inspecting registration quality.
  Image warped_moving() const;

  //! the composed transform from all stages, mapping fixed space to moving space
  CompositeTransformType::Pointer get_transform() const;

  /**
   * @brief Write the transform computed by run() so that it can be reused.
   *
   * The transform depends only on the two images and the registration settings, so a saved one can
   * stand in for re-running the registration whenever those are unchanged.
   */
  void save_transform(const std::string& filename) const;

  /**
   * @brief Read a transform written by save_transform(), in place of running the registration.
   *
   * transform_points() and get_transform() work afterwards exactly as they would have after run().
   * warped_moving() does not, since no images were supplied.
   *
   * @return whether the transform was read
   */
  bool load_transform(const std::string& filename);

  /**
   * @brief Prepare a distance transform for registration by clamping and rescaling it.
   *
   * Values are clamped to +/- band and then mapped to [0,1].  This keeps the metric focused on the
   * region near the surface, where the distance transform is informative, instead of on the
   * unbounded values far away from it.
   *
   * @param dt a distance transform
   * @param band the half-width of the retained band, in physical units
   * @return the clamped and rescaled image
   */
  static Image make_registration_image(const Image& dt, double band = 5.0);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace shapeworks
