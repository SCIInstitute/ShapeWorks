#include "MeshAssignThickness.h"

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

void compute_thickness(Mesh &mesh, Image &image, Image &dt, double threshold) {
  SW_LOG("Computing thickness with threshold {}", threshold);

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

    int max_steps = 500;
    int min_steps = 2;
    int steps = 0;

    float intensity = std::numeric_limits<float>::max();
    do {
      VectorPixelType gradient = interpolator->Evaluate(point);
      intensity = image.evaluate(point);

      // normalize the gradient
      float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
      gradient[0] /= norm * min_spacing;
      gradient[1] /= norm * min_spacing;
      gradient[2] /= norm * min_spacing;

      point[0] += gradient[0];
      point[1] += gradient[1];
      point[2] += gradient[2];

      steps++;
    } while ((intensity > threshold && max_steps-- > 0) || steps < min_steps);

    // compute distance between start and end points
    double distance = start.EuclideanDistanceTo(point);
    values->InsertValue(i, distance);
  }
  mesh.setField("thickness", values, Mesh::Point);
}
}  // namespace shapeworks::mesh
