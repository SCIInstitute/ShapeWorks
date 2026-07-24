#include "ImageRegistration.h"

#include "ShapeworksUtils.h"

#include <itkANTSNeighborhoodCorrelationImageToImageMetricv4.h>
#include <itkAffineTransform.h>
#include <itkCenteredTransformInitializer.h>
#include <itkConjugateGradientLineSearchOptimizerv4.h>
#include <itkDisplacementFieldTransform.h>
#include <itkDisplacementFieldTransformParametersAdaptor.h>
#include <itkHDF5TransformIOFactory.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkRegistrationParameterScalesFromPhysicalShift.h>
#include <itkResampleImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkSyNImageRegistrationMethod.h>
#include <itkVersorRigid3DTransform.h>

#include <algorithm>
#include <functional>

namespace shapeworks {

namespace {

using ImageType = Image::ImageType;
using RigidTransformType = itk::VersorRigid3DTransform<double>;
using AffineTransformType = itk::AffineTransform<double, 3>;
using DisplacementFieldTransformType = itk::DisplacementFieldTransform<double, 3>;
using CompositeTransformType = itk::CompositeTransform<double, 3>;
// Both inputs are prepared the same way by make_registration_image, so they are effectively the same
// modality.  A mutual information metric, which ANTs uses because it registers arbitrary modalities,
// is poorly conditioned here and lets the line search diverge; matching intensities directly is both
// correct for this input and far more stable.
using LinearMetricType = itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType>;
using CorrelationMetricType = itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<ImageType, ImageType>;
using OptimizerType = itk::ConjugateGradientLineSearchOptimizerv4Template<double>;

constexpr double kLineSearchLowerLimit = 0.0;
constexpr double kLineSearchUpperLimit = 2.0;
constexpr double kLineSearchEpsilon = 0.2;

// A heavily shrunk level of an already small image carries almost no signal, and the optimizer
// responds by taking a large wrong step that the finer levels never recover from.  Never shrink a
// dimension below this many voxels.
constexpr unsigned int kMinimumLevelSize = 16;

// The furthest a single optimizer step may move a point, as a fraction of the largest physical
// dimension of the images being registered.
constexpr double kMaximumStepFraction = 0.1;

//---------------------------------------------------------------------------
/// Reduce the requested shrink factors so that no level shrinks the image below kMinimumLevelSize.
std::vector<unsigned int> clamp_shrink_factors(const std::vector<unsigned int>& shrink_factors,
                                               const ImageType* image) {
  const auto size = image->GetBufferedRegion().GetSize();
  const auto smallest = std::min({size[0], size[1], size[2]});
  const auto largest_useful = std::max(1u, static_cast<unsigned int>(smallest / kMinimumLevelSize));

  std::vector<unsigned int> clamped;
  clamped.reserve(shrink_factors.size());
  for (const auto factor : shrink_factors) {
    clamped.push_back(std::min(std::max(1u, factor), largest_useful));
  }
  return clamped;
}

//---------------------------------------------------------------------------
/// Build an optimizer with the scale estimator wired to the given metric, so that rotation,
/// translation and scaling parameters all step by a comparable physical distance.
template <typename TMetric>
OptimizerType::Pointer make_optimizer(TMetric* metric, unsigned int iterations, const ImageType* fixed) {
  using ScalesEstimatorType = itk::RegistrationParameterScalesFromPhysicalShift<TMetric>;
  auto scales_estimator = ScalesEstimatorType::New();
  scales_estimator->SetMetric(metric);
  scales_estimator->SetTransformForward(true);

  // The metric is flat wherever the two shapes do not overlap at all, so an unbounded step can throw
  // the transform into that dead zone, where there is no gradient left to bring it back.  Cap how far
  // a single step may move any point, relative to the size of what is being registered.
  const auto size = fixed->GetBufferedRegion().GetSize();
  const auto spacing = fixed->GetSpacing();
  double largest_extent = 0.0;
  for (unsigned int i = 0; i < 3; i++) {
    largest_extent = std::max(largest_extent, size[i] * spacing[i]);
  }

  auto optimizer = OptimizerType::New();
  optimizer->SetNumberOfIterations(iterations);
  optimizer->SetScalesEstimator(scales_estimator);
  optimizer->SetMaximumStepSizeInPhysicalUnits(largest_extent * kMaximumStepFraction);
  optimizer->SetDoEstimateLearningRateOnce(false);
  optimizer->SetDoEstimateLearningRateAtEachIteration(true);
  optimizer->SetLowerLimit(kLineSearchLowerLimit);
  optimizer->SetUpperLimit(kLineSearchUpperLimit);
  optimizer->SetEpsilon(kLineSearchEpsilon);
  // keep whatever scored best rather than wherever the last step happened to land
  optimizer->SetReturnBestParametersAndValue(true);
  return optimizer;
}

//---------------------------------------------------------------------------
/// ITK drives every resolution level with a single optimizer, so the iteration count would otherwise
/// be the same at all of them.  This resets it as each level starts, which lets the coarse levels --
/// where iterating is cheap and most of the alignment is won -- run far longer than the finest one.
template <typename TRegistration>
class LevelIterationCommand : public itk::Command {
 public:
  using Self = LevelIterationCommand;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro(Self);

  void set_iterations(const std::vector<unsigned int>& iterations) { iterations_ = iterations; }

  void Execute(itk::Object* caller, const itk::EventObject& event) override {
    Execute(const_cast<const itk::Object*>(caller), event);
  }

  void Execute(const itk::Object* caller, const itk::EventObject& event) override {
    if (!itk::MultiResolutionIterationEvent().CheckEvent(&event)) {
      return;
    }
    const auto* registration = dynamic_cast<const TRegistration*>(caller);
    if (!registration) {
      return;
    }
    const auto level = registration->GetCurrentLevel();
    if (level >= iterations_.size()) {
      return;
    }
    auto* optimizer = const_cast<OptimizerType*>(dynamic_cast<const OptimizerType*>(registration->GetOptimizer()));
    if (optimizer) {
      optimizer->SetNumberOfIterations(iterations_[level]);
    }
  }

 protected:
  LevelIterationCommand() = default;

 private:
  std::vector<unsigned int> iterations_;
};

//---------------------------------------------------------------------------
/// Convert our per-level settings into the array types the ITK registration methods expect.
template <typename TRegistration>
void apply_multi_resolution_schedule(TRegistration* registration, const std::vector<unsigned int>& shrink_factors,
                                     const std::vector<double>& smoothing_sigmas) {
  const auto levels = static_cast<unsigned int>(shrink_factors.size());

  typename TRegistration::ShrinkFactorsArrayType shrink_array;
  shrink_array.SetSize(levels);
  typename TRegistration::SmoothingSigmasArrayType sigma_array;
  sigma_array.SetSize(levels);

  for (unsigned int level = 0; level < levels; level++) {
    shrink_array[level] = shrink_factors[level];
    sigma_array[level] = smoothing_sigmas[level];
  }

  registration->SetNumberOfLevels(levels);
  registration->SetShrinkFactorsPerLevel(shrink_array);
  registration->SetSmoothingSigmasPerLevel(sigma_array);
  registration->SmoothingSigmasAreSpecifiedInPhysicalUnitsOn();
}

}  // namespace

//---------------------------------------------------------------------------
class ImageRegistration::Impl {
 public:
  TransformType transform_type = TransformType::SyN;
  double gradient_step = 0.25;
  double update_field_variance = 3.0;
  double total_field_variance = 0.0;

  // Deformable schedule.  The finest level is deliberately cheap: the displacement field is smooth,
  // so little is gained there while it is by far the most expensive level to iterate on.
  std::vector<unsigned int> iterations = {40, 20, 0};
  std::vector<unsigned int> shrink_factors = {4, 2, 1};
  std::vector<double> smoothing_sigmas = {2.0, 1.0, 0.0};

  // Linear (rigid and affine) schedule, which starts coarser and iterates much harder than the
  // deformable one.  Those coarse levels are cheap, and skimping on them leaves the linear stages
  // unconverged, which the deformable stage then cannot recover from.
  std::vector<unsigned int> linear_iterations = {2100, 1200, 1200, 10};
  std::vector<unsigned int> linear_shrink_factors = {6, 4, 2, 1};
  std::vector<double> linear_smoothing_sigmas = {3.0, 2.0, 1.0, 0.0};

  //! shrink factors reduced to suit the size of the images actually being registered
  std::vector<unsigned int> effective_shrink_factors;
  std::vector<unsigned int> effective_linear_shrink_factors;
  unsigned int correlation_radius = 4;

  CompositeTransformType::Pointer composite;
  ImageType::Pointer fixed;
  ImageType::Pointer moving;

  void run_rigid();
  void run_affine();
  void run_syn();

  //! per-level adaptors that resample the displacement field as the SyN stage moves up the pyramid
  std::vector<typename itk::SyNImageRegistrationMethod<ImageType, ImageType,
                                                       DisplacementFieldTransformType>::TransformParametersAdaptorPointer>
  make_field_adaptors() const;
};

//---------------------------------------------------------------------------
void ImageRegistration::Impl::run_rigid() {
  auto rigid = RigidTransformType::New();

  // seed the rigid transform by aligning the centers of mass, which gets the optimizer close enough
  // that it does not have to search for gross translation
  using InitializerType = itk::CenteredTransformInitializer<RigidTransformType, ImageType, ImageType>;
  auto initializer = InitializerType::New();
  initializer->SetTransform(rigid);
  initializer->SetFixedImage(fixed);
  initializer->SetMovingImage(moving);
  initializer->MomentsOn();
  initializer->InitializeTransform();

  auto metric = LinearMetricType::New();

  using RegistrationType = itk::ImageRegistrationMethodv4<ImageType, ImageType, RigidTransformType>;
  auto registration = RegistrationType::New();
  registration->SetFixedImage(fixed);
  registration->SetMovingImage(moving);
  registration->SetMetric(metric);
  registration->SetInitialTransform(rigid);
  registration->InPlaceOn();
  registration->SetOptimizer(make_optimizer(metric.GetPointer(), linear_iterations.front(), fixed));
  apply_multi_resolution_schedule(registration.GetPointer(), effective_linear_shrink_factors, linear_smoothing_sigmas);

  auto level_command = LevelIterationCommand<RegistrationType>::New();
  level_command->set_iterations(linear_iterations);
  registration->AddObserver(itk::MultiResolutionIterationEvent(), level_command);

  registration->Update();

  composite->AddTransform(registration->GetModifiableTransform());
}

//---------------------------------------------------------------------------
void ImageRegistration::Impl::run_affine() {
  auto metric = LinearMetricType::New();

  using RegistrationType = itk::ImageRegistrationMethodv4<ImageType, ImageType, AffineTransformType>;
  auto registration = RegistrationType::New();
  registration->SetFixedImage(fixed);
  registration->SetMovingImage(moving);
  registration->SetMetric(metric);
  // the rigid result carries the coarse alignment; the affine stage only has to find the residual
  registration->SetMovingInitialTransform(composite);
  registration->SetInitialTransform(AffineTransformType::New());
  registration->InPlaceOn();
  registration->SetOptimizer(make_optimizer(metric.GetPointer(), linear_iterations.front(), fixed));
  apply_multi_resolution_schedule(registration.GetPointer(), effective_linear_shrink_factors, linear_smoothing_sigmas);

  auto level_command = LevelIterationCommand<RegistrationType>::New();
  level_command->set_iterations(linear_iterations);
  registration->AddObserver(itk::MultiResolutionIterationEvent(), level_command);

  registration->Update();

  composite->AddTransform(registration->GetModifiableTransform());
}

//---------------------------------------------------------------------------
auto ImageRegistration::Impl::make_field_adaptors() const
    -> std::vector<typename itk::SyNImageRegistrationMethod<
        ImageType, ImageType, DisplacementFieldTransformType>::TransformParametersAdaptorPointer> {
  using RegistrationType = itk::SyNImageRegistrationMethod<ImageType, ImageType, DisplacementFieldTransformType>;
  using AdaptorType = itk::DisplacementFieldTransformParametersAdaptor<DisplacementFieldTransformType>;

  std::vector<typename RegistrationType::TransformParametersAdaptorPointer> adaptors;

  for (const auto shrink_factor : effective_shrink_factors) {
    // the displacement field lives on the virtual (fixed) domain, so mirror how the registration
    // shrinks that domain at this level
    auto shrink_filter = itk::ShrinkImageFilter<ImageType, ImageType>::New();
    shrink_filter->SetShrinkFactors(shrink_factor);
    shrink_filter->SetInput(fixed);
    shrink_filter->Update();
    const auto level_image = shrink_filter->GetOutput();

    auto adaptor = AdaptorType::New();
    adaptor->SetRequiredSpacing(level_image->GetSpacing());
    adaptor->SetRequiredSize(level_image->GetBufferedRegion().GetSize());
    adaptor->SetRequiredDirection(level_image->GetDirection());
    adaptor->SetRequiredOrigin(level_image->GetOrigin());

    adaptors.push_back(adaptor.GetPointer());
  }

  return adaptors;
}

//---------------------------------------------------------------------------
void ImageRegistration::Impl::run_syn() {
  auto metric = CorrelationMetricType::New();
  CorrelationMetricType::RadiusType radius;
  radius.Fill(correlation_radius);
  metric->SetRadius(radius);

  // the output transform must own a displacement field before registration starts; allocate an
  // identity (all zero) field over the fixed image domain
  using DisplacementFieldType = DisplacementFieldTransformType::DisplacementFieldType;
  auto make_zero_field = [this]() {
    auto field = DisplacementFieldType::New();
    field->CopyInformation(this->fixed);
    field->SetRegions(this->fixed->GetBufferedRegion());
    field->AllocateInitialized();
    return field;
  };

  auto output_transform = DisplacementFieldTransformType::New();
  output_transform->SetDisplacementField(make_zero_field());
  output_transform->SetInverseDisplacementField(make_zero_field());

  using RegistrationType = itk::SyNImageRegistrationMethod<ImageType, ImageType, DisplacementFieldTransformType>;
  auto registration = RegistrationType::New();
  registration->SetFixedImage(fixed);
  registration->SetMovingImage(moving);
  registration->SetMetric(metric);
  registration->SetMovingInitialTransform(composite);
  registration->SetInitialTransform(output_transform);
  registration->InPlaceOn();

  typename RegistrationType::NumberOfIterationsArrayType iteration_array;
  iteration_array.SetSize(iterations.size());
  for (size_t level = 0; level < iterations.size(); level++) {
    iteration_array[level] = iterations[level];
  }
  registration->SetNumberOfIterationsPerLevel(iteration_array);

  apply_multi_resolution_schedule(registration.GetPointer(), effective_shrink_factors, smoothing_sigmas);
  auto adaptors = make_field_adaptors();  // ITK takes this by non-const reference
  registration->SetTransformParametersAdaptorsPerLevel(adaptors);

  registration->SetLearningRate(gradient_step);
  registration->SetGaussianSmoothingVarianceForTheUpdateField(update_field_variance);
  registration->SetGaussianSmoothingVarianceForTheTotalField(total_field_variance);

  registration->Update();

  composite->AddTransform(registration->GetModifiableTransform());
}

//---------------------------------------------------------------------------
ImageRegistration::ImageRegistration() : impl_(std::make_unique<Impl>()) {}

//---------------------------------------------------------------------------
ImageRegistration::~ImageRegistration() = default;

//---------------------------------------------------------------------------
void ImageRegistration::set_transform_type(TransformType type) { impl_->transform_type = type; }

//---------------------------------------------------------------------------
void ImageRegistration::set_gradient_step(double step) { impl_->gradient_step = step; }

//---------------------------------------------------------------------------
void ImageRegistration::set_update_field_variance(double variance) { impl_->update_field_variance = variance; }

//---------------------------------------------------------------------------
void ImageRegistration::set_total_field_variance(double variance) { impl_->total_field_variance = variance; }

//---------------------------------------------------------------------------
void ImageRegistration::set_iterations(const std::vector<unsigned int>& iterations) {
  if (iterations.empty()) {
    throw std::invalid_argument("registration iterations must not be empty");
  }
  impl_->iterations = iterations;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_shrink_factors(const std::vector<unsigned int>& shrink_factors) {
  if (shrink_factors.empty()) {
    throw std::invalid_argument("registration shrink factors must not be empty");
  }
  impl_->shrink_factors = shrink_factors;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_smoothing_sigmas(const std::vector<double>& sigmas) {
  if (sigmas.empty()) {
    throw std::invalid_argument("registration smoothing sigmas must not be empty");
  }
  impl_->smoothing_sigmas = sigmas;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_linear_iterations(const std::vector<unsigned int>& iterations) {
  if (iterations.empty()) {
    throw std::invalid_argument("linear registration iterations must not be empty");
  }
  impl_->linear_iterations = iterations;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_linear_shrink_factors(const std::vector<unsigned int>& shrink_factors) {
  if (shrink_factors.empty()) {
    throw std::invalid_argument("linear registration shrink factors must not be empty");
  }
  impl_->linear_shrink_factors = shrink_factors;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_linear_smoothing_sigmas(const std::vector<double>& sigmas) {
  if (sigmas.empty()) {
    throw std::invalid_argument("linear registration smoothing sigmas must not be empty");
  }
  impl_->linear_smoothing_sigmas = sigmas;
}

//---------------------------------------------------------------------------
void ImageRegistration::set_correlation_radius(unsigned int radius) { impl_->correlation_radius = radius; }

//---------------------------------------------------------------------------
void ImageRegistration::run(const Image& fixed, const Image& moving) {
  if (impl_->iterations.size() != impl_->shrink_factors.size() ||
      impl_->iterations.size() != impl_->smoothing_sigmas.size()) {
    throw std::invalid_argument("registration iterations, shrink factors and smoothing sigmas must be the same length");
  }
  if (impl_->linear_iterations.size() != impl_->linear_shrink_factors.size() ||
      impl_->linear_iterations.size() != impl_->linear_smoothing_sigmas.size()) {
    throw std::invalid_argument(
        "linear registration iterations, shrink factors and smoothing sigmas must be the same length");
  }

  impl_->fixed = fixed.getITKImage();
  impl_->moving = moving.getITKImage();
  impl_->composite = CompositeTransformType::New();
  impl_->effective_shrink_factors = clamp_shrink_factors(impl_->shrink_factors, impl_->fixed);
  impl_->effective_linear_shrink_factors = clamp_shrink_factors(impl_->linear_shrink_factors, impl_->fixed);

  auto run_stage = [](const char* name, const std::function<void()>& stage) {
    try {
      stage();
    } catch (const itk::ExceptionObject& e) {
      throw std::runtime_error(std::string("image registration failed during the ") + name + " stage: " + e.what());
    }
  };

  run_stage("rigid", [this]() { impl_->run_rigid(); });

  if (impl_->transform_type == TransformType::Affine || impl_->transform_type == TransformType::SyN) {
    run_stage("affine", [this]() { impl_->run_affine(); });
  }

  if (impl_->transform_type == TransformType::SyN) {
    run_stage("SyN", [this]() { impl_->run_syn(); });
  }
}

//---------------------------------------------------------------------------
std::vector<Point3> ImageRegistration::transform_points(const std::vector<Point3>& points) const {
  if (!impl_->composite) {
    throw std::runtime_error("transform_points called before run()");
  }

  std::vector<Point3> transformed;
  transformed.reserve(points.size());
  for (const auto& point : points) {
    transformed.push_back(impl_->composite->TransformPoint(point));
  }
  return transformed;
}

//---------------------------------------------------------------------------
Image ImageRegistration::warped_moving() const {
  if (!impl_->composite) {
    throw std::runtime_error("warped_moving called before run()");
  }

  auto resampler = itk::ResampleImageFilter<ImageType, ImageType>::New();
  resampler->SetInput(impl_->moving);
  resampler->SetTransform(impl_->composite);
  resampler->SetOutputParametersFromImage(impl_->fixed);
  resampler->SetInterpolator(itk::LinearInterpolateImageFunction<ImageType, double>::New());
  resampler->SetDefaultPixelValue(0);
  resampler->Update();

  return Image(resampler->GetOutput());
}

//---------------------------------------------------------------------------
ImageRegistration::CompositeTransformType::Pointer ImageRegistration::get_transform() const {
  if (!impl_->composite) {
    throw std::runtime_error("get_transform called before run()");
  }
  return impl_->composite;
}

//---------------------------------------------------------------------------
/// The displacement field is far larger than everything else combined, so it is kept beside the
/// linear transforms as an image of floats rather than inside the transform file as raw doubles.
/// Float carries the field to well under a micron, which is far finer than it is accurate.
static std::string displacement_field_path(const std::string& filename) { return filename + ".field.nrrd"; }


//---------------------------------------------------------------------------
void ImageRegistration::save_transform(const std::string& filename) const {
  if (!impl_->composite) {
    throw std::runtime_error("save_transform called before run()");
  }

  itk::HDF5TransformIOFactory::RegisterOneFactory();

  using StoredFieldType = itk::Image<itk::Vector<float, 3>, 3>;

  auto linear = CompositeTransformType::New();
  StoredFieldType::Pointer stored_field;

  for (size_t i = 0; i < impl_->composite->GetNumberOfTransforms(); i++) {
    auto transform = impl_->composite->GetNthTransform(i);
    auto* field = dynamic_cast<DisplacementFieldTransformType*>(transform.GetPointer());
    if (!field) {
      linear->AddTransform(transform);
      continue;
    }
    // only the forward field is needed to carry points across; the inverse would double the size
    using CastType = itk::CastImageFilter<DisplacementFieldTransformType::DisplacementFieldType, StoredFieldType>;
    // stored at full resolution: the field carries real detail, and halving it costs a sixth of the
    // registration's own accuracy, which is the opposite of the point
    auto cast = CastType::New();
    cast->SetInput(field->GetDisplacementField());
    cast->Update();
    stored_field = cast->GetOutput();
  }

  try {
    auto writer = itk::TransformFileWriterTemplate<double>::New();
    writer->SetFileName(filename);
    writer->SetInput(linear);
    writer->Update();

    if (stored_field) {
      auto field_writer = itk::ImageFileWriter<StoredFieldType>::New();
      field_writer->SetFileName(displacement_field_path(filename));
      field_writer->SetInput(stored_field);
      field_writer->UseCompressionOn();
      field_writer->Update();
    }
  } catch (const itk::ExceptionObject& e) {
    throw std::runtime_error(std::string("unable to write transform \"") + filename + "\": " + e.what());
  }
}

//---------------------------------------------------------------------------
bool ImageRegistration::load_transform(const std::string& filename) {
  itk::HDF5TransformIOFactory::RegisterOneFactory();

  using StoredFieldType = itk::Image<itk::Vector<float, 3>, 3>;

  auto composite = CompositeTransformType::New();
  try {
    auto reader = itk::TransformFileReaderTemplate<double>::New();
    reader->SetFileName(filename);
    reader->Update();

    for (const auto& transform : *reader->GetTransformList()) {
      auto* casted = dynamic_cast<CompositeTransformType::TransformType*>(transform.GetPointer());
      if (!casted) {
        return false;
      }
      // a composite reads back as its individual transforms, in the order they were added
      if (auto* nested = dynamic_cast<CompositeTransformType*>(casted)) {
        for (size_t i = 0; i < nested->GetNumberOfTransforms(); i++) {
          composite->AddTransform(nested->GetNthTransform(i));
        }
      } else {
        composite->AddTransform(casted);
      }
    }

    const auto field_file = displacement_field_path(filename);
    if (ShapeWorksUtils::file_exists(field_file)) {
      auto field_reader = itk::ImageFileReader<StoredFieldType>::New();
      field_reader->SetFileName(field_file);
      field_reader->Update();

      using CastType = itk::CastImageFilter<StoredFieldType, DisplacementFieldTransformType::DisplacementFieldType>;
      auto cast = CastType::New();
      cast->SetInput(field_reader->GetOutput());
      cast->Update();

      auto field_transform = DisplacementFieldTransformType::New();
      field_transform->SetDisplacementField(cast->GetOutput());
      composite->AddTransform(field_transform);
    }
  } catch (const itk::ExceptionObject&) {
    return false;  // a cache that cannot be read is simply not a cache hit
  }

  if (composite->GetNumberOfTransforms() == 0) {
    return false;
  }

  impl_->composite = composite;
  return true;
}

//---------------------------------------------------------------------------
Image ImageRegistration::make_registration_image(const Image& dt, double band) {
  if (band <= 0.0) {
    throw std::invalid_argument("band must be positive");
  }

  Image result(dt);
  auto image = result.getITKImage();

  // A distance transform is only meaningful near the surface; far away it grows without bound and
  // would otherwise dominate the similarity metric.  Clamp to the band and rescale to [0,1].
  itk::ImageRegionIterator<ImageType> it(image, image->GetRequestedRegion());
  for (it.GoToBegin(); !it.IsAtEnd(); ++it) {
    const auto clamped = std::clamp(static_cast<double>(it.Get()), -band, band);
    it.Set(static_cast<Image::PixelType>((clamped + band) / (2.0 * band)));
  }

  return result;
}

}  // namespace shapeworks
