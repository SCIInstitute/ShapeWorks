#include "MeshComputeThickness.h"

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>

#include "Logging.h"
namespace shapeworks::mesh {

using VectorPixelType = itk::CovariantVector<float, 3>;
using GradientImageType = itk::Image<VectorPixelType, 3>;
using GradientFilterType = itk::GradientImageFilter<Image::ImageType>;
using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<GradientImageType, double>;

template <class TImageType>
static typename TImageType::PixelType get_value(TImageType *image, double *point) {
  typename TImageType::PointType p;
  p[0] = point[0];
  p[1] = point[1];
  p[2] = point[2];

  typename TImageType::IndexType index;

  image->TransformPhysicalPointToIndex(p, index);

  typename TImageType::RegionType region = image->GetLargestPossibleRegion();

  if (!region.IsInside(index)) {
    return itk::NumericTraits<typename TImageType::PixelType>::Zero;
  }

  return image->GetPixel(index);
}

void compute_thickness(Mesh &mesh, Image &image, Image &dt, double threshold, double min_dist, double max_dist) {
  SW_DEBUG("Computing thickness with threshold {}, min_dist {}, max_dist {}", threshold, min_dist, max_dist);

  // compute gradient image from distance transform
  auto gradient_filter = GradientFilterType::New();
  gradient_filter->SetInput(dt.getITKImage());
  gradient_filter->Update();
  auto gradient_map = gradient_filter->GetOutput();

  auto poly_data = mesh.getVTKMesh();

  // create array of thickness values
  auto values = vtkSmartPointer<vtkDoubleArray>::New();
  values->SetNumberOfComponents(1);
  values->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  values->SetName("thickness");

  auto interpolator = GradientInterpolatorType::New();
  interpolator->SetInputImage(gradient_map);

  Vector spacing = dt.spacing();
  auto min_spacing = std::min<double>({spacing[0], spacing[1], spacing[2]});

  for (int i = 0; i < mesh.numPoints(); i++) {
    Point3 point;
    poly_data->GetPoint(i, point.GetDataPointer());
    Point3 start = point;

    Point3 intensity_start = point;
    bool intensity_found = false;

    double last_distance = 0;

    bool should_stop = false;
    int steps = 0;
    float intensity = std::numeric_limits<float>::max();
    do {
      intensity = image.evaluate(point);
      if (!intensity_found && intensity > threshold) {
        intensity_start = point;
        intensity_found = true;
      }

      should_stop = false;

      double current_distance = start.EuclideanDistanceTo(point);

      // if we haven't found any intensity above the threshold within the min_dist, give up and call it a zero
      if (!intensity_found && current_distance > min_dist) {
        intensity_start = point;
        should_stop = true;
      }

      // if we have found intensity above the threshold, and we've gone past the max_dist, stop
      if (intensity_found && current_distance > max_dist) {
        should_stop = true;
      }

      // if we've hit the intensity area and are now past it, stop
      if (intensity_found && intensity < threshold) {
        should_stop = true;
      }

      // if we've passed the center and the DT is telling us to go back
      if (current_distance < last_distance) {
        should_stop = true;
      }
      last_distance = current_distance;

      if (steps > 1000) {  // fail-safe
        should_stop = true;
      }

      if (!should_stop) {
        // evaluate gradient
        VectorPixelType gradient = interpolator->Evaluate(point);

        // normalize the gradient
        float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
        gradient[0] /= norm;
        gradient[1] /= norm;
        gradient[2] /= norm;

        gradient[0] *= min_spacing * 0.1;
        gradient[1] *= min_spacing * 0.1;
        gradient[2] *= min_spacing * 0.1;

        // take step
        point[0] += gradient[0];
        point[1] += gradient[1];
        point[2] += gradient[2];

        steps++;
      }

    } while (!should_stop);

    // compute distance between start and end points
    double distance = intensity_start.EuclideanDistanceTo(point);
    values->InsertValue(i, distance);
  }
  mesh.setField("thickness", values, Mesh::Point);
}
}  // namespace shapeworks::mesh
