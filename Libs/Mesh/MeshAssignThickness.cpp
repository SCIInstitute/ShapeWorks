#include "MeshAssignThickness.h"

#include <itkGradientImageFilter.h>

#include "Logging.h"
namespace shapeworks::mesh {

using VectorPixelType = itk::CovariantVector<float, 3>;
using GradientImageType = itk::Image<VectorPixelType, 3>;
using GradientFilterType = itk::GradientImageFilter<Image::ImageType>;

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

void assign_thickness(Mesh &mesh, const Image &image, const Image &dt, double threshold) {
  SW_LOG("Assign thickness with threshold {}", threshold);

  // compute gradient image from distance transform

  auto gradient_filter = GradientFilterType::New();
  gradient_filter->SetInput(dt.getITKImage());
  gradient_filter->Update();
  auto gradient_map = gradient_filter->GetOutput();

  auto poly_data = mesh.getVTKMesh();

  auto values = vtkSmartPointer<vtkDoubleArray>::New();
  values->SetNumberOfComponents(1);
  values->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  values->SetName("thickness");

  // Find the nearest neighbors to each point and compute distance between them
    for (int i = 0; i < mesh.numPoints(); i++) {
  //for (int i = 0; i < 10; i++) {
    Point3 point;
    poly_data->GetPoint(i, point.GetDataPointer());
    Point3 start = point;

    int max_steps = 500;
    int min_steps = 2;
    int steps = 0;

    float intensity = std::numeric_limits<float>::max();
    do {
      VectorPixelType gradient = get_value(gradient_map, point.GetDataPointer());
      intensity = get_value(image.getITKImage().GetPointer(), point.GetDataPointer());
      float dt_value = get_value(dt.getITKImage().GetPointer(), point.GetDataPointer());

      //SW_LOG("intensity: {}, dt: {}, gradient: ({}, {}, {})", intensity, dt_value, gradient[0], gradient[1],
        //     gradient[2]);

      // normalize the gradient
      float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
      gradient[0] /= norm;
      gradient[1] /= norm;
      gradient[2] /= norm;

      //      if (dt_value > 0) {
      //        point[0] -= gradient[0];
      //      point[1] -= gradient[1];
      //    point[2] -= gradient[2];
      //    }
      //  else {
      //  point[0] += gradient[0];
      // point[1] += gradient[1];
      // point[2] += gradient[2];
      //}

      point[0] += gradient[0];
      point[1] += gradient[1];
      point[2] += gradient[2];

      steps++;
    } while ((intensity > threshold && max_steps-- > 0) || steps < min_steps);

//    SW_TRACE(steps);
    // compute distance between start and end itk points
    double distance = start.EuclideanDistanceTo(point);
    values->InsertValue(i, distance);
  }
  mesh.setField("thickness", values, Mesh::Point);
}
}  // namespace shapeworks::mesh
